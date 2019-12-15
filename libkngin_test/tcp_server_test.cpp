#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <unordered_map>
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

static std::shared_ptr<barrier> g_barrier = nullptr;

const char *g_data = "01234567889abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.";
const int   g_data_size = 64;
const int   times = 30;

class test_server {
public:
    test_server (const server_opts &_opts)
        : m_server(_opts),
          m_bufs(),
          m_bufs_mutex(),
          m_savetime(time(NULL)),
          m_times(0)
    {
    }

    bool
    run ()
    {
        m_server.set_session_handler(std::bind(&test_server::on_new_session, this,
                                               std::placeholders::_1));
        m_server.set_close_handler(std::bind(&test_server::on_close, this,
                                             std::placeholders::_1));
        m_server.set_crash_handler([] () {
            assert(!"server crashed");
            exit(1);
        });
        return m_server.run();
    }

    void
    stop ()
    {
        m_server.stop();
    }

    void
    on_close (const tcp::session &_session)
    {
        log_info("session %s closed", _session.name().c_str());
        {
            local_lock _lock(m_bufs_mutex);
            assert(m_bufs.find(_session.key()) != m_bufs.end());
            m_bufs.erase(_session.key());
            //log_debug("size: %d", m_bufs.size());
        }
    }

    void
    on_new_session (server::session_ptr _session)
    {
        assert(_session);
        log_info("new session from %s", _session->name().c_str());

        // create session info
        {
            local_lock _lock(m_bufs_mutex);
            assert(m_bufs.find(_session->key()) == m_bufs.end());
            m_bufs[_session->key()] = {
                .si_session = _session
            };
            //log_debug("size: %d", m_bufs.size());
        }

        // process
        process(_session);
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
            session::in_buffer_ptr _in_buf = std::make_shared<in_buffer>(_arr->data(), 8);
            _s.recv( // recv ack
                _in_buf, [_arr] (session &_s, in_buffer &_buf, size_t _size)
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
                        session::in_buffer_ptr _in_buf = std::make_shared<in_buffer>(_arr1->data(), _arr1->size());
                        _s.recv( // recv reverse data
                            _in_buf, [_arr1] (session &_s, in_buffer &_buf, size_t _size)
                        {
                            //log_info("recv data %s from %s",
                            //         out_buffer(_buf.begin(), _buf.size()).dump().c_str(),
                            //         _s.name().c_str());
                        });
                    });
                }
            });
        });
    }

protected:
    tcp::server m_server;

    struct session_info {
        session::session_ptr si_session;
    };
    typedef std::unordered_map<std::string, session_info> buffer_map;
    buffer_map             m_bufs;
    mutex                  m_bufs_mutex;

    time_t                 m_savetime;
    std::atomic<uint64_t>  m_times;
};

void
tcp_server_test ()
{
#define SERVER_ADDR "192.168.0.2"
//#define SERVER_ADDR "127.0.0.1"
//#define SERVER_ADDR "fe80::26e4:35c1:eea7:68a2%eno1"
//#define SERVER_ADDR "::1%16"
#define SERVER_PORT 20000
    g_barrier = std::make_shared<barrier>(2);
    tcp::server_opts _opts = {
            .name                   = SERVER_ADDR,
            .port                   = SERVER_PORT,
            .allow_ipv6             = false,
            .backlog                = 10000,
            .thread_num             = 100,
            .disable_debug          = false,
            .disable_info           = false,
            .separate_listen_thread = true,
    };
    test_server _s(_opts);
    assert(_s.run());

    if (g_barrier->wait())
        g_barrier->destroy();
    _s.stop();
}

// server session 要求
// 基本操作线程安全
// closing,closed状态下操作安全
// 缓冲区安全
// 效率
