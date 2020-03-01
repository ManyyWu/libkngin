#ifndef KNGIN_SESSION_H
#define KNGIN_SESSION_H

#include <functional>
#include <memory>
#include <map>
#include <deque>
#include "core/base/define.h"
#include "core/base/lock.h"
#include "core/base/buffer.h"
#include "core/base/noncopyable.h"
#include "core/event/event_loop.h"
#include "net/address.h"
#include "net/socket.h"
#include "net/sockopts.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

// mutex
#if (ON == KNGIN_ONE_LOOP_PER_SESSION)
#define KNGIN_SESSION_NO_MUTEX ON
#else
#define KNGIN_SESSION_NO_MUTEX OFF
#endif

// callback lower water line
#define KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT SIZE_MAX

class session
    : public epoller_event,
      public std::enable_shared_from_this<session> {
public:
    typedef std::function<void (session &, error_code)>        error_handler;

    typedef std::function<void (session &, in_buffer, size_t)> message_handler;

    typedef std::function<void (session &, uint8_t)>           oob_handler;

    typedef std::function<void (session &)>                    sent_handler;

    struct in_context {
        in_buffer       buffer;
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        message_handler handler;
#endif
        size_t          lowat;
    };

    struct out_context {
        msg_buffer      buffer;
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        sent_handler handler;
#endif
    };

    typedef std::deque<in_context>   in_ctxq;

    typedef std::deque<out_context>  out_ctxq;

    typedef std::shared_ptr<session> session_ptr;

    typedef std::weak_ptr<session>   session_weak_ptr;

public:
    session             () = delete;

    session             (event_loop &loop, k::socket &&socket,
                         const address &local_addr, const address &peer_addr);

    virtual
    ~session            () noexcept;

public:
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    send                (msg_buffer buf);
#else
    bool
    send                (msg_buffer buf, sent_handler &&handler);
#endif

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    recv                (in_buffer buf, size_t lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#else
    bool
    recv                (in_buffer buf, message_handler &&handler,
                         size_t lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#endif

    virtual void
    close               (bool sync = false);

    void
    rd_shutdown         ();

    void
    wr_shutdown         ();

    bool
    closed              () noexcept
    { return closed_; }

    //bool
    //connected           () const
    //{ return status() == TCP_ESTABLISHED; }

    //int32_t
    //status              () const
    //{ return sockopts::tcp_info(socket_).tcpi_state; }

    const error_code &
    last_error          () const noexcept
    { return last_error_; }

public:
    void
    set_read_lowat      (int size)
    { sockopts::set_rcvlowat(socket_, size); }
    int
    read_lowat          ()
    { return sockopts::rcvlowat(socket_); }
    void
    set_write_lowat     (int size)
    { sockopts::set_sndlowat(socket_, size); }
    int
    write_lowat         ()
    { return sockopts::sndlowat(socket_); }
    void
    set_keepalive       (bool on)
    { sockopts::set_keepalive(socket_, on); }
    bool
    keepalive           ()
    { return sockopts::keepalive(socket_); }

public:
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    void
    set_message_handler (const message_handler &handler)
    { assert(!registed()); message_handler_ = handler; }
    void
    set_sent_handler    (const sent_handler &handler)
    { assert(!registed()); sent_handler_ = handler; }
#endif
    void
    set_oob_handler     (const oob_handler &handler)
    { assert(!registed()); oob_handler_ = handler; enable_oob(); }
    void
    set_error_handler   (const error_handler &handler)
    { assert(!registed()); error_handler_ = handler; }

    // TODO: Optimize for callback storage

public:
    const address &
    local_addr          () const noexcept
    { return local_addr_; }

    const address &
    peer_addr           () const noexcept
    { return peer_addr_; }

    const std::string
    name                () const noexcept
    { return name_; }

    const std::string &
    key                 () const noexcept
    { return key_; }

public:
    event_loop *
    loop                () noexcept
    { return loop_; }

    const event_loop *
    loop                () const noexcept
    { return loop_; }

    session_ptr
    self                ()
    { return shared_from_this(); }

    session_weak_ptr
    weak_ptr            ()
    { return weak_from_this(); }

private:
    k::socket &
    socket              () noexcept
    { return socket_; }

private:
    virtual void
    on_events           (event_loop &loop, uint32_t flags);

    void
    on_write            ();

    void
    on_read             ();

    void
    on_oob              ();

    void
    on_error            ();
          
    void
    on_close            ();
          
private:
    void
    clear_queues        ();

private:
    event_loop *      loop_;

    k::socket         socket_;

    std::atomic_bool  closed_;

    std::atomic_bool  closing_;

    const address     local_addr_;

    const address     peer_addr_;

    const std::string name_;

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    message_handler   message_handler_;

    sent_handler      sent_handler_;
#endif

    in_ctxq           in_ctxq_;

    out_ctxq          out_ctxq_;

    out_context *     next_out_ctx_;

    in_context *      next_in_ctx_;

#if (ON == KNGIN_SESSION_ET_MODE)
    std::atomic_bool  recv_complete_;

    std::atomic_bool  send_complete_;
#endif

    oob_handler       oob_handler_;

    error_handler     error_handler_;

#if (ON != KNGIN_SESSION_NO_MUTEX)
    mutex             out_bufq_mutex_;

    mutex             in_bufq_mutex_;
#endif

    error_code        last_error_;

    const std::string key_;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SESSION_H */
