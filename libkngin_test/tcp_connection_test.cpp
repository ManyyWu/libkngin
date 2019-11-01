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
    if (_server_sock.connect(_server_addr) < 0)
        log_error("%s", strerror(errno));
    log_info("c: connected");

    // read
    int _reply = 0;
    {
        buffer _buf(4);
        int _ret = _server_sock.read(_buf, _buf.writeable());
        if (_ret < 0)
            log_error("c: %s", strerror(errno));
        if (!_ret)
            goto close;
        log_info("c: port %d", _buf.peek_int32());
    }
    std::cerr << "> ";
    std::cin >> _reply;
    // write
    {
        buffer _buf(4);
        _buf.write_int32(_reply);
        if (_server_sock.write(_buf, _buf.readable()) < 0)
            log_error("c: %s", strerror(errno));
    }

close:
    _server_sock.close();
    log_info("c: client closed");
    return 0;
}

static int 
server (void *_args)
{
    event_loop _loop((thread *)_args);

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
    _conn.set_read_done_cb([] (tcp_connection &_conn, buffer &_buf, size_t _size) {
        log_info("s: on_message: fd = %d, data = \"%s\", size = %" PRIu64,
        _conn.socket().fd(), _buf.dump().c_str(), _size);
    });
    _conn.set_write_done_cb([] (tcp_connection &_conn) {
        log_info("s: write done: fd = %d", _conn.socket().fd());
    });
    _conn.set_close_cb([] (tcp_connection &_conn) {
        log_info("s: close: fd = %d", _conn.socket().fd());
    });

    // loop
    _loop.loop();
}

void
tcp_connection_test ()
{
    k::thread _server_thr("server");
    _server_thr.run(std::bind(server, &_server_thr));
    k::thread::sleep(1000);

    for (int i = 0; i< 100; i++) {
        k::thread _client_thr("client");
        _client_thr.run(std::bind(client, (void *)1));
        _client_thr.join(NULL);
    }
    _server_thr.join(NULL);
}
