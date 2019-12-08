#ifndef _SESSION_H_
#define _SESSION_H_

#include <functional>
#include <memory>
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
    typedef std::function<void (const session &, std::error_code)> close_handler;

    typedef std::function<void (session &, in_buffer &, size_t)>   message_handler;

    typedef std::function<void (session &)>                        sent_handler;

    typedef std::function<void (session &, uint8_t)>               oob_handler;

    typedef std::shared_ptr<session>                               session_ptr;

    typedef std::shared_ptr<in_buffer>                             in_buffer_ptr;

    typedef std::shared_ptr<out_buffer>                            out_buffer_ptr;

    typedef std::deque<out_buffer_ptr>                             out_buffer_queue;

public:
    session         () = delete;

    session         (event_loop &_loop, k::socket &&_socket,
                     const address &_local_addr, const address &_peer_addr);

    ~session        () KNGIN_NOEXP;

public:
    bool
    send            (out_buffer_ptr _buf);

    bool
    recv            (in_buffer_ptr _buf, size_t _lowat = KNGIN_DEFAULT_MESSAGE_CALLBACK_LOWAT);

    void
    close           (bool _blocking = false);

    void
    rd_shutdown     ();

    void
    wr_shutdown     ();

    bool
    connected       () const
    { return m_connected; }

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
    void
    set_message_handler (const message_handler &_handler)
    { m_message_handler = _handler; }
    void
    set_sent_handler    (const sent_handler &_handler)
    { m_sent_handler = _handler; }
    void
    set_close_handler   (const close_handler &_handler)
    { m_close_handler = _handler; }
    void
    set_oob_handler     (const oob_handler &_handler)
    { m_oob_handler = _handler; }

    // TODO: Optimize callback function storage

public:
    const address &
    local_addr      () const KNGIN_NOEXP
    { return m_local_addr; }

    const address &
    peer_addr       () const KNGIN_NOEXP
    { return m_peer_addr; }

    const std::string
    name            () const KNGIN_NOEXP
    { return m_name; }

    uint64_t
    serial          () const KNGIN_NOEXP
    { return m_serial; }

public:
    void
    check_thread    () const KNGIN_NOEXP
    { m_loop->check_thread(); }

public:
    event_loop_pimpl_ptr &
    loop          () KNGIN_NOEXP
    { return m_loop; }

    std::shared_ptr<session>
    self     ()
    { return shared_from_this(); }

private:
    uint64_t
    next_serial     ()
    { return m_next_serial++; }

    k::socket &
    socket          ()
    { return m_socket; }

private:
    virtual void
    on_write        ();

    virtual void
    on_read         ();

    virtual void
    on_oob          ();

    virtual void
    on_error        ();

    void
    on_close        (std::error_code _ec);

private:
    event_loop_pimpl_ptr m_loop;

    k::socket            m_socket;

    std::atomic<bool>    m_connected;

    address              m_local_addr;

    address              m_peer_addr;

    const std::string    m_name;

    sent_handler         m_sent_handler;

    message_handler      m_message_handler;

    oob_handler          m_oob_handler;

    close_handler        m_close_handler;

    out_buffer_queue     m_out_bufq;

    in_buffer_ptr        m_in_buf;

    size_t               m_callback_lowat;

    uint64_t             m_serial;

    mutex                m_mutex;

private:
    static uint64_t      m_next_serial;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* _SESSION_H_ */