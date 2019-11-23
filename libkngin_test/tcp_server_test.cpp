#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <ctime>
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
    assert(address::addrstr2addr(_addr_str, _port, _server_addr));

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

    // read
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

    return 0;
}


class test_server {
public:
    test_server (const tcp_server_opts &_opts)
        : m_server(_opts),
          m_bufs(),
          m_mutex(),
          m_savetime(time(NULL)),
          m_times(0)
    {
    }

    bool
    run ()
    {
        m_server.set_message_cb(std::bind(&test_server::on_message, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3));
        m_server.set_sent_cb(std::bind(&test_server::on_sent, this, std::placeholders::_1));
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
    on_message (tcp_connection &_conn, buffer &_buf, size_t _size)
    {
        log_info("readed %d bytes from connection [%s:%d - %s:%d], data: %s", 
                 _size,
                 _conn.local_addr().addrstr().c_str(), 
                 _conn.local_addr().port(),
                 _conn.peer_addr().addrstr().c_str(), 
                 _conn.peer_addr().port(),
                 _buf.dump().c_str());
        if (1 == _buf.peek_uint32()) {
           // m_server.stop();
           // return;
        }
        std::shared_ptr<buffer> _buf1 = nullptr;
        {
            local_lock _lock(m_mutex);
            _buf1 = m_bufs[_conn.serial()];
        }
        check(_buf1);

        _buf1->resize(4);
        _buf1->reset();
        _buf1->write_uint32(_conn.peer_addr().port());
        _conn.send(_buf1);
    }

    void
    on_sent (tcp_connection &_conn)
    {
        log_info("connection [%s:%d - %s:%d] written done",
                 _conn.local_addr().addrstr().c_str(), 
                 _conn.local_addr().port(),
                 _conn.peer_addr().addrstr().c_str(),
                 _conn.peer_addr().port()
                 );

        std::shared_ptr<buffer> _buf = nullptr;
        {
            local_lock _lock(m_mutex);
            _buf = m_bufs[_conn.serial()];
        }
        check(_buf);

        _buf->resize(4);
        _buf->reset();
        _conn.recv(_buf);
    }

    void
    on_close (const tcp_connection &_conn)
    {
        log_info("connection [%s:%d - %s:%d] closed",
                 _conn.local_addr().addrstr().c_str(),
                 _conn.local_addr().port(),
                 _conn.peer_addr().addrstr().c_str(),
                 _conn.peer_addr().port()
                 );

        {
            local_lock _lock(m_mutex);
            m_bufs.erase(_conn.serial());
        }
    }

    void
    on_connection_establish (tcp_server::tcp_connection_ptr _conn)
    {
        check(_conn);
        log_info("new connection [%s:%d - %s:%d]",
                 _conn->local_addr().addrstr().c_str(), 
                 _conn->local_addr().port(),
                 _conn->peer_addr().addrstr().c_str(),
                 _conn->peer_addr().port()
                 );
        std::shared_ptr<buffer> _buf = nullptr;
        uint64_t                _size = 0;
        {
            local_lock _lock(m_mutex);
            m_bufs[_conn->serial()] = std::make_shared<buffer>(4);
            _buf = m_bufs[_conn->serial()];
            _size = m_bufs.size();
        }
        check(_buf);

        _buf->resize(4);
        _buf->reset();
        _buf->write_uint32(_conn->peer_addr().port());
        _conn->send(_buf);

        if (m_savetime == time(NULL)) {
            m_times++;
        } else {
            log_warning("accepted %" PRIu64 " new connections in last second, "
                        "current connection num = %" PRIu64,
                        m_times.load(), m_server.conn_num());
            m_savetime = time(NULL);
            m_times = 0;
        }
    }

    void
    on_oob (tcp_connection &_conn, uint8_t _data)
    {
    }

protected:
    tcp_server                                       m_server;

    std::unordered_map<uint64_t, std::shared_ptr<buffer>> m_bufs;

    mutex                                            m_mutex;

    time_t                                           m_savetime;

    std::atomic<uint64_t>                            m_times;
};

void
tcp_server_test ()
{
    tcp_server_opts _opts = {
        .name                   = std::string(SERVER_ADDR),
        .port                   = SERVER_PORT,
        .allow_ipv6             = false,
        .backlog                = 100,
        .thread_num             = 1,
        .disable_debug          = true,
        .disable_info           = true,
        .separate_listen_thread = true,
    };
    test_server _s(_opts);
    check(_s.run());

    //for (int i = 0; i < 10000; i++) {
    //    thread _client((std::string("client") + std::to_string(i)).c_str());
    //    _client.run(client);
    //    _client.join(nullptr);
    //}
    std::cin.get();
    _s.stop();
    //thread::sleep(3600000);
}