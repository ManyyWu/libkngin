#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unordered_map>
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
    typedef std::function<void (socket&&)>            new_connection_cb;

    typedef tcp_connection::read_done_cb              read_done_cb;

    typedef tcp_connection::write_done_cb             write_done_cb;

    typedef tcp_connection::read_oob_cb               read_oob_cb;

    typedef tcp_connection::close_cb                  close_cb;

    typedef std::shared_ptr<tcp_connection>           tcp_connection_ptr;

    typedef io_thread::event_loop_ptr                 event_loop_ptr;

    typedef std::vector<tcp_connection *>             tcp_connection_list;

    typedef std::unordered_map<int, tcp_connection *> tcp_connection_map;

public:
    tcp_server            () = delete;

    tcp_server            (const tcp_server_opts &_opts);

    ~tcp_server           ();

public:
    bool
    run                   ();

    void
    stop                  ();

public:
    void
    remove_connection     (tcp_connection &_conn);

public:
    int
    broadcast             (tcp_connection_list &_list, buffer &&_buf);

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

protected:
    event_loop_ptr
    assign_thread         ()                        { return m_threadpool.next_loop(); }

protected:
    socket 
    accept                ();

protected:
    void
    on_new_connection     (socket &&_sock);

    void
    on_close              (tcp_connection &_conn);

protected:
    const tcp_server_opts m_opts;

    io_threadpool         m_threadpool;

    tcp_connection_map    m_connections;

    tcp_connection_ptr    m_listener;

    address               m_listen_addr;

    new_connection_cb     m_new_connection_cb;

    write_done_cb         m_write_done_cb;

    read_done_cb          m_read_done_cb;

    read_oob_cb           m_oob_cb;

    close_cb              m_close_cb;

    std::atomic<bool>     m_stopped;

    mutex                 m_mutex;
};

__NAMESPACE_END

#endif /* _TCP_SERVER_H_ */