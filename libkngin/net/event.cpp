#ifdef _WIN32
#else
#include <sys/eventfd.h>
#endif
#include "core/common.h"
#include "core/system_error.h"
#include "net/event.h"
#include "net/epoller_event.h"
#include "net/event_loop.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/net/event.cpp"

KNGIN_NAMESPACE_K_BEGIN

event::event (event_loop_pimpl_ptr _loop,
              event_handler &&_event_handler)
    try
    : epoller_event(::eventfd(0, EFD_CLOEXEC)),
      m_loop(_loop),
      m_event_handler(std::move(_event_handler))
{
    arg_check(m_loop && m_event_handler);
    if (FD_INVALID(m_fd))
        throw k::system_error("::eventfd() erorr");
    enable_read();
} catch (...) {
    log_fatal("event::event() error");
    throw;
}

event::~event() KNGIN_NOEXCP
{
    if (is_single_ref_ptr(m_loop))
        return; // removed
    ignore_excp(
        if (m_loop->registed(self()))
            m_loop->remove_event(self());
        this->close();
    );
}

void
event::notify ()
{
    char _arr[8];
    in_buffer(_arr, 8).write_uint64(1);
    this->writen(out_buffer(_arr, 8)); // blocked
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
    this->readn(_buf); // blocked

    if (m_event_handler)
        log_excp_error(
            m_event_handler(),
            "event::m_event_handler() error"
        );
}

KNGIN_NAMESPACE_K_END
