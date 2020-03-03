#ifndef KNGIN_IMPL_H
#define KNGIN_IMPL_H

// mutex
#if defined(KNGIN_USE_WIN_MUTEX)
#  define TYPEDEF_MUTEX_IMPL(type)     \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class win_mutex;                 \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::win_mutex (type)
#elif defined(KNGIN_USE_POSIX_MUTEX)
#  define TYPEDEF_MUTEX_IMPL(type)     \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class posix_mutex;               \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::posix_mutex (type)
#endif /* defined(KNGIN_USE_WIN_MUTEX) */

// cond
#if defined(KNGIN_USE_WIN_COND)
#  define TYPEDEF_COND_IMPL(type)      \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class win_cond;                  \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::win_cond (type)
#elif defined(KNGIN_USE_POSIX_COND)
#  define TYPEDEF_COND_IMPL(type)      \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class posix_cond;                \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::posix_cond (type)
#endif /* defined(KNGIN_USE_WIN_COND) */

// rwlock
#if defined(KNGIN_USE_WIN_RWLOCK)
#  define TYPEDEF_RWLOCK_IMPL(type)    \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class win_rwlock;                \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::win_rwlock (type)
#elif defined(KNGIN_USE_POSIX_RWLOCK)
#  define TYPEDEF_RWLOCK_IMPL(type)    \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class posix_rwlock;              \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::posix_rwlock (type)
#endif /* defined(KNGIN_USE_WIN_RWLOCK) */

// barrier
#if defined(KNGIN_USE_WIN_BARRIER)
#  define TYPEDEF_BARRIER_IMPL(type)   \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class win_barrier;               \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::win_barrier (type)
#elif defined(KNGIN_USE_POSIX_BARRIER)
#  define TYPEDEF_BARRIER_IMPL(type)   \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class posix_barrier;             \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::posix_barrier (type)
#endif /* defined(KNGIN_USE_WIN_BARRIER) */

// thread
#if defined(KNGIN_USE_WIN_THREAD)
#  define TYPEDEF_THREAD_IMPL(type)    \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class win_thread;                \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::win_thread (type)
#elif defined(KNGIN_USE_POSIX_THREAD)
#  define TYPEDEF_THREAD_IMPL(type)    \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN\
      class posix_thread;              \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END  \
    typedef k::detail::impl::posix_thread (type)
#endif /* defined(KNGIN_USE_WIN_THREAD) */

#endif /* KNGIN_IMPL_H */