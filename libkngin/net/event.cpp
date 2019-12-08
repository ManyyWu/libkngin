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

event::pimpl::pimpl (event_loop_pimpl_ptr _loop,
                     event_handler &&_event_handler)
    try
    : epoller_event(::eventfd(0, EFD_CLOEXEC)),
      m_loop(_loop),
      m_event_handler(std::move(_event_handler))
{
    arg_check(m_loop);
    if (FD_INVALID(m_fd))
        throw k::system_error("::eventfd() erorr");
    enable_read();
} catch (...) {
    log_fatal("event::event() error");
    throw;
}

event::pimpl::~pimpl() KNGIN_NOEXP
{
    if (is_single_ref_ptr(m_loop))
        return; // removed
    ignore_exp(
        if (m_loop->registed(self()))
            m_loop->remove_event(self());
        this->close();
    );
}

void
event::pimpl::notify ()
{
    char _arr[8];
    in_buffer(_arr, 8).write_uint64(1);
    this->writen(out_buffer(_arr, 8)); // blocked
}

void
event::pimpl::on_error ()
{
    on_read();
}

void
event::pimpl::on_read ()
{
    char _arr[8];
    in_buffer _buf(_arr, 8);
    this->readn(_buf); // blocked

    if (m_event_handler)
        ignore_exp(m_event_handler());
}

event::event (event_loop_pimpl_ptr _loop, event_handler &&_event_handler)
    : m_pimpl(std::make_shared<event_pimpl>(
                  _loop, std::move(_event_handler)))
{
}

event::event  (event_loop &_loop, event_handler &&_event_handler)
    : m_pimpl(std::make_shared<event_pimpl>(
                  _loop.pimpl(), std::move(_event_handler)))
{
}

KNGIN_NAMESPACE_K_END
