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

class simple_ftp_server {
public:
    test_server (const server_opts &_opts)
        : m_server(_opts),
          m_bufs(),
          m_mutex(),
          m_savetime(time(NULL)),
          m_times(0)
    {
    }

    bool
    run ()
    {
    }

    void
    stop ()
    {
        m_server.stop();
    }

    void
    on_message (tcp::session &_session, in_buffer &_buf, size_t _size)
    {
    }

    void
    on_sent (tcp::session &_session)
    {
    }

    void
    on_close (const tcp::session &_session)
    {
    }

    void
    on_new_session (tcp::server::session_ptr _session)
    {
    }

    void
    on_oob (tcp::session &_session, uint8_t _data)
    {
    }

protected:
    tcp::server            m_server;

    typedef std::unordered_map<uint64_t, std::shared_ptr<char *>> buffer_map;
    buffer_map             m_bufs;
    mutex                  m_bufs_mutex;

    time_t                 m_savetime;
    std::atomic<uint64_t>  m_times;
};

void
simple_ftp_server_test ()
{

//#define SERVER_ADDR "192.168.0.2"
//#define SERVER_ADDR "127.0.0.1"
#define SERVER_ADDR "fe80::f87f:8669:d667:1316%16"
#define SERVER_PORT 20000

    g_barrier = std::make_shared<barrier>(2);
    tcp::server_opts _opts = {
        .name                   = SERVER_ADDR,
        .port                   = SERVER_PORT,
        .allow_ipv6             = false,
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