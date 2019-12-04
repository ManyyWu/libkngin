#include <cstring>
#include <iostream>
#include <functional>
#include <cassert>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/common.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/system_error.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/address.h"
#include "../libkngin/net/tcp/session.h"
#include "../libkngin/net/io_thread.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/session_test.cpp"

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
    _server_sock.connect(_server_addr);
close:
        //_server_sock.close();
        //log_info("c: client closed");
        //return 0;
    log_info("c: connected");

    int _reply = 0;
    std::cerr << "> " << std::endl;
    std::cin >> _reply;
    // write
    {
        char _arr[4];
        in_buffer(_arr, 4).write_int32(_reply);
        out_buffer _buf(_arr, 4);
        std::error_code _ec;
        _server_sock.write(_buf, _ec);
        if (_ec) {
            log_error("c: write error - %s", system_error_str(_ec).c_str());
            goto close;
        }
    }

    // read
    {
        char _arr[4];
        in_buffer _buf(_arr, 4);
        std::error_code _ec;
        if (!_server_sock.read(_buf, _ec))
            goto close;
        if (_ec) {
            log_error("c: read error - %s", system_error_str(_ec).c_str());
            goto close;
        }

        log_info("c: read integer %d", out_buffer(_arr, 4).peek_int32());
    }

    return 0;
}

class mythread : public io_thread {
public:
    mythread ()
        : io_thread("io_thread"),
          m_session(nullptr),
          m_in_buf(std::make_shared<in_buffer>(m_in_arr, 4)),
          m_server_thr("server_thread")
    {
    }

    virtual
    ~mythread ()
    {
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
            sockopts::set_reuseaddr(_server_sock, true);
            sockopts::set_reuseport(_server_sock, true);
            std::string _a;
            log_info("s: server_addr: %s", _server_sock.name().c_str());
            _server_sock.bind(_server_addr);
            _server_sock.listen(5);
            log_info("s: listening...");

            // create a session
            address _client_addr;
            k::socket _client_sock(_server_sock.accept(_client_addr));
            log_info("s: connected to client: %s", _client_sock.name().c_str());
            m_session = std::make_shared<session>(m_loop, std::move(_client_sock),
                                                  _server_addr, _client_addr);

            // set callback
            m_session->set_message_handler([this] (session &_session, in_buffer &_buf, size_t _size) {
                uint16_t _port = _session.peer_addr().port();
                log_info("s: on_message: from %s, data = \"%s\", size = %" PRIu64,
                        _session.name().c_str(),
                         _buf.dump().c_str(), _size);
                typedef session::out_buffer_ptr buffer_ptr;
                in_buffer(m_arr, 4).write_uint32(_port);
                check(_session.send(std::make_shared<out_buffer>(m_arr, 4)));
            });
            m_session->set_sent_handler([] (session &_session) {
                uint16_t _port = _session.peer_addr().port();
                log_info("s: on_sent: to %s", _session.name().c_str());
                thread::sleep(1000);
            });
            m_session->set_close_handler([] (const session &_session, std::error_code _ec) {
                log_info("s: on_close: fd = %d - %s", _session.serial(), system_error_str(_ec).c_str());
            });

            // recv
            {
                m_session->recv(m_in_buf);
            }
        } catch (...) {
            if (m_session)
                m_session->close();
            throw;
        }
        return 0;
    }

protected:
    tcp::session::session_ptr m_session;

    char                      m_arr[4];

    char                      m_in_arr[4];
    session::in_buffer_ptr    m_in_buf;

    thread                    m_server_thr;
};

void
tcp_connection_test ()
{
    mythread _server_thr;
    _server_thr.run();

    thread _client("client");
    thread::sleep(1000);
    _client.run(client);
    _client.join();
    _server_thr.get_loop()->stop();

    _server_thr.join();
}
