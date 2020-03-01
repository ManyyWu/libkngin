#include "core/base/define.h"
#ifdef KNGIN_FLAG_HAVE_EVENTFD

#ifdef _WIN32
#else
#include <sys/eventfd.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/event/event.h"
#include "core/event/event_loop.h"

#define KNGIN_FILENAME "libkngin/core/event/event.cpp"

KNGIN_NAMESPACE_K_BEGIN

event::event (event_handler &&event_handler)
    try
    : epoller_event(::eventfd(0, EFD_CLOEXEC), 
                    epoller_event::EVENT_TYPE_EVENT),
      event_handler_(std::move(event_handler))
{
    arg_check(event_handler_);
    if (invalid())
        throw k::system_error("::eventfd() erorr");
    enable_read();
} catch (...) {
    log_fatal("event::event() error");
    throw;
}

event::~event() noexcept
{
    if (registed())
        log_warning("the timer must be closed"
                    " before object disconstructing");
    if (valid())
        filefd::close();
}

void
event::notify ()
{
    assert(valid());
    char arr[8];
    in_buffer(arr, 8).write_uint64(1);
    log_excp_fatal(
        this->writen(out_buffer(arr, 8)), // blocked
        "event::writen() error"
    );
}

void
event::close ()
{
    if (valid())
        filefd::close();
}

void
event::on_events (event_loop &loop, uint32_t flags)
{
    try {
        if ((EPOLLHUP | EPOLLERR | EPOLLIN) & flags)
            this->on_read();
    } catch (std::exception &e) {
        log_fatal("caught an exception in event::on_event(), %s", e.what());
        throw;
    } catch (...) {
        log_fatal("caught an undefined exception in event::on_event()");
        throw;
    }
}

void
event::on_read ()
{
    char arr[8];
    in_buffer buf(arr, 8);
    log_excp_fatal(
        this->readn(buf), // blocked
        "event::readn() error"
    );
    //log_debug("event read %" PRIu64, out_buffer(arr, 8).peek_uint64());

    if (event_handler_) {
        log_excp_error(
            event_handler_(),
            "event::event_handler_() error"
        );
    }
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EVENTFD */
