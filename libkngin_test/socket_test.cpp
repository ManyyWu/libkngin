#include <cstring>
#include <iostream>
#include <functional>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/socket_test.cpp"

using namespace k;

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 40000

static int
client (void *_args)
{
    std::string _addr_str = {SERVER_ADDR};
    uint16_t    _port = SERVER_PORT;

    address _server_addr;
    assert(address::addrstr2addr(_addr_str, _port, _server_addr));

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
        log_info("read integer %d", _buf.peek_int32());
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
    std::string _addr_str = {SERVER_ADDR};
    uint16_t    _port = SERVER_PORT;
    address     _server_addr;

    assert(address::addrstr2addr(_addr_str, _port, _server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    check(_server_sock.valid());
    assert(sockopts::set_reuseaddr(_server_sock, true));
    assert(sockopts::set_reuseport(_server_sock, true));
    log_debug("server_addr: %s:%hu", _server_addr.addrstr().c_str(), _server_addr.port());
    if (_server_sock.bind(_server_addr) < 0)
        log_error("%s", strerror(errno));
    if (_server_sock.listen(5) < 0)
        log_error("%s", strerror(errno));
    log_info("listening...");
    while (_ok) {
        address _client_addr;
        k::socket _client_sock(_server_sock.accept(_client_addr));
        log_info("connect to: %s:%d", _client_addr.addrstr().c_str(),
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
                     _client_addr.addrstr().c_str(),
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
    k::thread _server_thr("server");
    _server_thr.run(std::bind(server, (void *)1));
    k::thread::sleep(1000);

    for (int i = 0; i< 100; ++i) {
        k::thread _client_thr("client");
        _client_thr.run(std::bind(client, (void *)1));
        _client_thr.join(nullptr);
    }

    _server_thr.join(nullptr);
}
