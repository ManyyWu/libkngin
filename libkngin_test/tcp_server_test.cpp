#include <string>
#include <map>
#include <ctime>
#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/memory.h"
#include "../libkngin/core/base/common.h"
#include "../libkngin/core/base/system_error.h"
#include "../libkngin/core/event/io_thread.h"
#include "../libkngin/net/tcp/server.h"
#ifndef _WIN32
#include <mcheck.h>
#endif

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/libkngin_test/tcp_server_test.cpp"

using namespace k;
using namespace k::tcp;

const char *g_data = "01234567889abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.";
const int   g_data_size = 10176;
const int   times = 10;

std::atomic_size_t g_total_size(0);
timestamp          g_ts(0);

class test_server {
public:
    test_server (event_loop &_loop, const server_opts &_opts)
        : m_loop(&_loop),
          m_server(_loop, _opts),
          m_sessions(),
          m_sessions_mutex()
    {
    }

    bool
    run ()
    {
        m_server.set_session_handler([this] (server::session_ptr _session) {
            if (_session->closed())
                return;
            log_info("new session from %s", _session->name().c_str());

            g_ts = timestamp::realtime();
            // create session info
            {
                local_lock _lock(m_sessions_mutex);
                if (m_sessions.find(_session->key()) != m_sessions.end()) {
                    log_warning("existed session %s", _session->name().c_str());
                    _session->close();
                    return;
                }
                m_sessions.insert(std::make_pair(_session->key(), session_info(_session)));
                //log_debug("size: %d", m_sessions.size());
            }

#define CLOSE_COND 0
#if (true == !!CLOSE_COND)
            if (_session->peer_addr().port() > 50000) {
                auto _loop = m_loop.lock();
                assert(_loop);
                _loop->run_in_loop([this, _loop] () {
                    stop();
                    _loop->stop();
                });
                return;
            }
#endif
            // process
            process(_session);
        }); // end of new_session_handler, run in listner thread

        m_server.set_error_handler([this] (tcp::session &_session, std::error_code _ec) {
            log_info("session %s error - %s",
                     _session.name().c_str(), system_error_str(_ec).c_str());
            {
                local_lock _lock(m_sessions_mutex);
                assert(m_sessions.find(_session.key()) != m_sessions.end());
                m_sessions.erase(_session.key());
                size_t _size = m_sessions.size();
                log_debug("size: %" PRIu64, _size);
            }
            _session.close();
            log_warning("total read:%" PRIu64 ", time: %" PRIu64 "ms",
            g_total_size.load(), (timestamp::realtime() - g_ts).value());
        }); // end of session_error_handler, run in any thread of pool

        m_server.set_crash_handler([this] () {
            log_fatal("server crashed");
            stop();
            m_loop->stop();
        }); // end of server_crash_handler, run in main thread
        return m_server.run();
    }

    void
    stop ()
    {
        m_server.stop();
        {
            local_lock _lock(m_sessions_mutex);
            for (auto &_iter : m_sessions) {
                if (!_iter.second.si_session->closed())
                    _iter.second.si_session->close(true);
            }
            m_sessions.clear();
        }
    }

    void
    process (server::session_ptr _session)
    {
        uint8_arr_ptr _msg_arr = k::make_shared_array<char>(8);
        in_buffer(_msg_arr.get(), 8).write_int32(times).write_int32(g_data_size);
        _session->send( // send times and buf size
            msg_buffer(_msg_arr, 0, 8),
            [] (session &_s)
        {
            uint8_arr_ptr _arr = k::make_shared_array<char>(8);
            _s.recv( // recv ack
                in_buffer(_arr.get(), 8),
                [_arr] (session &_s, in_buffer _buf, size_t _size)
            {
                if_not (_size == 8)
                    return;
                if (out_buffer(_buf.begin(), 4).peek_int32() != times or
                    out_buffer(_buf.begin() + 4, 4).peek_int32() != g_data_size) {
                    log_error("ack error");
                    _s.close();
                    return;
                }
                for (int i = 0; i < times; i++) {
/*                    uint8_arr_ptr _msg_arr = k::make_shared_array<char>(g_data_size);
                    in_buffer(_msg_arr.get(), g_data_size).write_bytes(g_data, g_data_size);
                    _s.send( // send data
                        msg_buffer(_msg_arr, 0, g_data_size),
                        [] (session &_s)
                    {*/
                        uint8_arr_ptr _arr1 = k::make_shared_array<char>(g_data_size);
                        _s.recv( // recv reverse data
                            in_buffer(_arr1.get(), g_data_size),
                            [_arr1] (session &_s, in_buffer _buf, size_t _size)
                        {
                            if (_size < g_data_size)
                                log_debug("first");
                            else
                                g_total_size += _size;
                            //log_info("recv data %s from %s", out_buffer(_buf.begin(), _buf.size()).dump().c_str(), _s.name().c_str());
                        }, 10);
//                    });
                }
            });
        });
    }

protected:
    event_loop *m_loop;

