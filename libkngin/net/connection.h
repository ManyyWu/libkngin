#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>
#include "define.h"
#include "address.h"
#include "epoller_event.h"
#include "event_loop.h"
#include "socket.h"
#include "address.h"

__NAMESPACE_BEGIN

class connection {
public:
    typedef std::function<void *(connection &)> connection_cb;

public:
    connection        () = delete;

    explicit
    connection        (event_loop *_loop, socket &_socket,
                       const address &_local_addr, const address &_peer_addr);

    ~connection       ();

public:


protected:
    event_loop *
    loop              () { return m_loop; }

    socket &
    socket            () { return m_socket; }

protected:
    event_loop *   m_loop;

    class socket   m_socket;

    epoller_event  m_event;

    address        m_local_addr;

    address        m_peer_addr;

    connection_cb  m_read_cb;

    connection_cb  m_write_cb;

    connection_cb  m_writable_cb;

    connection_cb  m_readable_cb;

    connection_cb  m_read_done_cb;

    connection_cb  m_write_done_cb;
};

__NAMESPACE_END

#endif /* _CONNECTION_H_ */