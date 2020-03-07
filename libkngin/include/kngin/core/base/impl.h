#ifndef KNGIN_IMPL_H
#define KNGIN_IMPL_H

#define declare_detail_impl(type)       \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN \
      class type;                       \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END

// mutex
#if defined(KNGIN_USE_WIN_MUTEX)
#  define typedef_mutex_impl(type) \
  declare_detail_impl(win_mutex);  \
  typedef k::detail::impl::win_mutex(type)
#elif defined(KNGIN_USE_POSIX_MUTEX)
#  define typedef_mutex_impl(type)  \
  declare_detail_impl(posix_mutex); \
  typedef k::detail::impl::posix_mutex(type)
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

// cond
#if defined(KNGIN_USE_WIN_COND)
#  define typedef_cond_impl(type) \
  declare_detail_impl(win_cond);  \
  typedef k::detail::impl::win_cond(type)
#elif defined(KNGIN_USE_POSIX_COND)
#  define typedef_cond_impl(type)  \
  declare_detail_impl(posix_cond); \
  typedef k::detail::impl::posix_cond(type)
#endif /* defined(KNGIN_USE_WIN_COND) */

// rwlock
#if defined(KNGIN_USE_WIN_RWLOCK)
#  define TYPEDEF_RWLOCK_IMPL(type) \
  declare_detail_impl(win_rwlock);  \
  typedef k::detail::impl::win_rwlock(type)
#elif defined(KNGIN_USE_POSIX_RWLOCK)
#  define TYPEDEF_RWLOCK_IMPL(type)  \
  declare_detail_impl(posix_rwlock); \
  typedef k::detail::impl::posix_rwlock(type)
#endif /* defined(KNGIN_USE_WIN_RWLOCK) */

// barrier
#if defined(KNGIN_USE_WIN_BARRIER)
#  define typedef_barrier_impl(type) \
  declare_detail_impl(win_barrier);  \
  typedef k::detail::impl::win_barrier(type)
#elif defined(KNGIN_USE_POSIX_BARRIER)
#  define typedef_barrier_impl(type)  \
  declare_detail_impl(posix_barrier); \
  typedef k::detail::impl::posix_barrier(type)
#endif /* defined(KNGIN_USE_WIN_BARRIER) */

// thread
#if defined(KNGIN_USE_WIN_THREAD)
#  define typedef_thread_impl(type) \
  declare_detail_impl(win_thread);  \
  typedef k::detail::impl::win_thread(type)
#elif defined(KNGIN_USE_POSIX_THREAD)
#  define typedef_thread_impl(type)  \
  declare_detail_impl(posix_thread); \
  typedef k::detail::impl::posix_thread(type)
#endif /* defined(KNGIN_USE_WIN_THREAD) */

// reactor
#if defined(KNGIN_USE_IOCP_REACTOR)
#  define typedef_reactor_impl(type) \
  declare_detail_impl(iocp_reactor); \
  typedef k::detail::impl::iocp_reactor(type)
#elif defined(KNGIN_USE_EPOLL_REACTOR)
#  define typedef_reactor_impl(type)  \
  declare_detail_impl(epoll_reactor); \
  typedef k::detail::impl::epoll_reactor(type)
#elif defined(KNGIN_USE_POLL_REACTOR)
#  define typedef_reactor_impl(type) \
  declare_detail_impl(poll_reactor); \
  typedef k::detail::impl::poll_reactor(type)
#elif defined(KNGIN_USE_SELECT_REACTOR)
#  define typedef_reactor_impl(type)   \
  declare_detail_impl(select_reactor); \
  typedef k::detail::impl::select_reactor(type)
#endif /* defined(KNGIN_USE_IOCP_REACTOR) */

#endif /* KNGIN_IMPL_H */