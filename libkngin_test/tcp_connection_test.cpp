#include <cstring>
#include <iostream>
#include <functional>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"
#include "../libkngin/net/tcp/tcp_connection.h"

using namespace k;

//#define SERVER_ADDR "192.168.0.2"
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 20000

static int
client (void *_args)
{
    inet_addrstr _addr_str = {SERVER_ADDR};
    uint16_t     _port = SERVER_PORT;

    address _server_addr;
    assert(address::str2sockaddr(_addr_str, _port,_server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    log_info("c: connecting...");
    if (_server_sock.connect(_server_addr) < 0) {
        log_error("%s", strerror(errno));
close:
        _server_sock.close();
        log_info("c: client closed");
    }
    log_info("c: connected");

    int _reply = 0;
    std::cerr << "> ";
    std::cin >> _reply;
    // write
    {
        buffer _buf(4);
        _buf.write_int32(_reply);
        if (_server_sock.write(_buf, _buf.readable()) < 0) {
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
        log_info("c: port %d", _buf.peek_int32());
    }

    return 0;
}

static int
client_mgr (void *_args)
{
    thread::sleep(1000);
    for (int i = 0; i< 100; i++) {
        k::thread _client_thr("client");
        _client_thr.run(std::bind(client, (void *)1));
        _client_thr.join(NULL);
    }
}

class mythread : public thread {
public:
    explicit
    mythread ()
        : thread("server"),
          m_loop(NULL),
          m_conn(NULL)
    {
        thread::run(std::bind(&mythread::server, this));
    }

public:
    event_loop *
    get_loop ()
    {
        return m_loop;
    }

    bool
    looping ()
    {
        return (m_loop && m_loop->looping());
    }

    tcp_connection *
    get_conn ()
    {
        return m_conn;
    }

    bool
    connected ()
    {
        return (looping() && m_conn);
    }

public:
    static int
    server (void *_args)
    {
        mythread *_thr = (mythread *)_args;
        event_loop _loop(_thr);
        _thr->m_loop = &_loop;

        try {
            // init server
            bool _ok = true;
            inet_addrstr _addr_str = {SERVER_ADDR};
            uint16_t     _port = SERVER_PORT;

            address _server_addr;
            assert(address::str2sockaddr(_addr_str, _port,_server_addr));

            k::socket _server_sock(socket::IPV4_TCP);
            assert(sockopts::set_reuseaddr(_server_sock, true));
            assert(sockopts::set_reuseport(_server_sock, true));
            inet_addrstr _a;
            log_info("s: server_addr: %s:%hu", _server_addr.addrstr(_a), _server_addr.port());
            if (_server_sock.bind(_server_addr) < 0)
                log_error("s: %s", strerror(errno));
            if (_server_sock.listen(5) < 0)
                log_error("s: %s", strerror(errno));
            log_info("s: listening...");

            // create a connection
            address _client_addr;
            k::socket _client_sock(_server_sock.accept(_client_addr));
            inet_addrstr _client_addr_str;
            log_info("s: connect to: %s:%hu", _client_addr.addrstr(_client_addr_str),
                    _client_addr.port());
            tcp_connection _conn(&_loop, std::move(_client_sock),
                                 std::move(address(_server_addr)), std::move(address(_client_addr)));
            _thr->m_conn = &_conn;
            _conn.set_read_done_cb([] (tcp_connection &_conn, buffer &_buf, size_t _size) {
                inet_addrstr _client_addr_str;
                uint16_t _port = _conn.peer_addr().port();
                log_info("s: on_message: from %s:%d, data = \"%s\", size = %" PRIu64,
                        _conn.peer_addr().addrstr(_client_addr_str), _port,
                        _buf.dump().c_str(), _size);
                buffer _outbuf(4);
                _outbuf.write_uint32(_port);
                _conn.send(std::move(_outbuf));
            });
            _conn.set_write_done_cb([] (tcp_connection &_conn) {
                inet_addrstr _client_addr_str;
                uint16_t _port = _conn.peer_addr().port();
                log_info("s: write done: to %s:%d",
                         _conn.peer_addr().addrstr(_client_addr_str), _port);
            });
            _conn.set_close_cb([] (tcp_connection &_conn) {
                log_info("s: close: fd = %d", _conn.socket().fd());
            });

            // loop
            _loop.loop();
        } catch (...) {
            _thr->m_loop = NULL;
            throw;
        }
    }

protected:
    bool
    run ()
    {}

protected:
    event_loop *    m_loop;

    tcp_connection *m_conn;
};

void
tcp_connection_test ()
{
    mythread _server_thr;
    thread _client_mgr("client mgr");
    _client_mgr.run(client_mgr);

    while (!_server_thr.looping())
        thread::sleep(100);
    while (!_server_thr.connected())
        thread::sleep(100);
    tcp_connection *_conn = _server_thr.get_conn();

    buffer _inbuf(4);
    _conn->recv(_inbuf);

    _client_mgr.join(NULL);
    _server_thr.join(NULL);
}
