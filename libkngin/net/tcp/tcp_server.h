#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <unordered_map>
#include "define.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "address.h"
#include "event_loop.h"
#include "server_opts.h"
#include "listener.h"
#include "io_threadpool.h"

KNGIN_NAMESPACE_K_BEGIN
KNGIN_NAMESPACE_TCP_BEGIN

class tcp_server {
public:
    typedef tcp_connection::tcp_connection_ptr               tcp_connection_ptr;

    typedef std::function<void (tcp_connection_ptr)>         connection_establish_cb;

    typedef tcp_connection::message_cb                       message_cb;

    typedef tcp_connection::sent_cb                          sent_cb;

    typedef tcp_connection::oob_cb                           oob_cb;

    typedef tcp_connection::close_cb                         close_cb;

    typedef io_thread::event_loop_ptr                        event_loop_ptr;

    typedef std::shared_ptr<listener>                        listener_ptr;

    typedef std::vector<tcp_connection *>                    tcp_connection_list;

    typedef std::unordered_map<uint32_t, tcp_connection_ptr> tcp_connection_map;

public:
    tcp_server                  () = delete;

    tcp_server                  (const tcp_server_opts &_opts);

    ~tcp_server                 ();

public:
    bool
    run                         ();

    void
    stop                        ();

    size_t
    conn_num                    ();

public:
    void
    remove_connection           (tcp_connection_ptr _conn);

public:
    int
    broadcast                   (tcp_connection_list &_list, buffer &&_buf);

public:
    void
    set_connection_establish_cb (connection_establish_cb &&_cb) { m_connection_establish_cb = std::move(_cb); }
    void
    set_message_cb              (message_cb &&_cb)              { m_message_cb = std::move(_cb); }
    void
    set_sent_cb                 (sent_cb &&_cb)                 { m_sent_cb = std::move(_cb); }
    void
    set_close_cb                (close_cb &&_cb)                { m_close_cb = std::move(_cb); }
    void
    set_oob_cb                  (oob_cb &&_cb)                  { m_oob_cb = std::move(_cb); }

protected:
    event_loop_ptr
    assign_thread               ()                              { return m_threadpool.next_loop(); }

protected:
    bool
    parse_addr                  (const std::string &_name, uint16_t _port);

protected:
    void
    on_new_connection           (socket &&_sock);

    void
    on_close                    (const tcp_connection &_conn);

    void
    on_listener_error           (listener &_conn);

protected:
    const tcp_server_opts   m_opts;

    io_threadpool           m_threadpool;

    tcp_connection_map      m_connections;

    listener_ptr            m_listener;

    address                 m_listen_addr;

    connection_establish_cb m_connection_establish_cb;

    sent_cb                 m_sent_cb;

    message_cb              m_message_cb;

    oob_cb                  m_oob_cb;

    close_cb                m_close_cb;

    std::atomic<bool>       m_stopped;

    mutex                   m_mutex;
};

KNGIN_NAMESPACE_TCP_END
KNGIN_NAMESPACE_K_END

#endif /* _TCP_SERVER_H_ */