#ifndef KNGIN_EPOLL_REACTOR_H
#define KNGIN_EPOLL_REACTOR_H

#include "kngin/core/define.h"
#include "kngin/core/base/mutex.h"
#include "detail/core/base/list.h"
#include "detail/core/event/impl/epoll_event.h"

KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN

class epoll_reactor {
public:
  epoll_reactor () {}

  ~epoll_reactor () {}

private:
  typedef std::shared_ptr<epoll_event> epoll_event_ptr;
//  typedef list<epoll_event_ptr> event_list;

//  event_list events_;

  int epoll_fd_;

  int waker_fd_;

  mutex mutex_;
};

KNGIN_NAMESPACE_K_DETAIL_IMPL_END

#endif /* KNGIN_EPOLL_REACTOR_H */