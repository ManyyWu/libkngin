#ifndef KNGIN_TCP_SERVER_H
#define KNGIN_TCP_SERVER_H

#include <map>
#include "core/define.h"
#include "core/noncopyable.h"
#include "net/address.h"
#include "net/event_loop.h"
#include "net/listener.h"
#include "net/io_threadpool.h"
#include "net/tcp/server_opts.h"
#include "net/tcp/session.h"
#include "net/tcp/server.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

class server : public noncopyable {
public:
    typedef session::in_buffer_ptr             in_buffer_ptr;

    typedef session::out_buffer_ptr            out_buffer_ptr;

    typedef session::session_ptr               session_ptr;

    typedef session::message_handler           message_handler;

    typedef session::sent_handler              sent_handler;

    typedef session::oob_handler               oob_handler;

    typedef session::close_handler             close_handler;

    typedef std::function<void (session_ptr)>  session_handler;

    typedef listener::listener_ptr             listener_ptr;

    typedef std::function<void (void)>         crash_handler;

    typedef std::vector<session_ptr>           session_list;

    typedef std::map<std::string, session_ptr> session_map;

public:
    server            () = delete;

    server            (const server_opts &_opts);

    ~server           () KNGIN_NOEXCP;

public:
    bool
    run               ();

    void
    stop              (bool _crash = false);

public:
    void
    broadcast         (session_list &_list, msg_buffer _buf);

public:
    void
    set_session_handler (session_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_session_handler = std::move(_handler); }
    void
    set_message_handler (message_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_message_handler = std::move(_handler); }
    void
    set_sent_handler    (sent_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_sent_handler = std::move(_handler); }
    void
    set_close_handler   (close_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_close_handler = std::move(_handler); }
    void
    set_oob_handler     (oob_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_oob_handler = std::move(_handler); }
    void
    set_crash_handler   (crash_handler &&_handler) KNGIN_NOEXCP
    { assert(m_stopped); m_crash_handler = std::move(_handler); }

protected:
    event_loop &
    assign_thread     ()
    { return m_threadpool.next_loop(); }

protected:
    void
    on_new_session    (socket &&_sock);

private:
    const server_opts m_opts;

    io_threadpool     m_threadpool;

#if (ON == KNGIN_SERVER_MANAGE_SESSIONS)
    session_map       m_sessions;
#endif

    listener_ptr      m_listener;

    address           m_listen_addr;

    session_handler   m_session_handler;

    sent_handler      m_sent_handler;

    message_handler   m_message_handler;

    oob_handler       m_oob_handler;

    close_handler     m_close_handler;

    crash_handler     m_crash_handler;

    std::atomic<bool> m_stopped;

    std::atomic<bool> m_stopping;

    mutex             m_mutex;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TCP_SERVER_H */