    tcp::server m_server;

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
//#define SERVER_ADDR "192.168.0.2"
#define SERVER_ADDR "127.0.0.1"
//#define SERVER_ADDR "fe80::26e4:35c1:eea7:68a2%eno1"
//#define SERVER_ADDR "::1%16"
#define SERVER_PORT 20000
    tcp::server_opts _opts = {
        .name                   = SERVER_ADDR,
        .port                   = SERVER_PORT,
        .allow_ipv4             = true,
        .allow_ipv6             = false,
        .backlog                = 10000,
        .thread_num             = 4,
        .disable_debug          = false,
        .disable_info           = false,
        .separate_listen_thread = true,
        .keep_alive             = true
    };
    event_loop _loop;

    test_server _server(_loop, _opts);
    assert(_server.run());
/*
    timer::timerid _client_timer =
        _loop.run_after(1000,
            [&] (const timer::timer_ptr &_timer)
    {
        try {
            k::socket _sock(k::socket::IPV4_TCP);
            _sock.connect(address(SERVER_ADDR, SERVER_PORT, false));

            int _times = 0;
            int _size  = 0;
            {
                char _arr[8];
                {
                    in_buffer _in_buf(_arr, 8);
                    _sock.read(_in_buf);
                    out_buffer(_arr, 8).read_int32(_times).read_int32(_size);
                    log_info("client: times = %d, size = %d", _times, _size);
                }
                {
                    out_buffer _out_buf(_arr, 8);
                    _sock.write(_out_buf);
                }
            }

            for (int _i = 0; _i < _times; ++_i) {
                char _arr[_size + 1]; // XXX: unsafe
                _arr[_size] = 0;
                {
                    in_buffer _in_buf(_arr, _size);
                    _sock.read(_in_buf);
                }
                {
                    out_buffer(_arr, _size).read_bytes(_arr, _size);
                    log_info("client: %s", _arr);
                }
                {
                    for (int _j = 0; _j < _size / 2; ++_j)
                        std::swap(_arr[_j], _arr[_size - _j - 1]);
                }
                {
                    out_buffer _out_buf(_arr, _size);
                    _sock.write(_out_buf);
                }
            }
            _sock.close();
        } catch (...) {
            log_error("client error");
            _loop.cancel(_timer);
        }
    });
*/

    _loop.run_after(60000,
        [&] (const timer::timer_ptr _timer)
    {
        _server.stop();

        log_warning("main thread is closing...3s");
        timestamp _current_time = timestamp::monotonic();
        _loop.run_at(_current_time + timestamp(1000),
                     [&] (const timer::timer_ptr &_timer) {
             log_warning("main thread is closing...2s");
        });
        _loop.run_at(_current_time + timestamp(2000),
                     [&] (const timer::timer_ptr &_timer) {
             log_warning("main thread is closing...1s");
        });
        _loop.run_at(_current_time + timestamp(3000),
                     [&] (const timer::timer_ptr &_timer) {
             //_loop.cancel(_client_timer);
             _loop.stop();
        });
    });

    _loop.run();
    _server.stop();
}

#warning "signal event"
#warning "log回调,type+str+size"
#warning "map改鏈表+指针"
