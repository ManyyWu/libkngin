#include <cstring>
#include <cassert>
#include <iostream>
#include <functional>
#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/logfile.h"
#include "../libkngin/core/base/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/socket_test.cpp"

using namespace k;

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 40000

static int
client ()
{
    std::string _addr_str = {SERVER_ADDR};
    uint16_t    _port = SERVER_PORT;

    address _server_addr(_addr_str, _port, false);
    k::socket _server_sock(socket::IPV4_TCP);
    std::error_code _ec;
    _server_sock.connect(_server_addr, _ec);
    if (_ec) {
        log_error("connect() error, %s", system_error_str(_ec).c_str());
        return 1;
    }
    log_info("connecting...");

    // read
    int _reply = 0;
    {
        char _arr[4];
        in_buffer _buf(_arr, 4);
        _server_sock.read(_buf);
        log_info("read integer %d", out_buffer(_arr, 4).peek_int32());
    }
    std::cerr << "> ";
    std::cin >> _reply;
    // write
    {
        char _arr[4];
        in_buffer(_arr, 4).write_int32(_reply);
        out_buffer _buf(_arr, 4);
        _server_sock.write(_buf);
    }

    _server_sock.close();
    return 0;
}

static int
server ()
{
    bool _ok = true;
    std::string _addr_str = SERVER_ADDR;
    uint16_t    _port = SERVER_PORT;
    address     _server_addr(_addr_str, _port, false);

    k::socket _server_sock(socket::IPV4_TCP);
    sockopts::set_reuseaddr(_server_sock, true);
    sockopts::set_reuseport(_server_sock, true);
    _server_sock.bind(_server_addr);
    _server_sock.listen(5);
    log_info("listening...");
    while (_ok) {
        address _client_addr;
        k::socket _client_sock(_server_sock.accept(_client_addr));
        // write
        {
            char _arr[4];
            in_buffer(_arr, 4).write_int32(_client_addr.port());
            out_buffer _buf(_arr, 4);
            _client_sock.write(_buf);
        }
        // read
        {
            char _arr[4];
            in_buffer _buf(_arr, 4);
            _client_sock.read(_buf);
            if (out_buffer(_arr, 4).peek_int32() == 0)
                _ok = false;
            log_info("read integer %d from client %s",
                     out_buffer(_arr, 4).peek_int32(),
                     _client_sock.name().c_str());
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
    _server_thr.run(server);
    k::thread::sleep(1000);

    for (int i = 0; i< 100; ++i) {
        k::thread _client_thr("client");
        _client_thr.run(client);
        if (_client_thr.join())
            break;
    }

    _server_thr.join();
}
