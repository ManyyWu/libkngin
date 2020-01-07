#ifndef KNGIN_SESSION_H
#define KNGIN_SESSION_H

#include <functional>
#include <memory>
#include <map>
#include <deque>
#include "core/define.h"
#include "core/buffer.h"
#include "core/noncopyable.h"
#include "net/address.h"
#include "net/epoller_event.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/address.h"
#include "net/sockopts.h"
#include "net/epoller.h"
#include "net/epoller_event.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

#define KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT SIZE_MAX


class session
    : public epoller_event,
      public std::enable_shared_from_this<session> {
public:
    typedef event_loop::pimpl_weak_ptr                             loop_weak_ptr;

    typedef std::function<void (const session &, std::error_code)> close_handler;

    typedef std::function<void (session &, in_buffer, size_t)>     message_handler;

    typedef std::function<void (session &, uint8_t)>               oob_handler;

    typedef std::function<void (session &)>                        sent_handler;

    struct recv_context {
        msg_buffer      buffer;
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        message_handler handler;
#endif
    };

    struct send_context {
        in_buffer       buffer;
#if (ON == KNGIN_SESSION_TEMP_CALLBACK)
        message_handler handler;
#endif
    };

    typedef std::deque<recv_context>                               recv_ctxq;

    typedef std::deque<send_context>                               send_ctxq;

    typedef std::shared_ptr<session>                               session_ptr;


public:
    session         () = delete;

    session         (event_loop &_loop, k::socket &&_socket,
                     const address &_local_addr, const address &_peer_addr);

    virtual
    ~session        () KNGIN_NOEXCP;

public:
#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    send            (msg_buffer _buf);
#else
    bool
    send            (msg_buffer _buf, sent_handler &&_handler);
#endif

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    bool
    recv            (in_buffer _buf, size_t _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#else
    bool
    recv            (in_buffer _buf, message_handler &&_handler,
                     size_t _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);
#endif

    virtual void
    close           (bool _blocking = false);

    void
    rd_shutdown     ();

    void
    wr_shutdown     ();

    bool
    closed          () KNGIN_NOEXCP
    { return m_closed; }

    bool
    connected       () const
    { return status() == TCP_ESTABLISHED; }

    int32_t
    status          () const
    { return sockopts::tcp_info(m_socket).tcpi_state; }

public:
    void
    set_read_lowat  (int _size)
    { sockopts::set_rcvlowat(m_socket, _size); }
    int
    read_lowat      ()
    { return sockopts::rcvlowat(m_socket); }
    void
    set_write_lowat (int _size)
    { sockopts::set_sndlowat(m_socket, _size); }
    int
    write_lowat     ()
    { return sockopts::sndlowat(m_socket); }
    void
    set_keepalive   (bool _on)
    { sockopts::set_keepalive(m_socket, _on); }
    bool
    keepalive       ()
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
    set_close_handler   (const close_handler &_handler)
    { assert(!registed()); m_close_handler = _handler; }
    void
    set_oob_handler     (const oob_handler &_handler)
    { assert(!registed()); m_oob_handler = _handler; enable_oob(); }

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
    loop_weak_ptr &
    loop                () KNGIN_NOEXCP
    { return m_loop; }

    const loop_weak_ptr &
    loop                () const KNGIN_NOEXCP
    { return m_loop; }

    session_ptr
    self                ()
    { return shared_from_this(); }

public:
    void
    set_establish       () KNGIN_NOEXCP
    { m_established = true; }

private:
    k::socket &
    socket              () KNGIN_NOEXCP
    { return m_socket; }

private:
    virtual void
    on_write            ();

    virtual void
    on_read             ();

    virtual void
    on_oob              ();

    virtual void
    on_error            ();
          
    void
    on_close            ();
          
    void
    on_close            (std::error_code _ec);

private:
    void
    clear_queues        ();
          
private:
    loop_weak_ptr     m_loop;

    k::socket         m_socket;

    std::atomic_bool  m_closed;

    const address     m_local_addr;

    const address     m_peer_addr;

    const std::string m_name;

#if (OFF == KNGIN_SESSION_TEMP_CALLBACK)
    message_handler   m_message_handler;

    sent_handler      m_sent_handler;
#endif

    recv_ctxq         m_recv_ctxq;

    send_ctxq         m_send_ctxq;

#if (ON == KNGIN_SESSION_ET_MODE)
    bool              m_recv_complete;

    bool              m_send_complete;
#endif

    oob_handler       m_oob_handler;

    close_handler     m_close_handler;

#if (ON != KNGIN_SESSION_NO_MUTEX)
    mutex             m_out_bufq_mutex;

    mutex             m_in_bufq_mutex;
#endif

    size_t            m_callback_lowat;

    const std::string m_key;

    std::atomic_bool  m_established;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_SESSION_H */
