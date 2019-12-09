#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <functional>
#include <cassert>
#include <memory>
#include <system_error>
#include "core/noncopyable.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

class listener
    : public epoller_event,
      public std::enable_shared_from_this<listener> {
public:
    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (std::error_code)> close_handler;

public:
    listener      () = delete;

    listener      (event_loop &_loop, k::socket &&_socket);

    virtual
    ~listener     () KNGIN_NOEXCP;

public:
    void
    bind          (const address &_listen_addr);

    void
    bind          (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXCP;

    void
    listen        (int _backlog, accept_handler &&_new_ssesion_handler,
                   close_handler &&_close_handler);

    void
    listen        (int _backlog, std::error_code &_ec,
                   accept_handler &&_handler,
                   close_handler &&close_handler) KNGIN_NOEXCP;

    void
    close         (bool _blocking = true);

public:
    event_loop_pimpl_ptr &
    loop          () KNGIN_NOEXCP
    { return m_loop; }

    std::shared_ptr<listener>
    self          ()
    { return shared_from_this(); }

public:
    void
    check_thread  () const KNGIN_NOEXCP
    { m_loop->check_thread(); }

private:
    virtual void
    on_read       ();

    virtual void
    on_error      ();

    void
    on_close      (std::error_code _ec);

private:
    event_loop_pimpl_ptr m_loop;

    k::socket            m_socket;

    std::atomic<bool>    m_closed;

    address              m_listen_addr;

    accept_handler       m_accept_handler;

    close_handler        m_close_handler;

    filefd               m_idle_file;
};

KNGIN_NAMESPACE_K_END

#endif /* _LISTENER_H_ */

