#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>
#include "define.h"
#include "address.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "socket.h"
#include "address.h"
#include "sockopts.h"
#include "epoller.h"
#include "buffer.h"
#include "epoller_event.h"

__NAMESPACE_BEGIN

class connection {
public:
    typedef std::function<void (connection &, buffer &, size_t)> write_done_cb;

    typedef std::function<void (connection &, buffer &, size_t)> read_done_cb;

    typedef std::function<void (connection &, uint8_t)>          read_oob_cb;

    typedef std::function<void (connection &)>                   close_cb;

    typedef std::function<void (void)>                           writeable_cb;

    typedef std::function<void (void)>                           readable_cb;

public:
    connection  () = delete;

    connection  (event_loop *_loop, socket &&_socket,
                 const address &_local_addr, const address &_peer_addr);

    ~connection ();

public:
    bool
    send      (buffer &_buf, size_t _size);

    bool
    recv      (buffer &_buf, size_t _size);

    void
    close     () { m_socket.close(); }

    bool
    connected () { return m_connected; }

public:
    void
    set_read_done_cb  (read_done_cb &&_cb);

    void
    set_write_done_cb (write_done_cb &&_cb);

    void
    set_close_cb      (close_cb &&_cb);

    void
    set_oob_cb        (read_oob_cb &&_cb);

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
    handle_write  ();

    void
    handle_read   ();

    void
    handle_close  ();

    void
    handle_oob    ();

    void
    handle_error  ();

public:
    class socket &
    socket () { return m_socket; }

    event_loop *
    loop   () { return m_loop; }

protected:
    event_loop *      m_loop;

    class socket      m_socket;

    epoller_event     m_event;

    std::atomic<bool> m_connected;

    address           m_local_addr;

    address           m_peer_addr;

    writeable_cb      m_writable_cb;

    readable_cb       m_readable_cb;

    write_done_cb     m_write_done_cb;

    read_done_cb      m_read_done_cb;

    read_oob_cb       m_oob_cb;

    close_cb          m_close_cb;

    buffer *          m_out_buf;

    size_t            m_out_size;

    buffer *          m_in_buf;

    size_t            m_in_size;

    uint8_t           m_oob_buf;
};

__NAMESPACE_END

#endif /* _CONNECTION_H_ */