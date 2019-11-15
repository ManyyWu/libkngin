#include <string>
#include <functional>
#include "tcp_server.h"
#include "thread.h"
#include "common.h"

using namespace k;

class test_server {
public:
    test_server (const tcp_server_opts &_opts)
        : m_server(_opts)
    {
    }

    bool
    run ()
    {
        m_server.set_read_done_cb(std::bind(&test_server::on_read_done, this, 
                                            std::placeholders::_1, 
                                            std::placeholders::_2, 
                                            std::placeholders::_3));
        m_server.set_write_done_cb(std::bind(&test_server::on_write_done, this, std::placeholders::_1));
        m_server.set_connection_establish_cb(std::bind(&test_server::on_connection_establish, this, std::placeholders::_1));
        m_server.set_oob_cb(std::bind(&test_server::on_oob, this, std::placeholders::_1, std::placeholders::_2));
        m_server.set_close_cb(std::bind(&test_server::on_close, this, std::placeholders::_1));
        return m_server.run();
    }

    void
    stop ()
    {
        return m_server.stop();
    }

    void
    on_read_done (tcp_connection &_conn, buffer &_buf, size_t _size)
    {
    }

    void
    on_write_done (tcp_connection &_conn)
    {
    }

    void
    on_close (tcp_connection &_conn)
    {
    }

    void
    on_connection_establish (tcp_server::tcp_connection_ptr _conn)
    {
    }

    void
    on_oob (tcp_connection &_conn, uint8_t _data)
    {
    }

protected:
    tcp_server m_server;
};

void
tcp_server_test ()
{
    tcp_server_opts _opts = {
        std::string("127.0.0.1"),
        20000,
        false,
        10,
        1
    };

    test_server _s(_opts);
    check(_s.run());

    thread::sleep(3600000);
}