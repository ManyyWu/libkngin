#ifndef KNGIN_TCP_SERVER_H
#define KNGIN_TCP_SERVER_H

#include <map>
#include <vector>
#include "core/base/define.h"
#include "core/base/noncopyable.h"
#include "core/event/event_loop.h"
#include "core/event/io_threadpool.h"
#include "net/address.h"
#include "net/listener.h"
#include "net/tcp/server_opts.h"
#include "net/tcp/session.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

class server : public noncopyable {
public:
    typedef session::session_ptr               session_ptr;

    typedef session::message_handler           message_handler;

    typedef session::sent_handler              sent_handler;

    typedef session::oob_handler               oob_handler;

    typedef session::error_handler             error_handler;

    typedef std::function<void (session_ptr)>  session_handler;

    typedef listener::listener_ptr             listener_ptr;

    typedef std::function<void (void)>         crash_handler;

    typedef std::vector<session_ptr>           session_list;

    typedef std::map<std::string, session_ptr> session_map;

public:
    server              () = delete;

    server              (event_loop &loop, const server_opts &opts);

    ~server             () noexcept;

public:
    bool
    run                 ();

    void
    stop                ();

public:
    void
    broadcast           (session_list &list, msg_buffer buf);

public:
    void
    set_session_handler (session_handler &&handler) noexcept
    { assert(m_stopped); m_session_handler = std::move(handler); }
    void
    set_message_handler (message_handler &&handler) noexcept
    { assert(m_stopped); m_message_handler = std::move(handler); }
    void
    set_sent_handler    (sent_handler &&handler) noexcept
    { assert(m_stopped); m_sent_handler = std::move(handler); }
    void
    set_error_handler   (error_handler &&handler) noexcept
    { assert(m_stopped); m_error_handler = std::move(handler); }
    void
    set_oob_handler     (oob_handler &&handler) noexcept
    { assert(m_stopped); m_oob_handler = std::move(handler); }
    void
    set_crash_handler   (crash_handler &&handler) noexcept
    { assert(m_stopped); m_crash_handler = std::move(handler); }

protected:
    event_loop &
    assign_thread       ()
    { return m_threadpool.next_loop(); }

private:
    event_loop *      m_loop;

    const server_opts m_opts;

    io_threadpool     m_threadpool;

    listener_ptr      m_listener;

    session_handler   m_session_handler;

    sent_handler      m_sent_handler;

    message_handler   m_message_handler;

    oob_handler       m_oob_handler;

    error_handler     m_error_handler;

    crash_handler     m_crash_handler;

    std::atomic_bool  m_stopped;

    std::atomic_bool  m_stopping;

    mutex             m_mutex;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TCP_SERVER_H */