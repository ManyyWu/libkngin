#include "kngin/net/address.h"
#include <iostream>
#if defined(KNGIN_SYSTEM_WIN32)
#include <ws2ipdef.h>
#else
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

using namespace std;

int
main () {
  {
    k::address a = k::address("192.168.0.2", 10000, false);
    cerr << a.ip_address().c_str() << ":" << a.port() << endl;
    cerr << "ipv6:" << a.ipv6() << "," << "ipv4:" << a.ipv4() << endl;
    cerr << "size:" << a.size() << endl;
    cerr << "ipv4 mapped:" << a.is_ipv4_mapped() << endl;
    cerr << endl;

    ::sockaddr_in sa;
    ::memset(&sa, 0, sizeof(::sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(10000);
    sa.sin_addr.s_addr = ::inet_addr("192.168.0.2");
    k::address a1 = sa;
    cerr << a1.ip_address().c_str() << ":" << a1.port() << endl;
    cerr << "ipv6:" << a1.ipv6() << "," << "ipv4:" << a1.ipv4() << endl;
    cerr << "size:" << a1.size() << endl;
    cerr << "ipv4 mapped:" << a1.is_ipv4_mapped() << endl;
    cerr << endl;

    cerr << "equal:" << (a == a1) << endl;
    cerr << "not equal:" << (a != a1) << endl;
    cerr << endl;
  }

  {
    k::address a = k::address("fe80::1b28:e1c2:1ce8:3672", 10000, true);
    cerr << a.ip_address().c_str() << ":" << a.port() << endl;
    cerr << "ipv6:" << a.ipv6() << "," << "ipv4:" << a.ipv4() << endl;
    cerr << "size:" << a.size() << endl;
    cerr << "ipv4 mapped:" << a.is_ipv4_mapped() << endl;
    cerr << endl;
  }

  {
    k::address a = k::address("::1", 10000, true);
    cerr << a.ip_address().c_str() << ":" << a.port() << endl;
    cerr << "ipv6:" << a.ipv6() << "," << "ipv4:" << a.ipv4() << endl;
    cerr << "size:" << a.size() << endl;
    cerr << "ipv4 mapped:" << a.is_ipv4_mapped() << endl;
    cerr << endl;
  }

  {
    k::address a = k::address("::ffff:192.168.0.2", 10000, true);
    cerr << a.ip_address().c_str() << ":" << a.port() << endl;
    cerr << "ipv6:" << a.ipv6() << "," << "ipv4:" << a.ipv4() << endl;
    cerr << "size:" << a.size() << endl;
    cerr << "ipv4 mapped:" << a.is_ipv4_mapped() << endl;
    cerr << endl;
  }
  {
    cerr << "valid ipv4 address:" << k::address::is_valid_ipv4_string("192.168.0.2") << endl;
    cerr << "valid ipv4 address:" << k::address::is_valid_ipv4_string("192.168.0.0") << endl;
    cerr << "valid ipv4 address:" << k::address::is_valid_ipv4_string("0.0.0.0") << endl;
    cerr << "valid ipv4 address:" << k::address::is_valid_ipv4_string("::ffff:0.0.0.0") << endl;
    cerr << "valid ipv6 address:" << k::address::is_valid_ipv6_string("0.0.0.0") << endl;
    cerr << "valid ipv6 address:" << k::address::is_valid_ipv6_string("::ffff:0.0.0.0") << endl;
    cerr << "valid ipv6 address:" << k::address::is_valid_ipv6_string("::1") << endl;
  }

  return 0;
}
