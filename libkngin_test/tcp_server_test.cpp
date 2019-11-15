#include <string>
#include <functional>
#include "tcp_server.h"
#include "thread.h"
#include "common.h"

using namespace k;

//#define SERVER_ADDR "192.168.0.2"
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 20000

static int
client ()
{
    std::string _addr_str = {SERVER_ADDR};
    uint16_t    _port = SERVER_PORT;

    address _server_addr;
    assert(address::str2sockaddr(_addr_str, _port, _server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    log_info("c: connecting...");
    if (_server_sock.connect(_server_addr) < 0) {
        log_error("%s", strerror(errno));
close:
        _server_sock.close();
        log_info("c: client closed");
        return 0;
    }
    log_info("c: connected");

    // read
    uint32_t _reply;
    {
        buffer _buf(4);
        int _ret = _server_sock.read(_buf, _buf.writeable());
        if (_ret < 0) {
            log_error("c: %s", strerror(errno));
            goto close;
        }
        if (!_ret)
            goto close;
        log_info("c: read integer %d", _reply = _buf.peek_uint32());
    }

    // write
    {
        buffer _buf(4);
        _buf.write_uint32(_reply);
        if (_server_sock.write(_buf, _buf.readable()) <= 0) {
                    log_error("c: %s", strerror(errno));
            goto close;
        }
    }

    return 0;
}


class test_server {
public:
    test_server (const tcp_server_opts &_opts)
        : m_server(_opts),
          m_buf(4)
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
        m_server.stop();
    }

    void
    on_read_done (tcp_connection &_conn, buffer &_buf, size_t _size)
    {
        log_info("readed %d bytes from connection [%s:%d - %s:%d], data: %s", 
                 _size,
                 _conn.local_addr().addrstr().c_str(), 
                 _conn.peer_addr().addrstr().c_str(), 
                 _buf.dump());
        _conn.send(_buf);
    }

    void
    on_write_done (tcp_connection &_conn)
    {
        log_info("connection [%s:%d - %s:%d] writen done",
                 _conn.local_addr().addrstr().c_str(), 
                 _conn.peer_addr().addrstr().c_str()
                 );
    }

    void
    on_close (const tcp_connection &_conn)
    {
        log_info("connection [%s:%d - %s:%d] closed");
    }

    void
    on_connection_establish (tcp_server::tcp_connection_ptr _conn)
    {
        log_info("new connection [%s:%d - %s:%d]",
                 _conn->local_addr().addrstr().c_str(), 
                 _conn->peer_addr().addrstr().c_str()
                 );
        m_buf.reset();
        m_buf.write_uint32(_conn->peer_addr().port());
        _conn->send(m_buf);
    }

    void
    on_oob (tcp_connection &_conn, uint8_t _data)
    {
    }

protected:
    tcp_server m_server;

    buffer     m_buf;
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

    for (int i = 0; i < 100; i++) {
        thread _client((std::string("client") + std::to_string(i)).c_str());
        thread::sleep(1000);
        _client.run(client);
        _client.join(nullptr);
    }
    thread::sleep(3600000);
}