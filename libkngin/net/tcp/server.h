#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unordered_map>
#include "define.h"
#include "session.h"
#include "server.h"
#include "address.h"
#include "event_loop.h"
#include "server_opts.h"
#include "listener.h"
#include "io_threadpool.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

class server {
public:
    typedef session::session_ptr                      session_ptr;

    typedef std::function<void (session_ptr)>         session_cb;

    typedef session::message_cb                       message_cb;

    typedef session::sent_cb                          sent_cb;

    typedef session::oob_cb                           oob_cb;

    typedef session::close_cb                         close_cb;

    typedef io_thread::event_loop_ptr                 event_loop_ptr;

    typedef std::shared_ptr<listener>                 listener_ptr;

    typedef std::vector<session *>                    session_list;

    typedef std::unordered_map<uint32_t, session_ptr> session_map;

public:
    server            () = delete;

    server            (const server_opts &_opts);

    ~server           ();

public:
    bool
    run               ();

    void
    stop              ();

    size_t
    session_num       ();

public:
    void
    remove_session    (session_ptr _session);

public:
    int
    broadcast         (session_list &_list, buffer &&_buf);

public:
    void
    set_session_cb    (session_cb &&_cb) { m_session_cb = std::move(_cb); }
    void
    set_message_cb    (message_cb &&_cb) { m_message_cb = std::move(_cb); }
    void
    set_sent_cb       (sent_cb &&_cb)    { m_sent_cb = std::move(_cb); }
    void
    set_close_cb      (close_cb &&_cb)   { m_close_cb = std::move(_cb); }
    void
    set_oob_cb        (oob_cb &&_cb)     { m_oob_cb = std::move(_cb); }

protected:
    event_loop_ptr
    assign_thread     ()                 { return m_threadpool.next_loop(); }

protected:
    bool
    parse_addr        (const std::string &_name, uint16_t _port);

protected:
    void
    on_new_session    (socket &&_sock);

    void
    on_close          (const session &_session);

    void
    on_listener_error (listener &_session);

protected:
    const server_opts m_opts;

    io_threadpool     m_threadpool;

    session_map       m_sessions;

    listener_ptr      m_listener;

    address           m_listen_addr;

    session_cb        m_session_cb;

    sent_cb           m_sent_cb;

    message_cb        m_message_cb;

    oob_cb            m_oob_cb;

    close_cb          m_close_cb;

    std::atomic<bool> m_stopped;

    mutex             m_mutex;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* _TCP_SERVER_H_ */