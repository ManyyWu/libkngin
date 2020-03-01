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
    { assert(stopped_); session_handler_ = std::move(handler); }
    void
    set_message_handler (message_handler &&handler) noexcept
    { assert(stopped_); message_handler_ = std::move(handler); }
    void
    set_sent_handler    (sent_handler &&handler) noexcept
    { assert(stopped_); sent_handler_ = std::move(handler); }
    void
    set_error_handler   (error_handler &&handler) noexcept
    { assert(stopped_); error_handler_ = std::move(handler); }
    void
    set_oob_handler     (oob_handler &&handler) noexcept
    { assert(stopped_); oob_handler_ = std::move(handler); }
    void
    set_crash_handler   (crash_handler &&handler) noexcept
    { assert(stopped_); crash_handler_ = std::move(handler); }

protected:
    event_loop &
    assign_thread       ()
    { return threadpool_.next_loop(); }

private:
    event_loop *      loop_;

    const server_opts opts_;

    io_threadpool     threadpool_;

    listener_ptr      listener_;

    session_handler   session_handler_;

    sent_handler      sent_handler_;

    message_handler   message_handler_;

    oob_handler       oob_handler_;

    error_handler     error_handler_;

    crash_handler     crash_handler_;

    std::atomic_bool  stopped_;

    std::atomic_bool  stopping_;

    mutex             mutex_;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_TCP_SERVER_H */