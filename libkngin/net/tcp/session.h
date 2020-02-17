#ifndef KNGIN_SESSION_H
#define KNGIN_SESSION_H

#include <functional>
#include <memory>
#include <map>
#include <deque>
#include "core/base/define.h"
#include "core/base/buffer.h"
#include "core/base/noncopyable.h"
#include "core/event/epoller.h"
#include "core/event/event_loop.h"
#include "core/event/epoller.h"
#include "core/event/epoller.h"
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

    session             (event_loop &_loop, k::socket &&_socket,
                         const address &_local_addr, const address &_peer_addr);

    virtual
    ~session            () KNGIN_NOEXCP;

public:
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    send                (msg_buffer _buf);
#else
    bool
    send                (msg_buffer _buf, sent_handler &&_handler);
#endif

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    recv                (in_buffer _buf, size_t _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#else
    bool
    recv                (in_buffer _buf, message_handler &&_handler,
                         size_t _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#endif

    virtual void
    close               (bool _sync = false);

    void
    rd_shutdown         ();

    void
    wr_shutdown         ();

    bool
    closed              () KNGIN_NOEXCP
    { return m_closed; }

    bool
    connected           () const
    { return status() == TCP_ESTABLISHED; }

    int32_t
    status              () const
    { return sockopts::tcp_info(m_socket).tcpi_state; }

    const error_code &
    last_error          () const KNGIN_NOEXCP
    { return m_last_error; }

public:
    void
    set_read_lowat      (int _size)
    { sockopts::set_rcvlowat(m_socket, _size); }
    int
    read_lowat          ()
    { return sockopts::rcvlowat(m_socket); }
    void
    set_write_lowat     (int _size)
    { sockopts::set_sndlowat(m_socket, _size); }
    int
    write_lowat         ()
    { return sockopts::sndlowat(m_socket); }
    void
    set_keepalive       (bool _on)
    { sockopts::set_keepalive(m_socket, _on); }
    bool
    keepalive           ()
    { return sockopts::keepalive(m_socket); }

public:
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    void
    set_message_handler (const message_handler &_handler)
    { assert(!registed()); m_message_handler = _handler; }
    void
    set_sent_handler    (const sent_handler &_handler)
    { assert(!registed()); m_sent_handler = _handler; }
#endif
    void
    set_oob_handler     (const oob_handler &_handler)
    { assert(!registed()); m_oob_handler = _handler; enable_oob(); }
    void
    set_error_handler   (const error_handler &_handler)
    { assert(!registed()); m_error_handler = _handler; }

    // TODO: Optimize callback function storage

public:
    const address &
    local_addr          () const KNGIN_NOEXCP
    { return m_local_addr; }

    const address &
    peer_addr           () const KNGIN_NOEXCP
    { return m_peer_addr; }

    const std::string
    name                () const KNGIN_NOEXCP
    { return m_name; }

    const std::string &
    key                 () const KNGIN_NOEXCP
    { return m_key; }

public:
    event_loop *
    loop                () KNGIN_NOEXCP
    { return m_loop; }

    const event_loop *
    loop                () const KNGIN_NOEXCP
    { return m_loop; }

    session_ptr
    self                ()
    { return shared_from_this(); }

    session_weak_ptr
    weak_ptr            ()
    { return weak_from_this(); }

private:
    k::socket &
    socket              () KNGIN_NOEXCP
    { return m_socket; }

private:
    virtual void
    on_events           (event_loop &_loop, uint32_t _flags);

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
    event_loop *      m_loop;

    k::socket         m_socket;

    std::atomic_bool  m_closed;

    std::atomic_bool  m_closing;

    const address     m_local_addr;

    const address     m_peer_addr;

    const std::string m_name;

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    message_handler   m_message_handler;

    sent_handler      m_sent_handler;
#endif

    in_ctxq           m_in_ctxq;

    out_ctxq          m_out_ctxq;

    out_context *     m_next_out_ctx;

    in_context *      m_next_in_ctx;

#if (ON == KNGIN_SESSION_ET_MODE)
    std::atomic_bool  m_recv_complete;

    std::atomic_bool  m_send_complete;
#endif

    oob_handler       m_oob_handler;

    error_handler     m_error_handler;

#if (ON != KNGIN_SESSION_NO_MUTEX)
    mutex             m_out_bufq_mutex;

    mutex             m_in_bufq_mutex;
#endif

    error_code        m_last_error;

    const std::string m_key;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SESSION_H */
