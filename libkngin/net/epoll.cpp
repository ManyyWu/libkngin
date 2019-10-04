#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include "define.h"
#include "common.h"
#include "exception.h"
#include "logfile.h"
#include "epoll.h"

__NAMESPACE_BEGIN

epoll::epoll (event_loop *_loop)
    try
    : m_loop(_loop),
      m_events(RESERVE_EPOLL_EVENT),
      m_epollfd(::epoll_create1(EPOLL_CLOEXEC))
{
    kassert(_loop);
    if (m_epollfd < 0) {
        log_fatal("epoll_create1() error [%s:%d]", m_epollfd, errno, strerror(errno));
        throw exception("epoll::epoll() error");
    }
} catch (...) {
    if (m_epollfd >= 0)
        ::close(m_epollfd);
    throw;
}

epoll::~epoll ()
{
     this->close();
}



void
epoll::close ()
{
    if (m_epollfd >= 0) {
        ::close(m_epollfd);
        m_epollfd = -1;
    }
}

__NAMESPACE_END
