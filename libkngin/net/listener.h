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

class listener : public noncopyable {
public:
    typedef std::shared_ptr<event_loop>           event_loop_ptr;

    typedef event_loop::event_loop_pimpl_ptr      event_loop_pimpl_ptr;

    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (std::error_code)> close_handler;

public:
    listener      () = delete;

    listener      (event_loop_ptr _loop, k::socket &&_socket);

    ~listener     () KNGIN_NOEXP;

public:
    void
    bind          (const address &_listen_addr)
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr); }

    void
    bind          (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr, _ec); }

    void
    listen        (int _backlog, accept_handler &&_new_ssesion_handler, 
                   close_handler &&close_handler);

    void
    listen        (int _backlog, std::error_code &_ec, 
                   accept_handler &&_handler, 
                   close_handler &&close_handler) KNGIN_NOEXP;

public:
    void
    close         ();

public:
    void
    check_thread  () const KNGIN_NOEXP
    { m_loop->check_thread(); }

private:
    void
    on_accept     () KNGIN_NOEXP;

    void
    on_close      (std::error_code _ec) KNGIN_NOEXP;

    void
    on_error      () KNGIN_NOEXP;

private:
    event_loop_ptr    m_loop;

    k::socket         m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_closed;

    address           m_listen_addr;

    accept_handler    m_accept_handler;

    close_handler     m_close_handler;

    filefd            m_idle_file;
};

KNGIN_NAMESPACE_K_END

#endif /* _LISTENER_H_ */

