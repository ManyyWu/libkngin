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
    k::socket      _s(socket::IPV6_TCP);

    // print flags
//    cerr << "SO_BROADCAST: " << sockopts::broadcast(_s) << endl;
    cerr << "SO_DEBUG    : " << sockopts::debug(_s) << endl;
    cerr << "SO_DONTROUTE: " << sockopts::nonroute(_s) << endl;
    cerr << "SO_ERROR    : " << sockopts::error(_s).value() << endl;
    cerr << "SO_KEEPALIVE: " << sockopts::keepalive(_s) << endl;
    linger _l = sockopts::linger(_s);
    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
    cerr << "SO_OOBINLINE: " << sockopts::oobinline(_s) << endl;
    cerr << "SO_RCVBUF   : " << sockopts::rcvbuf(_s) << endl;
    cerr << "SO_SNDBUF   : " << sockopts::sndbuf(_s) << endl;
//    cerr << "SO_RCVLOWAT : " << sockopts::rcvlowat(_s) << endl;
//    cerr << "SO_SNDLOWAT : " << sockopts::sndlowat(_s) << endl;
    timeval _t = sockopts::rcvtimeo(_s);
    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    _t = sockopts::sndtimeo(_s);
    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    cerr << "SO_REUSEADDR: " << sockopts::reuseaddr(_s) << endl;
//    cerr << "SO_REUSEPORT: " << sockopts::reuseport(_s) << endl;
    cerr << "SO_TYPE     : " << sockopts::type(_s) << endl;
    cerr << "IP_TOS      : " << sockopts::ip_tos(_s) << endl;
    cerr << "IP_TTL      : " << sockopts::ip_ttl(_s) << endl;
    cerr << "SO_V6ONLY   : " << sockopts::ipv4_disabled(_s) << endl;
//    cerr << "SO_MAXSEG   : " << sockopts::maxseg(_s) << endl;
    cerr << "SO_NODELAY  : " << sockopts::nodelay(_s) << endl;
    cerr << endl;
}
