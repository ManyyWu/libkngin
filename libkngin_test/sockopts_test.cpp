#include <iostream>
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/socket.h"

using namespace std;
using namespace k;

void
sockopts_test ()
{
    k::socket s(socket::IPV6_TCP);

    // print flags
    cerr << "SO_BROADCAST: " << sockopts(s.fd()).broadcast().val << endl;
    cerr << "SO_DEBUG    : " << sockopts(s.fd()).debug().val << endl;
    cerr << "SO_DONTROUTE: " << sockopts(s.fd()).nonroute().val << endl;
    cerr << "SO_ERROR    : " << sockopts(s.fd()).error().val << endl;
    cerr << "SO_KEEPALIVE: " << sockopts(s.fd()).keepalive().val << endl;
    linger _l;
    sockopts(s.fd()).linger(_l);
    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
    cerr << "SO_OOBINLINE: " << sockopts(s.fd()).oobinline().val << endl;
    cerr << "SO_RCVBUF   : " << sockopts(s.fd()).rcvbuf().val << endl;
    cerr << "SO_SNDBUF   : " << sockopts(s.fd()).sndbuf().val << endl;
    cerr << "SO_RCVLOWAT : " << sockopts(s.fd()).rcvlowat().val << endl;
    cerr << "SO_SNDLOWAT : " << sockopts(s.fd()).sndlowat().val << endl;
    timeval _t;
    sockopts(s.fd()).rcvtimeo(_t);
    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    sockopts(s.fd()).sndtimeo(_t);
    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    cerr << "SO_REUSEADDR: " << sockopts(s.fd()).reuseaddr().val << endl;
    cerr << "SO_REUSEPORT: " << sockopts(s.fd()).reuseport().val << endl;
    cerr << "SO_TYPE     : " << sockopts(s.fd()).type().val << endl;
    cerr << "IP_TOS      : " << sockopts(s.fd()).ip_tos().val << endl;
    cerr << "IP_TTL      : " << sockopts(s.fd()).ip_ttl().val << endl;
//    cerr << "SO_V6ONLY   : " << sockopts(s.fd()).ipv4_disabled().val << endl;
    cerr << "SO_MAXSEG   : " << sockopts(s.fd()).maxseg().val << endl;
    cerr << "SO_NODELAY  : " << sockopts(s.fd()).nodelay().val << endl;
    cerr << endl;

    // set flags
    cerr << "SO_BROADCAST: " << sockopts(s.fd()).set_broadcast(true).code << endl;
//    cerr << "SO_DEBUG    : " << sockopts(s.fd()).set_debug(true).code << endl;
    cerr << "SO_DONTROUTE: " << sockopts(s.fd()).set_nonroute(true).code << endl;
    cerr << "SO_KEEPALIVE: " << sockopts(s.fd()).set_keepalive(true).code << endl;
    sockopts(s.fd()).set_linger(1, 0);
    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
    cerr << "SO_RCVBUF   : " << sockopts(s.fd()).set_rcvbuf(61440).code << endl;
    cerr << "SO_SNDBUF   : " << sockopts(s.fd()).set_sndbuf(61440).code << endl;
//    cerr << "SO_RCVLOWAT : " << sockopts(s.fd()).set_rcvlowat(1).code << endl;
//    cerr << "SO_SNDLOWAT : " << sockopts(s.fd()).set_sndlowat(1).code << endl;
    sockopts(s.fd()).set_rcvtimeo({100, 0});
    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    sockopts(s.fd()).set_sndtimeo({100, 0});
    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    cerr << "SO_REUSEADDR: " << sockopts(s.fd()).set_reuseaddr(true).code << endl;
    cerr << "SO_REUSEPORT: " << sockopts(s.fd()).set_reuseport(true).code << endl;
    cerr << "IP_TOS      : " << sockopts(s.fd()).set_ip_tos(0).code << endl;
    cerr << "IP_TTL      : " << sockopts(s.fd()).set_ip_ttl(1).code << endl;
    cerr << "SO_V6ONLY   : " << sockopts(s.fd()).set_ipv6_only(true).code << endl;
    cerr << "SO_MAXSEG   : " << sockopts(s.fd()).set_maxseg(536).code << endl;
    cerr << "SO_NODELAY  : " << sockopts(s.fd()).set_nodelay(true).code << endl;

    // print flags
    cerr << "SO_BROADCAST: " << sockopts(s.fd()).broadcast().val << endl;
    cerr << "SO_DEBUG    : " << sockopts(s.fd()).debug().val << endl;
    cerr << "SO_DONTROUTE: " << sockopts(s.fd()).nonroute().val << endl;
    cerr << "SO_ERROR    : " << sockopts(s.fd()).error().val << endl;
    cerr << "SO_KEEPALIVE: " << sockopts(s.fd()).keepalive().val << endl;
    sockopts(s.fd()).linger(_l);
    cerr << "SO_LINGER   : " << _l.l_onoff << ", " << _l.l_linger << endl;
    cerr << "SO_OOBINLINE: " << sockopts(s.fd()).oobinline().val << endl;
    cerr << "SO_RCVBUF   : " << sockopts(s.fd()).rcvbuf().val << endl;
    cerr << "SO_SNDBUF   : " << sockopts(s.fd()).sndbuf().val << endl;
    cerr << "SO_RCVLOWAT : " << sockopts(s.fd()).rcvlowat().val << endl;
    cerr << "SO_SNDLOWAT : " << sockopts(s.fd()).sndlowat().val << endl;
    sockopts(s.fd()).rcvtimeo(_t);
    cerr << "SO_RCVTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    sockopts(s.fd()).sndtimeo(_t);
    cerr << "SO_SNDTIMEO : " << _t.tv_sec << ", " << _t.tv_usec << endl;
    cerr << "SO_REUSEADDR: " << sockopts(s.fd()).reuseaddr().val << endl;
    cerr << "SO_REUSEPORT: " << sockopts(s.fd()).reuseport().val << endl;
    cerr << "SO_TYPE     : " << sockopts(s.fd()).type().val << endl;
    cerr << "IP_TOS      : " << sockopts(s.fd()).ip_tos().val << endl;
    cerr << "IP_TTL      : " << sockopts(s.fd()).ip_ttl().val << endl;
    cerr << "SO_V6ONLY   : " << sockopts(s.fd()).ipv4_disabled().val << endl;
    cerr << "SO_MAXSEG   : " << sockopts(s.fd()).maxseg().val << endl;
    cerr << "SO_NODELAY  : " << sockopts(s.fd()).nodelay().val << endl;

}
