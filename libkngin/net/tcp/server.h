#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unordered_map>
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
    typedef session::in_buffer_ptr                    in_buffer_ptr;

    typedef session::out_buffer_ptr                   out_buffer_ptr;

    typedef session::session_ptr                      session_ptr;

    typedef session::message_handler                  message_handler;

    typedef session::sent_handler                     sent_handler;

    typedef session::oob_handler                      oob_handler;

    typedef session::close_handler                    close_handler;

    typedef io_thread::event_loop_ptr                 event_loop_ptr;

    typedef std::function<void (session_ptr)>         session_handler;

    typedef std::shared_ptr<listener>                 listener_ptr;

    typedef std::function<void (void)>                crash_handler;

    typedef std::vector<session_ptr>                  session_list;

    typedef std::unordered_map<uint32_t, session_ptr> session_map;

public:
    server            () = delete;

    server            (const server_opts &_opts);

    ~server           () KNGIN_NOEXCP;

public:
    bool
    run               ();

    void
    stop              (bool _crash = false);

    size_t
    session_num       ();

public:
    void
    remove_session    (session_ptr _session);

public:
    int
    broadcast         (session_list &_list, out_buffer_ptr _buf);

public:
    void
    set_session_handler (session_handler &&_handler) KNGIN_NOEXCP
    { m_session_handler = std::move(_handler); }
    void
    set_message_handler (message_handler &&_handler) KNGIN_NOEXCP
    { m_message_handler = std::move(_handler); }
    void
    set_sent_handler    (sent_handler &&_handler) KNGIN_NOEXCP
    { m_sent_handler = std::move(_handler); }
    void
    set_close_handler   (close_handler &&_handler) KNGIN_NOEXCP
    { m_close_handler = std::move(_handler); }
    void
    set_oob_handler     (oob_handler &&_handler) KNGIN_NOEXCP
    { m_oob_handler = std::move(_handler); }
    void
    set_crash_handler   (crash_handler &&_handler) KNGIN_NOEXCP
    { m_crash_handler = std::move(_handler); }

protected:
    event_loop &
    assign_thread     ()
    { return m_threadpool.next_loop(); }

protected:
    void
    parse_addr        (const std::string &_name, uint16_t _port);

protected:
    void
    on_new_session    (socket &&_sock);

    void
    on_session_close  (const session &_session, std::error_code _ec);

    void
    on_listener_close (std::error_code _ec);

protected:
    const server_opts m_opts;

    io_threadpool     m_threadpool;

    session_map       m_sessions;

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

#endif /* _TCP_SERVER_H_ */