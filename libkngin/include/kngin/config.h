#ifndef KNGIN_CONFIG_H
#define KNGIN_CONFIG_H

// compiler checking
#if (__cplusplus < 201703L)
//#  error c++ version is too low, please build the project using c++17 standard.
#endif /* (__cplusplus < 201703L) */

// platform checking
#if defined(_WIN32)
#  define KNGIN_SYSTEM_WIN32
#  if defined (_WIN64)
#    define KNGIN_SYSTEM_WIN64
#  endif /* KNGIN_SYSTEM_WIN32 */
#elif defined(TARGET_OS_MAC)
#  define KNGIN_SYSTEM_MACOS
#elif defined(__linux__)
#  define KNGIN_SYSTEM_LINUX
#elif defined(__unix__)
#  define KNGIN_SYSTEM_UNIX
#else
#  error unsupported platform
#endif /* defined(_WIN32) */

// mutex
#if !defined(KNGIN_SYSTEM_WIN32)
#  undef ENABLE_USE_WIN_MUTEX
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(ENABLE_USE_WIN_MUTEX)
#  define KNGIN_USE_WIN_MUTEX
#elif defined(ENABLE_USE_POSIX_MUTEX)
#  define KNGIN_USE_POSIX_MUTEX
#else
#  define KNGIN_USE_POSIX_MUTEX
#endif /* defined(ENABLE_USE_WIN_MUTEX) */

// cond
#if !defined(KNGIN_SYSTEM_WIN32)
#  undef ENABLE_USE_WIN_COND
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(ENABLE_USE_WIN_COND)
#  define KNGIN_USE_WIN_COND
#elif defined(ENABLE_USE_POSIX_COND)
#  define KNGIN_USE_POSIX_COND
#else
#  define KNGIN_USE_POSIX_COND
#endif /* defined(ENABLE_USE_WIN_COND) */

// rwlock
#if !defined(KNGIN_SYSTEM_WIN32)
#  undef ENABLE_USE_WIN_RWLOCK
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(ENABLE_USE_WIN_RWLOCK)
#  define KNGIN_USE_WIN_RWLOCK
#elif defined(ENABLE_USE_POSIX_RWLOCK)
#  define KNGIN_USE_POSIX_RWLOCK
#else
#  define KNGIN_USE_POSIX_RWLOCK
#endif /* defined(ENABLE_USE_WIN_RWLOCK) */

// barrier
#if !defined(KNGIN_SYSTEM_WIN32)
#  undef ENABLE_USE_WIN_BARRIER
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(ENABLE_USE_WIN_BARRIER)
#  define KNGIN_USE_WIN_BARRIER
#elif defined(ENABLE_USE_POSIX_BARRIER)
#  define KNGIN_USE_POSIX_BARRIER
#else
#  define KNGIN_USE_POSIX_BARRIER
#endif /* defined(ENABLE_USE_WIN_BARRIER) */

// thread
#if !defined(KNGIN_SYSTEM_WIN32)
#  undef ENABLE_USE_WIN_THREAD
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(ENABLE_USE_WIN_THREAD)
#  define KNGIN_USE_WIN_THREAD
#elif defined(ENABLE_USE_POSIX_THREAD)
#  define KNGIN_USE_POSIX_THREAD
#else
#  error no thread definition was be selected
#endif /* defined(ENABLE_USE_WIN_THREAD)*/
#if defined(ENABLE_THREAD_USE_STD_FUNCTION)
#  define KNGIN_THREAD_USE_STD_FUNCTION
#endif /* defined(ENABLE_THREAD_USE_STD_FUNCTION) */

