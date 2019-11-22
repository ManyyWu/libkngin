#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include <functional>
#include <memory>
#include <deque>
#include "define.h"
#include "address.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "socket.h"
#include "address.h"
#include "sockopts.h"
#include "epoller.h"
#include "buffer.h"
#include "net_buffer.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

class tcp_connection {
public:
    typedef std::function<void (tcp_connection &, buffer &, size_t)> message_cb;

    typedef std::function<void (tcp_connection &)>                   sent_cb;

    typedef std::function<void (tcp_connection &, uint8_t)>          oob_cb;

    typedef std::function<void (const tcp_connection &)>             close_cb;

    //typedef std::function<void (void)>                               writeable_cb;

    //typedef std::function<void (void)>                               readable_cb;

    typedef std::shared_ptr<tcp_connection>                          tcp_connection_ptr;

    typedef std::shared_ptr<buffer>                                  buffer_ptr;

    typedef std::deque<buffer_ptr>                                   buffer_queue;

public:
    tcp_connection    () = delete;

    tcp_connection    (event_loop *_loop, k::socket &&_socket,
                       const address &_local_addr, const address &_peer_addr);

    ~tcp_connection   ();

public:
    bool
    send              (buffer_ptr _buf);

    bool
    recv              (buffer_ptr _buf);

    void
    close             ();

    void
    rd_shutdown       ();

    void
    wr_shutdown       ();

    bool
    connected         () { return m_connected; }

    void
    set_disconnected  () { m_connected = false; }

public:
    void
    set_message_cb    (const message_cb &_cb)  { m_message_cb= _cb; }

    void
    set_sent_cb       (const sent_cb &_cb)     { m_sent_cb = _cb; }

    void
    set_close_cb      (const close_cb &_cb)    { m_close_cb = _cb; }

    void
    set_oob_cb        (const oob_cb &_cb)      { m_oob_cb = _cb; }

//    void
//    set_readable_cb   (const readable_cb &_cb)   { m_readable_cb = _cb; }
//
//    void
//    set_writeable_cb  (const writeable_cb &_cb)   { m_writeable_cb = _cb; }

public:
    bool
    set_read_lowat  (int _size)  { return sockopts::set_rcvlowat(m_socket, _size); }

    bool
    set_write_lowat (int _size)  { return sockopts::set_sndlowat(m_socket, _size); }

    bool
    read_lowat      (int &_size) { return sockopts::rcvlowat(m_socket, _size); }

    bool
    write_lowat     (int &_size) { return sockopts::sndlowat(m_socket, _size); }

private:
    void
    on_write     ();

    void
    on_read      ();

    void
    on_close     ();

    void
    on_oob       ();

    void
    on_error     ();

public:
    k::socket &
    socket       ()       { return m_socket; }

    const address &
    local_addr   () const { return m_local_addr; }

    const address &
    peer_addr    () const { return m_peer_addr; }

    uint64_t
    serial       () const { return m_serial; }

    event_loop *
    get_loop     () const { return m_loop; }

protected:
    uint64_t
    next_serial  ()       { return m_next_serial++; }

protected:
    event_loop *      m_loop;

    k::socket         m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_connected;

    address           m_local_addr;

    address           m_peer_addr;

//    writeable_cb      m_writeable_cb;

//    readable_cb       m_readable_cb;

    sent_cb           m_sent_cb;

    message_cb        m_message_cb;

    oob_cb            m_oob_cb;

    close_cb          m_close_cb;

    buffer_queue      m_out_bufq;

    buffer_ptr        m_in_buf;

    uint64_t          m_serial;

    mutex             m_mutex;

protected:
    static uint64_t   m_next_serial;
};

__NAMESPACE_END

#endif /* _TCP_CONNECTION_H_ */