#include <netinet/in.h>
#include "define.h"
#include "exception.h"
#include "address.h"
#include "common.h"

__NAMESPACE_BEGIN

bool
address::addr (inet6_addrstr &_s) const
{
}

bool
address::inet6 () const
{
    struct sockaddr *_addr = (sockaddr *)&(m_sa.sa_in6);
    if (AF_INET == _addr->sa_family)
        return false;
    else if (AF_INET6 == _addr->sa_family)
        return true;
    else
        throw exception("invalid address family");
}

__NAMESPACE_END
