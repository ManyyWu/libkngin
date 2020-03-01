#include <iostream>
#include "../libkngin/net/sockopts.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/sockopts_test.cpp"

using namespace std;
using namespace k;

void
sockopts_test ()
{
    k::socket      s(socket::IPV6_TCP);

    // print flags
//    cerr << "SO_BROADCAST: " << sockopts::broadcast(s) << endl;
    cerr << "SO_DEBUG    : " << sockopts::debug(s) << endl;
    cerr << "SO_DONTROUTE: " << sockopts::nonroute(s) << endl;
    cerr << "SO_ERROR    : " << sockopts::error(s).value() << endl;
    cerr << "SO_KEEPALIVE: " << sockopts::keepalive(s) << endl;
    linger l = sockopts::linger(s);
    cerr << "SO_LINGER   : " << l.l_onoff << ", " << l.l_linger << endl;
    cerr << "SO_OOBINLINE: " << sockopts::oobinline(s) << endl;
    cerr << "SO_RCVBUF   : " << sockopts::rcvbuf(s) << endl;
    cerr << "SO_SNDBUF   : " << sockopts::sndbuf(s) << endl;
//    cerr << "SO_RCVLOWAT : " << sockopts::rcvlowat(s) << endl;
//    cerr << "SO_SNDLOWAT : " << sockopts::sndlowat(s) << endl;
    timeval t = sockopts::rcvtimeo(s);
    cerr << "SO_RCVTIMEO : " << t.tv_sec << ", " << t.tv_usec << endl;
    t = sockopts::sndtimeo(s);
    cerr << "SO_SNDTIMEO : " << t.tv_sec << ", " << t.tv_usec << endl;
    cerr << "SO_REUSEADDR: " << sockopts::reuseaddr(s) << endl;
//    cerr << "SO_REUSEPORT: " << sockopts::reuseport(s) << endl;
    cerr << "SO_TYPE     : " << sockopts::type(s) << endl;
    cerr << "IP_TOS      : " << sockopts::ip_tos(s) << endl;
    cerr << "IP_TTL      : " << sockopts::ip_ttl(s) << endl;
    cerr << "SO_V6ONLY   : " << sockopts::ipv4_disabled(s) << endl;
//    cerr << "SO_MAXSEG   : " << sockopts::maxseg(s) << endl;
    cerr << "SO_NODELAY  : " << sockopts::nodelay(s) << endl;
    cerr << endl;
}
