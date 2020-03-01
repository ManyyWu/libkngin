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
    test_server (event_loop &loop, const server_opts &opts)
        : m_loop(&loop),
          m_server(loop, opts),
          m_sessions(),
          m_sessions_mutex()
    {
    }

    bool
    run ()
    {
        m_server.set_session_handler([this] (server::session_ptr session) {
            if (session->closed())
                return;
            log_info("new session from %s", session->name().c_str());

            g_ts = timestamp::realtime();
            // create session info
            {
                local_lock lock(m_sessions_mutex);
                if (m_sessions.find(session->key()) != m_sessions.end()) {
                    log_warning("existed session %s", session->name().c_str());
                    session->close();
                    return;
                }
                m_sessions.insert(std::make_pair(session->key(), session_info(session)));
                //log_debug("size: %d", m_sessions.size());
            }

#define CLOSE_COND 0
#if (true == !!CLOSE_COND)
            if (session->peer_addr().port() > 50000) {
                auto loop = m_loop.lock();
                assert(loop);
                loop->run_in_loop([this, loop] () {
                    stop();
                    loop->stop();
                });
                return;
            }
#endif
            // process
            process(session);
        }); // end of new_session_handler, run in listner thread

        m_server.set_error_handler([this] (tcp::session &session, error_code ec) {
            log_info("session %s error - %s",
                     session.name().c_str(), system_error_str(ec).c_str());
            {
                local_lock lock(m_sessions_mutex);
                assert(m_sessions.find(session.key()) != m_sessions.end());
                m_sessions.erase(session.key());
                size_t size = m_sessions.size();
                log_debug("size: %" PRIu64, size);
            }
            session.close();
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
            local_lock lock(m_sessions_mutex);
            for (auto &iter : m_sessions) {
                if (!iter.second.si_session->closed())
                    iter.second.si_session->close(true);
            }
            m_sessions.clear();
        }
    }

    void
    process (server::session_ptr s)
    {
        uint8_arr_ptr msg_arr = k::make_shared_array<char>(8);
        in_buffer(msg_arr.get(), 8).write_int32(times).write_int32(g_data_size);
        s->send( // send times and buf size
            msg_buffer(msg_arr, 0, 8),
            [] (session &s)
        {
            uint8_arr_ptr arr = k::make_shared_array<char>(8);
            s.recv( // recv ack
                in_buffer(arr.get(), 8),
                [arr] (session &s, in_buffer buf, size_t size)
            {
                if_not (size == 8)
                    return;
                if (out_buffer(buf.begin(), 4).peek_int32() != times or
                    out_buffer(buf.begin() + 4, 4).peek_int32() != g_data_size) {
                    log_error("ack error");
                    s.close();
                    return;
                }
                for (int i = 0; i < times; i++) {
/*                    uint8_arr_ptr msg_arr = k::make_shared_array<char>(g_data_size);
                    in_buffer(msg_arr.get(), g_data_size).write_bytes(g_data, g_data_size);
                    s.send( // send data
                        msg_buffer(msg_arr, 0, g_data_size),
                        [] (session &s)
                    {*/
                        uint8_arr_ptr arr1 = k::make_shared_array<char>(g_data_size);
                        s.recv( // recv reverse data
                            in_buffer(arr1.get(), g_data_size),
                            [arr1] (session &s, in_buffer buf, size_t size)
                        {
                            if (size < g_data_size)
                                log_debug("first");
                            else
                                g_total_size += size;
                            //log_info("recv data %s from %s", out_buffer(buf.begin(), buf.size()).dump().c_str(), s.name().c_str());
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

        session_info (session::session_ptr &session)
            : si_session(session) {}
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
    tcp::server_opts opts = {
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
    event_loop loop;

    test_server server(loop, opts);
    assert(server.run());
/*
    timer::timerid client_timer =
        loop.run_after(1000,
            [&] (const timer::timer_ptr &timer)
    {
        try {
            k::socket sock(k::socket::IPV4_TCP);
            sock.connect(address(SERVER_ADDR, SERVER_PORT, false));

            int times = 0;
            int size  = 0;
            {
                char arr[8];
                {
                    in_buffer in_buf(arr, 8);
                    sock.read(in_buf);
                    out_buffer(arr, 8).read_int32(times).read_int32(size);
                    log_info("client: times = %d, size = %d", times, size);
                }
                {
                    out_buffer out_buf(arr, 8);
                    sock.write(out_buf);
                }
            }

            for (int i = 0; i < times; ++i) {
                char arr[size + 1]; // XXX: unsafe
                arr[size] = 0;
                {
                    in_buffer in_buf(arr, size);
                    sock.read(in_buf);
                }
                {
                    out_buffer(arr, size).read_bytes(arr, size);
                    log_info("client: %s", arr);
                }
                {
                    for (int j = 0; j < size / 2; ++j)
                        std::swap(arr[j], arr[size - j - 1]);
                }
                {
                    out_buffer out_buf(arr, size);
                    sock.write(out_buf);
                }
            }
            sock.close();
        } catch (...) {
            log_error("client error");
            loop.cancel(timer);
        }
    });
*/

    loop.run_after(6000,
        [&] (const timer::timer_ptr timer)
    {
        server.stop();

        log_warning("main thread is closing...3s");
        timestamp current_time = timestamp::realtime();
        loop.run_at(current_time + timestamp(1000),
                     [&] (const timer::timer_ptr &timer) {
             log_warning("main thread is closing...2s");
        });
        loop.run_at(current_time + timestamp(2000),
                     [&] (const timer::timer_ptr &timer) {
             log_warning("main thread is closing...1s");
        });
        loop.run_at(current_time + timestamp(3000),
                     [&] (const timer::timer_ptr &timer) {
             //loop.cancel(client_timer);
             loop.stop();
        });
    });

    loop.run();
    server.stop();
}
