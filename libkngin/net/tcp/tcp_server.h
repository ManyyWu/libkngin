#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "define.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "address.h"
#include "event_loop.h"
#include "server_opts.h"

__NAMESPACE_BEGIN

class tcp_server {
public:
    typedef std::function<void (tcp_connection &)> new_connection_cb;

    typedef tcp_connection::read_done_cb           read_done_cb;

    typedef tcp_connection::write_done_cb          write_done_cb;

    typedef tcp_connection::read_oob_cb            read_oob_cb;

    typedef tcp_connection::close_cb               close_cb;

public:
    tcp_server  () = delete;

    tcp_server  (address &&_listen_addr, const tcp_server_opts &_opts)

    ~tcp_server ();

public:
    // run
    // stop
    // enable_accept
    // disable_accept
    // accepting

public:
    // accept, be called when new connection comming

public:
    // add_connection

    // remove_connection

public:
    // broadcast (connection_list &_list, buffer &&_buf)

public:
    // threadpoll

    // set,get max connection

    // set,get thread num

public:
    void
    set_new_connection_cb (new_connection_cb &&_cb);

    void
    set_read_done_cb      (read_done_cb &&_cb);

    void
    set_write_done_cb     (write_done_cb &&_cb);

    void
    set_close_cb          (close_cb &&_cb);

    void
    set_oob_cb            (read_oob_cb &&_cb);

public:
    void
    handle_new_connection_cb (tcp_connection &_conn);

    void
    handle_read_done_cb      (tcp_connection &_conn);

    void
    handle_write_done_cb     (tcp_connection &_conn);

    void
    handle_close_cb          (tcp_connection &_conn);

    void
    handle_oob_cb            (tcp_connection &_conn);

protected:
    // assign_thread

protected:
    // m_threadpoll
    // m_listener
    // m_connections;
    // m_max_connection
    // m_accept_new

    new_connection_cb m_new_connection_cb;

    write_done_cb     m_write_done_cb;

    read_done_cb      m_read_done_cb;

    read_oob_cb       m_oob_cb;

    close_cb          m_close_cb;
};

__NAMESPACE_END

#endif /* _TCP_SERVER_H_ */