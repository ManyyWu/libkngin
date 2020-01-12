#ifdef _WIN32
#else
#include <sys/eventfd.h>
#endif
#include "core/common.h"
#include "core/system_error.h"
#include "net/event.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/event.cpp"

KNGIN_NAMESPACE_K_BEGIN

event::event (event_handler &&_event_handler)
    try
    : epoller_event(::eventfd(0, EFD_CLOEXEC), 
                    epoller_event::EVENT_TYPE_EVENT),
      m_event_handler(std::move(_event_handler))
{
    arg_check(m_event_handler);
    if (invalid())
        throw k::system_error("::eventfd() erorr");
    enable_read();
} catch (...) {
    log_fatal("event::event() error");
    throw;
}

event::~event() KNGIN_NOEXCP
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
    char _arr[8];
    in_buffer(_arr, 8).write_uint64(1);
    log_excp_fatal(
        this->writen(out_buffer(_arr, 8)), // blocked
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
event::on_error ()
{
    on_read();
}

void
event::on_read ()
{
    char _arr[8];
    in_buffer _buf(_arr, 8);
    log_excp_fatal(
        this->readn(_buf), // blocked
        "event::readn() error"
    );
    //log_debug("event read %" PRIu64, out_buffer(_arr, 8).peek_uint64());

    if (m_event_handler) {
        log_excp_error(
            m_event_handler(),
            "event::m_event_handler() error"
        );
    }
}

KNGIN_NAMESPACE_K_END
