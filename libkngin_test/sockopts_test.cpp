#include <iostream>
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/socket.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/sockopts_test.cpp"

using namespace std;
using namespace k;

void
sockopts_test ()
{
//    k::socket _s(socket::IPV6_TCP);
//    bool           i_val;
//    int            l_val;
//    struct linger  linger_val;
//    struct timeval timeval_val;
//
//    // print flags
//    cerr << "SO_BROADCAST: " << sockopts::broadcast(_s, i_val), i_val << endl;
//    cerr << "SO_DEBUG    : " << sockopts::debug(_s).val << endl;
//    cerr << "SO_DONTROUTE: " << sockopts::nonroute(_s).val << endl;
//    cerr << "SO_ERROR    : " << sockopts::error(_s).val << endl;
//    cerr << "SO_KEEPALIVE: " << sockopts::keepalive(_s).val << endl;
//    linger _l;
//    sockopts::linger(_s, _l);
//    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
//    cerr << "SO_OOBINLINE: " << sockopts::oobinline(_s).val << endl;
//    cerr << "SO_RCVBUF   : " << sockopts::rcvbuf(_s).val << endl;
//    cerr << "SO_SNDBUF   : " << sockopts::sndbuf(_s).val << endl;
//    cerr << "SO_RCVLOWAT : " << sockopts::rcvlowat(_s).val << endl;
//    cerr << "SO_SNDLOWAT : " << sockopts::sndlowat(_s).val << endl;
//    timeval _t;
//    sockopts::rcvtimeo(_s, _t);
//    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    sockopts::sndtimeo(_s, _t);
//    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    cerr << "SO_REUSEADDR: " << sockopts::reuseaddr(_s).val << endl;
//    cerr << "SO_REUSEPORT: " << sockopts::reuseport(_s).val << endl;
//    cerr << "SO_TYPE     : " << sockopts::type(_s).val << endl;
//    cerr << "IP_TOS      : " << sockopts::ip_tos(_s).val << endl;
//    cerr << "IP_TTL      : " << sockopts::ip_ttl(_s).val << endl;
////    cerr << "SO_V6ONLY   : " << sockopts::ipv4_disabled(_s).val << endl;
//    cerr << "SO_MAXSEG   : " << sockopts::maxseg(_s).val << endl;
//    cerr << "SO_NODELAY  : " << sockopts::nodelay(_s).val << endl;
//    cerr << endl;
//
//    // set flags
//    cerr << "SO_BROADCAST: " << sockopts::set_broadcast(_s, true).code << endl;
////    cerr << "SO_DEBUG    : " << sockopts::set_debug(_s, true).code << endl;
//    cerr << "SO_DONTROUTE: " << sockopts::set_nonroute(_s, true).code << endl;
//    cerr << "SO_KEEPALIVE: " << sockopts::set_keepalive(_s, true).code << endl;
//    sockopts::set_linger(_s, 1, 0);
//    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
//    cerr << "SO_RCVBUF   : " << sockopts::set_rcvbuf(_s, 61440).code << endl;
//    cerr << "SO_SNDBUF   : " << sockopts::set_sndbuf(_s, 61440).code << endl;
////    cerr << "SO_RCVLOWAT : " << sockopts::set_rcvlowat(_s, 1).code << endl;
////    cerr << "SO_SNDLOWAT : " << sockopts::set_sndlowat(_s, 1).code << endl;
//    sockopts::set_rcvtimeo(_s, {100, 0});
//    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    sockopts::set_sndtimeo(_s, {100, 0});
//    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    cerr << "SO_REUSEADDR: " << sockopts::set_reuseaddr(_s, true).code << endl;
//    cerr << "SO_REUSEPORT: " << sockopts::set_reuseport(_s, true).code << endl;
//    cerr << "IP_TOS      : " << sockopts::set_ip_tos(_s, 0).code << endl;
//    cerr << "IP_TTL      : " << sockopts::set_ip_ttl(_s, 1).code << endl;
//    cerr << "SO_V6ONLY   : " << sockopts::set_ipv6_only(_s, true).code << endl;
//    cerr << "SO_MAXSEG   : " << sockopts::set_maxseg(_s, 536).code << endl;
//    cerr << "SO_NODELAY  : " << sockopts::set_nodelay(_s, true).code << endl;
//
//    // print flags
//    cerr << "SO_BROADCAST: " << sockopts::broadcast(_s).val << endl;
//    cerr << "SO_DEBUG    : " << sockopts::debug(_s).val << endl;
//    cerr << "SO_DONTROUTE: " << sockopts::nonroute(_s).val << endl;
//    cerr << "SO_ERROR    : " << sockopts::error(_s).val << endl;
//    cerr << "SO_KEEPALIVE: " << sockopts::keepalive(_s).val << endl;
//    sockopts::linger(_s, _l);
//    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
//    cerr << "SO_OOBINLINE: " << sockopts::oobinline(_s).val << endl;
//    cerr << "SO_RCVBUF   : " << sockopts::rcvbuf(_s).val << endl;
//    cerr << "SO_SNDBUF   : " << sockopts::sndbuf(_s).val << endl;
//    cerr << "SO_RCVLOWAT : " << sockopts::rcvlowat(_s).val << endl;
//    cerr << "SO_SNDLOWAT : " << sockopts::sndlowat(_s).val << endl;
//    sockopts::rcvtimeo(_s, _t);
//    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    sockopts::sndtimeo(_s, _t);
//    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
//    cerr << "SO_REUSEADDR: " << sockopts::reuseaddr(_s).val << endl;
//    cerr << "SO_REUSEPORT: " << sockopts::reuseport(_s).val << endl;
//    cerr << "SO_TYPE     : " << sockopts::type(_s).val << endl;
//    cerr << "IP_TOS      : " << sockopts::ip_tos(_s).val << endl;
//    cerr << "IP_TTL      : " << sockopts::ip_ttl(_s).val << endl;
//    cerr << "SO_V6ONLY   : " << sockopts::ipv4_disabled(_s).val << endl;
//    cerr << "SO_MAXSEG   : " << sockopts::maxseg(_s).val << endl;
//    cerr << "SO_NODELAY  : " << sockopts::nodelay(_s).val << endl;

}
