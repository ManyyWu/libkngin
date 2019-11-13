#include <functional>
#include "define.h"
#include "socket.h"

__NAMESPACE_BEGIN


class listener {
public:
    typedef std::function<void (socket &&)>  accept_cb;

    typedef std::function<void (listener &)> error_cb;

public:
    listener      () = delete;

    listener      (event_loop *_loop, k::socket &&_socket);

    ~listener     ();

public:
    void
    set_accept_cb (accept_cb &&_cb) { m_accept_cb = std::move(_cb); }

    void
    set_error_cb  (error_cb &&_cb)  { m_error_cb = std::move(_cb); }

public:
    bool
    bind          (const address &_listen_addr);

    bool
    listen        (int _backlog);

    void
    close         ();

private:
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
};


__NAMESPACE_END
