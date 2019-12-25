#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <ctime>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/common.h"
#include "../libkngin/core/system_error.h"
#include "../libkngin/net/tcp/server.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/libkngin_test/tcp_server_test.cpp"

using namespace k;
using namespace k::tcp;

const char *g_data = "01234567889abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.";
const int   g_data_size = 64;
const int   times = 10;

class test_server {
public:
    test_server (event_loop &_loop, const server_opts &_opts)
        : m_loop(_loop.pimpl()),
          m_server(_loop, _opts),
          m_sessions(),
          m_sessions_mutex()
    {
    }

    bool
    run ()
    {
        m_server.set_session_handler([this] (server::session_ptr _session) {
            assert(_session);
            log_info("new session from %s", _session->name().c_str());

            // create session info
            {
                //local_lock _lock(m_sessions_mutex);
                if (!_session->connected()) {
                    if (!_session->closed())
                        _session->close();
                    return;
                }
                assert(m_sessions.find(_session->key()) == m_sessions.end());
                m_sessions.insert(std::make_pair(_session->key(), session_info(_session)));
                //log_debug("size: %d", m_sessions.size());
            }

#define CLOSE_COND 0
#if (true == !!CLOSE_COND)
            if (_session->peer_addr().port() > 50000) {
                _session->close();
                m_loop->stop();
                return;
            }
#endif

            // process
            process(_session);
        });

        m_server.set_close_handler([this] (const tcp::session &_session, std::error_code) {
            log_info("session %s closed", _session.name().c_str());
            {
                //local_lock _lock(m_sessions_mutex);
                if (m_sessions.find(_session.key()) == m_sessions.end()) // closed before add
                    return;
                m_sessions.erase(_session.key());
                //log_debug("size: %d", m_sessions.size());
            }
        });

        m_server.set_crash_handler([this] () {
            //assert(!"server crashed");
            m_server.stop();
            m_loop->stop();
        });
        return m_server.run();
    }

    void
    stop ()
    {
        m_server.stop();
    }

    void
    process (server::session_ptr _session)
    {
        msg_buffer::uint8_arr_ptr _msg_arr = k::make_shared_array<char>(8);
        in_buffer(_msg_arr.get(), 8).write_int32(times).write_int32(g_data_size);
        _session->send( // send times and buf size
            msg_buffer(_msg_arr, 0, 8), [] (session &_s)
        {
            std::shared_ptr<std::array<char, 8>> _arr = std::make_shared<std::array<char, 8>>();
            _s.recv( // recv ack
                in_buffer(_arr->data(), 8), [_arr] (session &_s, in_buffer _buf, size_t _size)
            {
                if_not (_size == 8)
                    return;
                if (out_buffer(_buf.begin(), 4).peek_int32() != times ||
                    out_buffer(_buf.begin() + 4, 4).peek_int32() != g_data_size) {
                    log_error("ack error");
                    _s.close();
                    return;
                }
                for (int i = 0; i < times; i++) {
                    msg_buffer::uint8_arr_ptr _msg_arr = k::make_shared_array<char>(g_data_size);
                    in_buffer(_msg_arr.get(), g_data_size).write_bytes(g_data, g_data_size);
                    _s.send( // send data
                        msg_buffer(_msg_arr, 0, g_data_size),
                        [] (session &_s)
                    {
                        std::shared_ptr<std::array<char, g_data_size>> _arr1 = std::make_shared<std::array<char, g_data_size>>();
                        _s.recv( // recv reverse data
                            in_buffer(_arr1->data(), _arr1->size()), [_arr1] (session &_s, in_buffer _buf, size_t _size)
                        {
                            log_info("recv data %s from %s",
                                     out_buffer(_buf.begin(), _buf.size()).dump().c_str(),
                                     _s.name().c_str());
                        });
                    });
                }
            });
        });
    }

protected:
    event_loop_pimpl_ptr   m_loop;

    tcp::server            m_server;

    struct session_info {
        session::session_ptr si_session;

        session_info (session::session_ptr &_session)
            : si_session(_session) {}
    };
    typedef std::map<std::string, session_info> buffer_map;
    buffer_map             m_sessions;
    mutex                  m_sessions_mutex;
};

void
tcp_server_test ()
{
#define SERVER_ADDR "192.168.0.2"
//#define SERVER_ADDR "127.0.0.1"
//#define SERVER_ADDR "fe80::26e4:35c1:eea7:68a2%eno1"
//#define SERVER_ADDR "::1%16"
#define SERVER_PORT 20000

    tcp::server_opts _opts = {
        .name                   = SERVER_ADDR,
        .port                   = SERVER_PORT,
        .allow_ipv4             = true,
        .allow_ipv6             = false,
        .backlog                = 10000,
        .thread_num             = 3,
        .disable_debug          = false,
        .disable_info           = false,
        .separate_listen_thread = true,
        .keep_alive             = true
    };
    event_loop _loop;

    test_server _s(_loop, _opts);
    assert(_s.run());

    _loop.run(nullptr, nullptr);

    _s.stop();
}