// log
#define FILENAME __FILE__
#if defined(NDEBUG)
#  define FUNCTION __FUNCTION__
#else
#  define FUNCTION ""
#endif /* defined(NDEBUG) */
#if defined(ENABLE_LOG_RELATIVE_PATH)
#  define KNGIN_LOG_RELATIVE_PATH
#endif /* defined(ENABLE_LOG_RELATIVE_PATH) */
#if defined(ENABLE_USE_ASYNC_LOGGER)
#  define KNGIN_USE_ASYNC_LOGGER
#  if defined(ASYNC_LOGGER_TIMEOUT)
#    define KNGIN_ASYNC_LOGGER_TIMEOUT ASYNC_LOGGER_TIMEOUT
#  else
#    defien KNGIN_ASYNC_LOGGER_TIMEOUT 3000ULL
#  endif /* defined(ASYNC_LOGGER_TIMEOUT) */
#endif /* defined(ENABLE_USE_ASYNC_LOGGER) */
#if defined(ENABLE_LOG_CALLBACK_USE_STD_FUNCTION)
#  define KNGIN_LOG_CALLBACK_USE_STD_FUNCTION
#endif /* defined(ENABLE_LOG_CALLBACK_USE_STD_FUNCTION) */

// session
#if defined(ENABLE_SESSION_USE_C_HANDLER)
#  define KNGIN_SESSION_USE_C_HANDLER
#endif /* defined(KNGIN_SESSION_USE_C_HANDLER) */
#if defined(ENABLE_SESSION_NO_MUTEX)
#  define KNGIN_SESSION_NO_MUTEX
#endif /* defined(KNGIN_SESSION_NO_MUTEX) */
#if !defined(KNGIN_SYSTEM_WIN32)
#  if defined(ENABLE_SESSION_USE_ET_MODE)
#    define KNGIN_SESSION_USE_ET_MODE
#  endif /* defined(KNGIN_SESSION_USE_ET_MODE) */
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

// timer
#if defined(KNGIN_SYSTEM_WIN32)
#  undef KNGIN_USE_TIMERFD
#endif /* KNGIN_SYSTEM_WIN32 */

// epoll
#if !defined(KNGIN_SYSTEM_WIN32)
#  define KNGIN_HAS_DISCRIPTOR
#  define KNGIN_HAS_EPOLL
#  define KNGIN_HAS_EPOLL_EVENT
#  define KNGIN_EPOLL_REACTOR_WAIT_SET_SIZE REACTOR_WAIT_SET_SIZE
#  define KNGIN_EPOLL_TIMEOUT REACTOR_TIMEOUT
#endif /* !defined(KNGIN_SYSTEM_WIN32) */

// iocp
#if defined(KNGIN_SYSTEM_WIN32)
#  define KNGIN_HAS_IOCP
#  define KNGIN_IOCP_REACTOR_WAIT_SET_SIZE REACTOR_WAIT_SET_SIZE
#  define KNGIN_IOCP_TIMEOUT REACTOR_TIMEOUT
#endif /* defined(KNGIN_SYSTEM_WIN32) */

// timer
#if defined(ENABLE_USE_TIMERFD)
#  define KNGIN_USE_TIMERFD
#else
#  define KNGIN_TIMER_REMAINING_PRESISION TIMER_REMAINING_PRESISION
#endif /* defined(ENABLE_USE_TIMERFD) */

// event_fd
#if !defined(KNGIN_SYSTEM_WIN32)
#  define KNGIN_HAS_EVENTFD
#endif /* !defined(KNGIN_SYSTEM_WIN32) */
#if defined(KNGIN_SYSTEM_MACOS)
#  define KNGIN_HAS_EVENTFD_WINE
#endif /* defined(KNGIN_SYSTEM_MACOS) */

// list
#if defined(AUTO_FIX_THRESHOLD)
#  define KNGIN_AUTO_FIX_THRESHOLD AUTO_FIX_THRESHOLD
#else
#  define KNGIN_AUTO_FIX_THRESHOLD 100
#endif /* defined(AUTO_FIX_THRESHOLD) */
#if defined(PRE_ALLOC_SIZE)
#  define KNGIN_PRE_ALLOC_SIZE PRE_ALLOC_SIZE
#else
#  define KNGIN_PRE_ALLOC_SIZE 15
#endif /* defined(PRE_ALLOC_SIZE) */


#endif /* KNGIN_CONFIG_H */