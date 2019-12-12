#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>
#include <memory>
#include <ctime>
#include "../libkngin/core/thread.h"
#include "../libkngin/core/common.h"
#include "../libkngin/core/system_error.h"
#include "../libkngin/net/tcp/server.h"

using namespace k;
using namespace k::tcp;

static std::shared_ptr<barrier> g_barrier = nullptr;

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
        m_server.set_message_handler(std::bind(&test_server::on_message, this,
                                          std::placeholders::_1,
                                          std::placeholders::_2,
                                          std::placeholders::_3));
        m_server.set_sent_handler(std::bind(&test_server::on_sent, this,
                                  std::placeholders::_1));
        m_server.set_session_handler(std::bind(&test_server::on_new_session, this,
                                     std::placeholders::_1));
        m_server.set_oob_handler(std::bind(&test_server::on_oob, this,
                                 std::placeholders::_1, std::placeholders::_2));
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
    on_message (tcp::session &_session, in_buffer &_buf, size_t _size)
    {
        log_info("readed %d bytes from session %s, data: %s",
                 _size, _session.name().c_str(),
                 _buf.dump().c_str());
        in_buffer(_buf.begin(), 4).write_uint32(_session.peer_addr().port());
        _session.send(std::make_shared<out_buffer>(_buf.begin(), 4));
    }

    void
    on_sent (tcp::session &_session)
    {
        log_info("session %s written done", _session.name().c_str());

        std::shared_ptr<in_buffer> _buf = nullptr;
        {
            local_lock _lock(m_bufs_mutex);
            _buf = std::make_shared<in_buffer>(m_bufs[_session.key()].si_buf.get(), 4);
        }
        _session.recv(_buf);
    }

    void
    on_close (const tcp::session &_session)
    {
        log_info("session %s closed", _session.name().c_str());

        {
            local_lock _lock(m_bufs_mutex);
            m_bufs.erase(_session.key());
        }
    }

    void
    on_new_session (tcp::server::session_ptr _session)
    {
        assert(_session);
        log_info("new session %s", _session->name().c_str());
        std::shared_ptr<out_buffer> _buf = nullptr;
        std::shared_ptr<char *> _arr = nullptr;
        {
            local_lock _lock(m_bufs_mutex);
            m_bufs[_session->key()] = {_session, std::make_shared<char *>(new char[4])};
            _arr = m_bufs[_session->key()].si_buf;
            _buf = std::make_shared<out_buffer>(_arr.get(), 4);
        }

        {
            if (_session->peer_addr().port() >= 147000) { // close
                _session->close(true);
                if (g_barrier->wait())
                    g_barrier->destroy();
                return;
            }
        }

        in_buffer(_arr.get(), 4).write_uint32(_session->peer_addr().port());
        _session->send(_buf);

        if (m_savetime == time(NULL)) {
            m_times++;
        } else {
            log_warning("accepted %" PRIu64 " new sessions in last second, "
                        "current session num = %" PRIu64,
                        m_times.load(), m_bufs.size());
            m_savetime = time(NULL);
            m_times = 0;
        }
    }

    void
    on_oob (tcp::session &_session, uint8_t _data)
    {
    }

protected:
    tcp::server            m_server;

    struct session_info {
        session::session_ptr    si_session;
        std::shared_ptr<char *> si_buf;
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
//#define SERVER_ADDR "192.168.0.2"
//#define SERVER_ADDR "127.0.0.1"
#define SERVER_ADDR "fe80::26e4:35c1:eea7:68a2%eno1"
//#define SERVER_ADDR "::1%16"
#define SERVER_PORT 20000
    g_barrier = std::make_shared<barrier>(2);
    tcp::server_opts _opts = {
        .name                   = SERVER_ADDR,
        .port                   = SERVER_PORT,
        .allow_ipv6             = true,
        .backlog                = 100,
        .thread_num             = 3,
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