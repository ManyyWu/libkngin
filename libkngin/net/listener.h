#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <functional>
#include "define.h"
#include "socket.h"
#include "filefd.h"

KNGIN_NAMESPACE_K_BEGIN

class listener {
public:
    typedef std::function<void (k::socket &&)> accept_cb;

    typedef std::function<void (listener &)>   error_cb;

public:
    listener      () = delete;

    listener      (event_loop *_loop, k::socket &&_socket);

    ~listener     ();

public:
    bool
    bind          (const address &_listen_addr);

    bool
    listen        (int _backlog);

    void
    close         ();

public:
    void
    set_accept_cb (accept_cb &&_cb) { m_accept_cb = std::move(_cb); }

    void
    set_error_cb  (error_cb &&_cb)  { m_error_cb = std::move(_cb); }

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

