#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <functional>
#include "define.h"
#include "socket.h"
#include "system_error.h"
#include "filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class listener {
public:
    typedef std::function<void (k::socket &&)>    accept_cb;

    typedef std::function<void (std::error_code)> error_handler;

public:
    listener      () = delete;

    listener      (event_loop *_loop, k::socket &&_socket);

    ~listener     ();

public:
    void
    bind          (const address &_listen_addr)
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr); }
    void
    bind          (const address &_listen_addr, std::error_code &_ec)
    { assert(!m_closed); m_socket.bind(m_listen_addr = _listen_addr, _ec); }
    void
    listen        (int _backlog)
    { assert(!m_closed); m_socket.listen(_backlog); } 
    void
    listen        (int _backlog, std::error_code &_ec)
    { assert(!m_closed); m_socket.listen(_backlog, _ec); } 

public:
    void
    close         (error_handler &&_cb);

public:
    event_loop *
    get_loop      () const          { return m_loop; }

protected:
    void
    on_accept     ();

    void
    on_close      ();

    void
    on_error      ();

protected:
    event_loop *      m_loop;

    k::socket         m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_closed;

    address           m_listen_addr;

    accept_cb         m_accept_cb;

    error_cb          m_error_cb;

    filefd            m_idle_file;
};

KNGIN_NAMESPACE_K_END

#endif /* _LISTENER_H_ */

