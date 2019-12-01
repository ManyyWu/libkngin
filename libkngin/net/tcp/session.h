#ifndef _SESSION_H_
#define _SESSION_H_

#include <functional>
#include <memory>
#include <deque>
#include "core/define.h"
#include "core/buffer.h"
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

class session {
public:
    typedef std::function<void (session &, in_buffer &, size_t)> message_handler;

    typedef std::function<void (session &)>                      sent_handler;

    typedef std::function<void (session &, uint8_t)>             oob_handler;

    typedef std::function<void (const session &)>                close_handler;

    typedef std::shared_ptr<session>                             session_ptr;

    typedef std::shared_ptr<in_buffer>                           in_buffer_ptr;

    typedef std::shared_ptr<out_buffer>                          out_buffer_ptr;

    typedef std::deque<out_buffer_ptr>                           out_buffer_queue;

    typedef std::shared_ptr<event_loop>                          event_loop_ptr;

    typedef event_loop::event_loop_pimpl_ptr                     event_loop_pimpl_ptr;

public:
    session         () = delete;

    session         (event_loop_ptr _loop, k::socket &&_socket,
                     const address &_local_addr, const address &_peer_addr);

    ~session        () KNGIN_NOEXP;

public:
    bool
    send            (out_buffer_ptr _buf);

    bool
    recv            (in_buffer_ptr _buf);

    void
    close           ();

    void
    rd_shutdown     ();

    void
    wr_shutdown     ();

    bool
    connected       ()
    { return m_sessionected; }

public:
    bool
    set_read_lowat  (int _size)
    { return sockopts::set_rcvlowat(m_socket, _size); }

    bool
    read_lowat      (int &_size)
    { return sockopts::rcvlowat(m_socket, _size); }

    bool
    set_write_lowat (int _size)
    { return sockopts::set_sndlowat(m_socket, _size); }

    bool
    write_lowat     (int &_size)
    { return sockopts::sndlowat(m_socket, _size); }

    bool
    set_keepalive   (bool _on)
    { return sockopts::set_keepalive(m_socket, _on); }

    bool
    keepalive       (bool &_on)
    { return sockopts::keepalive(m_socket, _on); }

private:
    void
    on_write        ();

    void
    on_read         ();

    void
    on_close        ();

    void
    on_oob          ();

    void
    on_error        ();

public:
    void
    set_message_handler (const message_handler &_handler)
    { m_message_handler= _handler; }
    void
    set_sent_handler    (const sent_handler &_handler)
    { m_sent_handler = _handler; }
    void
    set_close_handler   (const close_handler &_handler)
    { m_close_handler = _handler; }
    void
    set_oob_handler     (const oob_handler &_handler)
    { m_oob_handler = _handler; }

public:
    k::socket &
    socket          ()
    { return m_socket; }

    const address &
    local_addr      () const
    { return m_local_addr; }

    const address &
    peer_addr       () const
    { return m_peer_addr; }

    uint64_t
    serial          () const
    { return m_serial; }

public:
    void
    check_thread    () const
    { m_loop->check_thread(); }

protected:
    uint64_t
    next_serial     ()
    { return m_next_serial++; }

protected:
    event_loop_ptr    m_loop;

    k::socket         m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_sessionected;

    address           m_local_addr;

    address           m_peer_addr;

    sent_handler      m_sent_handler;

    message_handler   m_message_handler;

    oob_handler       m_oob_handler;

    close_handler     m_close_handler;

    out_buffer_queue  m_out_bufq;

    in_buffer_ptr     m_in_buf;

    uint64_t          m_serial;

    mutex             m_mutex;

protected:
    static uint64_t   m_next_serial;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* _SESSION_H_ */