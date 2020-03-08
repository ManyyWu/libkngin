#ifndef KNGIN_BASE_IMPL_H
#define KNGIN_BASE_IMPL_H

#include "kngin/core/define.h"

// mutex
#if defined(KNGIN_USE_WIN_MUTEX)
  declare_detail_impl_class(win_mutex)
  typedef_detail_impl(win_mutex, mutex_impl)
#elif defined(KNGIN_USE_POSIX_MUTEX)
  declare_detail_impl_class(posix_mutex)
  typedef_detail_impl(posix_mutex, mutex_impl)
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

// cond
#if defined(KNGIN_USE_WIN_COND)
  declare_detail_impl_class(win_cond)
  typedef_detail_impl(win_cond, cond_impl)
#elif defined(KNGIN_USE_POSIX_COND)
  declare_detail_impl_class(posix_cond)
  typedef_detail_impl(posix_cond, cond_impl)
#endif /* defined(KNGIN_USE_WIN_COND) */

// rwlock
#if defined(KNGIN_USE_WIN_RWLOCK)
  declare_detail_impl_class(win_rwlock)
  typedef_detail_impl(win_rwlock, rwlock_impl)
#elif defined(KNGIN_USE_POSIX_RWLOCK)
  declare_detail_impl_class(posix_rwlock)
  typedef_detail_impl(posix_rwlock, rwlock_impl)
#endif /* defined(KNGIN_USE_WIN_RWLOCK) */

// barrier
#if defined(KNGIN_USE_WIN_BARRIER)
  declare_detail_impl_class(win_barrier)
  typedef_detail_impl(win_barrier, barrier_impl)
#elif defined(KNGIN_USE_POSIX_BARRIER)
  declare_detail_impl_class(posix_barrier)
  typedef_detail_impl(posix_barrier, barrier_impl)
#endif /* defined(KNGIN_USE_WIN_BARRIER) */

// thread
#if defined(KNGIN_USE_WIN_THREAD)
  declare_detail_impl_class(win_thread)
  typedef_detail_impl(win_thread, thread_impl)
#elif defined(KNGIN_USE_POSIX_THREAD)
  declare_detail_impl_class(posix_thread)
  typedef_detail_impl(posix_thread, thread_impl)
#endif /* defined(KNGIN_USE_WIN_THREAD) */

#endif /* KNGIN_BASE_IMPL_H */