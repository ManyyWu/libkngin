#include <cstring>
#include <iostream>
#include <functional>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"
#include "../libkngin/net/tcp/tcp_connection.h"
#include "../libkngin/net/io_thread.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/tcp_connection_test.cpp"

using namespace k;
using namespace k::tcp;

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

    int _reply = 0;
    std::cerr << "> " << std::endl;
    std::cin >> _reply;
    // write
    {
        buffer _buf(4);
        _buf.write_int32(_reply);
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
        log_info("c: read integer %d", _buf.peek_int32());
    }

    return 0;
}

class mythread : public io_thread {
public:
    mythread ()
        : io_thread("io_thread"),
          m_conn(nullptr),
          m_buf(std::make_shared<buffer>(4)),
          m_server_thr("server_thread")
    {
    }

    virtual
    ~mythread ()
    {
        delete m_conn;
    }

public:
    void
    run ()
    {
        io_thread::run();
        m_server_thr.run(std::bind(&mythread::process, this));
    }

protected:
    int
    process ()
    {
        try {
            // init server
            std::string _addr_str = {SERVER_ADDR};
            uint16_t    _port = SERVER_PORT;

            address _server_addr;
            assert(address::addrstr2addr(_addr_str, _port, _server_addr));

            k::socket _server_sock(socket::IPV4_TCP);
            assert(sockopts::set_reuseaddr(_server_sock, true));
            assert(sockopts::set_reuseport(_server_sock, true));
            std::string _a;
            log_info("s: server_addr: %s:%hu", _server_addr.addrstr().c_str(), _server_addr.port());
            if (_server_sock.bind(_server_addr) < 0)
                log_error("s: %s", strerror(errno));
            if (_server_sock.listen(5) < 0)
                log_error("s: %s", strerror(errno));
            log_info("s: listening...");

            // create a connection
            address _client_addr;
            k::socket _client_sock(_server_sock.accept(_client_addr));
            log_info("s: connected to client: %s:%hu", _client_addr.addrstr().c_str(),
                    _client_addr.port());
            m_conn = new tcp_connection(m_loop.get(), std::move(_client_sock),
                                        _server_addr, _client_addr);

            // set callback
            m_conn->set_message_cb([] (tcp_connection &_conn, buffer &_buf, size_t _size) {
                uint16_t _port = _conn.peer_addr().port();
                log_info("s: on_message: from %s:%d, data = \"%s\", size = %" PRIu64,
                         _conn.peer_addr().addrstr().c_str(), _port,
                         _buf.dump().c_str(), _size);
                typedef tcp_connection::buffer_ptr buffer_ptr;
                buffer_ptr _outbuf = std::make_shared<buffer>(4);
                _outbuf->write_uint32(_port);
                check(_conn.send(_outbuf));
            });
            m_conn->set_sent_cb([] (tcp_connection &_conn) {
                uint16_t _port = _conn.peer_addr().port();
                log_info("s: on_sent: to %s:%d",
                         _conn.peer_addr().addrstr().c_str(), _port);
                _conn.close();
            });
            m_conn->set_close_cb([] (const tcp_connection &_conn) {
                log_info("s: on_close: fd = %d", _conn.serial());
            });

            // recv
            {
                m_conn->recv(m_buf);
            }

            // send
        } catch (...) {
            if (m_conn)
                m_conn->close();
            throw;
        }
        return 0;
    }

protected:
    tcp_connection *           m_conn;

    tcp_connection::buffer_ptr m_buf;

    thread                     m_server_thr;
};

void
tcp_connection_test ()
{
    mythread _server_thr;
    _server_thr.run();

    thread _client("client");
    thread::sleep(1000);
    _client.run(client);
    _client.join(nullptr);
    _server_thr.get_loop()->stop();

    _server_thr.join(nullptr);
}
