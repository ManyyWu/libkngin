#include <atomic>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <array>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/eventfd.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/bits.h"
#include "../libkngin/core/error.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"

using namespace k;
using namespace std;
using namespace std::placeholders;

void
test ()
{
    bool _ok = true;
    inet_addrstr _addr_str = {"192.168.0.2"};
    uint16_t     _port = 20000;

    address _server_addr;
    assert(address::str2sockaddr(_addr_str, _port,_server_addr));

    k::socket _server_sock(socket::IPV4_TCP);
    if (_server_sock.bind(_server_addr) < 0)
        cerr << strerror(errno) << endl;
    if (_server_sock.listen(5) < 0)
        cerr << strerror(errno) << endl;
    cerr << "listening..." << endl;
    while (_ok) {
        address _client_addr;
        k::socket _client_sock(_server_sock.accept(_client_addr));
        inet_addrstr _client_addr_str;
        cerr << "connect to: " << _client_addr.addrstr(_client_addr_str)
             << ", " << _client_addr.port() << endl;
        _client_sock.close();
    }
    _server_sock.close();
}
