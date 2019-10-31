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

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 40000

static int
client (void *_args)
{

}

static void
on_message (tcp_connection &_conn, buffer &_buf, size_t _size)
{
    log_info("on_message: fd = %d, data = \"%s\", size = %ul", 
             _conn.socket().fd(), _buf.dump().c_str(), _size);
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
    log_debug("server_addr: %s:%d", _server_addr.addrstr(_a), _server_addr.port());
    if (_server_sock.bind(_server_addr) < 0)
        log_error("%s", strerror(errno));
    if (_server_sock.listen(5) < 0)
        log_error("%s", strerror(errno));
    log_info("listening...");

    // create a connection
    {
        address _client_addr;
        k::socket _client_sock(_server_sock.accept(_client_addr));
        inet_addrstr _client_addr_str;
        log_info("connect to: %s:%d", _client_addr.addrstr(_client_addr_str),
                _client_addr.port());
        tcp_connection _conn(&_loop, std::move(_client_sock), 
                             std::move(address(_server_addr)), std::move(address(_client_addr)));
    }

    // loop
    _loop.loop(NULL);
}

void
tcp_connection_test ()
{

    
}
