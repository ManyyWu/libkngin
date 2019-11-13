#include <string>
#include "tcp_server.h"
#include "thread.h"
#include "common.h"

using namespace k;

void
tcp_server_test ()
{
    tcp_server_opts _opts = {
        std::string("127.0.0.1"),
        20000,
        false,
        10,
        5
    };

    tcp_server _s(_opts);
    check(_s.run());
    thread::sleep(1000);
}