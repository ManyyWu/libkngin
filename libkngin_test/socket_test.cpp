#include <iostream>
#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/logfile.h"
#include "../libkngin/core/base/buffer.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"

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
    k::socket sock(k::socket::IPV4_TCP);
    log_info("connecting...");
    sock.connect(address(SERVER_ADDR, SERVER_PORT, false));

    int reply = 0;
    {
        char arr[4];
        in_buffer buf(arr, 4);
        sock.read(buf);
        log_info("read integer %d", out_buffer(arr, 4).peek_int32());
    }

    std::cerr << "> ";
    std::cin >> reply;
    // write
    {
        char arr[4];
        in_buffer(arr, 4).write_int32(reply);
        out_buffer buf(arr, 4);
        sock.write(buf);
    }

    sock.close();
    return 0;
}

static int
server ()
{
    bool ok = true;
    std::string addr_str = SERVER_ADDR;
    uint16_t    port = SERVER_PORT;
    address     server_addr(addr_str, port, false);

    k::socket server_sock(socket::IPV4_TCP);
    sockopts::set_reuseaddr(server_sock, true);
#ifndef _WIN32
    sockopts::set_reuseport(server_sock, true);
#endif
    server_sock.bind(server_addr);
    server_sock.listen(5);
    log_info("listening...");
    while (ok) {
        address client_addr;
        k::socket client_sock(server_sock.accept(client_addr));
        // write
        {
            char arr[4];
            in_buffer(arr, 4).write_int32(client_addr.port());
            out_buffer buf(arr, 4);
            client_sock.write(buf);
        }
        // read
        {
            char arr[4];
            in_buffer buf(arr, 4);
            client_sock.read(buf);
            if (out_buffer(arr, 4).peek_int32() == 0)
                ok = false;
            log_info("read integer %d from client %s",
                     out_buffer(arr, 4).peek_int32(),
                     client_sock.name().c_str());
        }
        client_sock.close();
    }
    server_sock.close();
    return 0;
}

void
socket_test ()
{
    k::thread server_thr("server");
    server_thr.run(server);
    k::thread::sleep(1000);

    for (int i = 0; i< 100; ++i) {
        k::thread client_thr("client");
        client_thr.run(client);
        if (client_thr.join())
            break;
    }

    server_thr.join();
}

