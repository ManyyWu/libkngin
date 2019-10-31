#include <cstring>
#include <iostream>
#include <functional>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"

using namespace k;

static int
client (void *_args)
{
    inet_addrstr _addr_str = {"127.0.0.1"};
    uint16_t     _port = 40000;

    address _server_addr;
    assert(address::str2sockaddr(_addr_str, _port,_server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    if (_server_sock.connect(_server_addr) < 0)
        log_error("%s", strerror(errno));
    log_info("connecting...");

    // read
    int _reply = 0;
    {
        buffer _buf(4);
        if (_server_sock.read(_buf, _buf.writeable()) < 0)
            log_error("%s", strerror(errno));
        log_info("myport %d", _buf.peek_int32());
    }
    std::cerr << "> ";
    std::cin >> _reply;
    // write
    {
        buffer _buf(4);
        _buf.write_int32(_reply);
        if (_server_sock.write(_buf, _buf.readable()) < 0)
            log_error("%s", strerror(errno));
    }

    _server_sock.close();
    return 0;
}

static int
server (void *_args)
{
    bool _ok = true;
    inet_addrstr _addr_str = {"127.0.0.1"};
    uint16_t     _port = 40000;

    address _server_addr;
    assert(address::str2sockaddr(_addr_str, _port,_server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    assert(sockopts::set_reuseaddr(_server_sock, true));
    assert(sockopts::set_reuseport(_server_sock, true));
    inet_addrstr _a;
    log_debug("server_addr: %s:%d", _server_addr.addrstr(_a), _server_addr.port());
    if (_server_sock.bind(_server_addr) < 0)
        log_error("%s", strerror(errno));
    if (_server_sock.listen(5) < 0)
        log_error("%s", strerror(errno));
    log_info("listening...");
    while (_ok) {
        address _client_addr;
        k::socket _client_sock(_server_sock.accept(_client_addr));
        inet_addrstr _client_addr_str;
        log_info("connect to: %s:%d", _client_addr.addrstr(_client_addr_str),
                _client_addr.port());
        // write
        {
            buffer _buf(4);
            _buf.write_int32(_client_addr.port());
            if (_client_sock.write(_buf, _buf.readable()) < 0)
                log_error("%s", strerror(errno));
        }
        // read
        {
            buffer _buf(4);
            if (_client_sock.read(_buf, _buf.writeable()) < 0)
                log_error("%s", strerror(errno));
            if (_buf.peek_int32() == 0)
                _ok = false;
            log_info("read integer %d from client %s:%d",
                    _buf.peek_int32(),
                    _client_addr.addrstr(_client_addr_str),
                    _client_addr.port());
        }
        _client_sock.close();
    }
    _server_sock.close();
    return 0;
}

void
socket_test ()
{
    k::thread _server_thr(std::bind(server, (void *)1), "server");
    _server_thr.run();
    k::thread::sleep(1000);

    for (int i = 0; i< 100; i++) {
        k::thread _client_thr(std::bind(client, (void *)1), "client");
        _client_thr.run();
        _client_thr.join(NULL);
    }

    _server_thr.join(NULL);
}
