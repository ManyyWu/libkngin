#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <functional>
#include <cassert>
#include <system_error>
#include "core/noncopyable.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

class listener : noncopyable {
public:
    typedef std::shared_ptr<event_loop>           event_loop_ptr;

    typedef event_loop::event_loop_pimpl_ptr      event_loop_pimpl_ptr;

    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (std::error_code)> error_handler;

public:
    listener      () = delete;

    listener      (event_loop_ptr _loop, k::socket &&_socket) KNGIN_EXP;

    ~listener     () KNGIN_NOEXP;

public:
    void
    bind          (const address &_listen_addr) KNGIN_EXP
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr); }
    void
    bind          (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr, _ec); }
    void
    listen        (int _backlog, accept_handler &&_accept_cb, error_handler &&_error_cb) KNGIN_EXP
    { assert(!m_closed); m_socket.listen(_backlog); } 
    void
    listen        (int _backlog, std::error_code &_ec) KNGIN_NOEXP
    { assert(!m_closed); m_socket.listen(_backlog, _ec); } 

public:
    void
    close         (error_handler &&_cb) KNGIN_EXP;

public:
    void
    check_thread  () const KNGIN_NOEXP
    { m_loop->check_thread(); }

protected:
    void
    on_accept     () KNGIN_NOEXP;

    void
    on_close      () KNGIN_NOEXP;

    void
    on_error      () KNGIN_NOEXP;

protected:
    event_loop_ptr    m_loop;

    k::socket         m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_closed;

    address           m_listen_addr;

    accept_handler    m_accept_cb;

    error_handler     m_error_cb;

    filefd            m_idle_file;
};

KNGIN_NAMESPACE_K_END

#endif /* _LISTENER_H_ */

