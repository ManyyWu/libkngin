#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include "define.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "address.h"
#include "event_loop.h"
#include "server_opts.h"
#include "io_threadpool.h"

__NAMESPACE_BEGIN

class tcp_server {
public:
    typedef std::function<void (tcp_connection &)> new_connection_cb;

    typedef tcp_connection::read_done_cb           read_done_cb;

    typedef tcp_connection::write_done_cb          write_done_cb;

    typedef tcp_connection::read_oob_cb            read_oob_cb;

    typedef tcp_connection::close_cb               close_cb;

    typedef std::vector<tcp_connection>            tcp_connection_list;

public:
    tcp_server            () = delete;

    tcp_server            (address &&_listen_addr, const tcp_server_opts &_opts)

    ~tcp_server           ();

public:
    void
    run                   ();

    void
    stop                  ();

    void
    accepting             ();

public:
    socket 
    accept                ();

public:
    bool
    add_connection        ();

    bool
    remove_connection     ();

public:
    int
    broadcast             (tcp_connection_list &_list, buffer &&_buf, bool _self = true); // self

public:
    void
    set_new_connection_cb (new_connection_cb &&_cb) { m_new_connection_cb = std::move(_cb); }

    void
    set_read_done_cb      (read_done_cb &&_cb)      { m_read_done_cb = std::move(_cb); }

    void
    set_write_done_cb     (write_done_cb &&_cb)     { m_write_done_cb = std::move(_cb); }

    void
    set_close_cb          (close_cb &&_cb)          { m_close_cb = std::move(_cb); }

    void
    set_oob_cb            (read_oob_cb &&_cb)       { m_oob_cb = std::move(_cb); }

public:
    event_loop_ptr
    assign_thread         ()                        { return m_threadpool.next_loop(); }

protected:
    void
    on_new_connection     (socket &&_conn);

    void
    on_message            (tcp_connection &_conn);

    void
    on_send_complete      (tcp_connection &_conn);

    void
    on_close              (tcp_connection &_conn);

    void
    on_oob                (tcp_connection &_conn);

protected:
    io_threadpool       m_threadpool;

#ifndef NDEBUG
    tcp_connection_list m_connections;
#endif

    tcp_connection      m_listener;

    new_connection_cb   m_new_connection_cb;

    write_done_cb       m_write_done_cb;

    read_done_cb        m_read_done_cb;

    read_oob_cb         m_oob_cb;

    close_cb            m_close_cb;
};

__NAMESPACE_END

#endif /* _TCP_SERVER_H_ */