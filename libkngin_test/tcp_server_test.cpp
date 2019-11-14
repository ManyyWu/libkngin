#include <string>
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
        m_server.set_read_done_cb(std::bind(&test_server::on_read_done, this));
        m_server.set_write_done_cb(std::bind(&test_server::on_write_done, this));
        m_server.set_connection_establish_cb(std::bind(&test_server::on_connection_establish, this));
        m_server.set_oob_cb(std::bind(&test_server::on_oob, this));
        m_server.set_close_cb(std::bind(&test_server::on_close, this));
        return m_server.run();
    }

    void
    stop ()
    {
        return m_server.stop();
    }

    void
    on_read_done ()
    {
    }

    void
    on_write_done ()
    {
    }

    void
    on_close ()
    {
    }

    void
    on_connection_establish ()
    {
    }

    void
    on_oob ()
    {
    }

protected:
    tcp_server m_server;
};

void
tcp_server_test ()
{
    tcp_server_opts _opts = {
        std::string("192.168.0.2"),
        20000,
        false,
        10,
        1
    };

    test_server _s(_opts);
    check(_s.run());

    thread::sleep(3600000);
}