#ifndef KNGIN_CONFIG_H
#define KNGIN_CONFIG_H

#define ON  true
#define OFF false

// compiler checking
#if (__cplusplus < 201703L)
#  error c++ version is too low, please build the project using c++17 standard.
#endif /* (__cplusplus < 201703L) */

/**** micro test ****/
// for log
#define ENABLE_LOG_RELATIVE_PATH
//#define ENABLE_USE_ASYNC_LOGGER
//#define ASYNC_LOGGER_TIMEOUT 3000ULL

// for timer
//#define ENABLE_USE_TIMERFD
#define TIMER_REMAINING_PRESISION 0ULL

// for session
//#define ENABLE_SESSION_USE_C_HANDLER
//#define ENABLE_SESSION_NO_MUTEX
//#define ENABLE_SESSION_USE_ET_MODE

// for epoll
#define REACTOR_WAIT_SET_SIZE 64

// reactor
#define REACTOR_TIMEOUT 3000ULL
/**** end of micro test ****/

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

// log
#define FILENAME __FILE__
#if (ON == KNGIN_LOG_RELATIVE_PATH)
#  undef FILENAME
#  define FILENAME KNGIN_FILENAME
#endif /* (ON == KNGIN_LOG_RELATIVE_PATH) */
#if defined(NDEBUG)
#  define FUNCTION __FUNCTION__
#else
#  define FUNCTION ""
#endif /* defined(NDEBUG) */
#if defined(ENABLE_LOG_RELATIVE_PATH)
#  define KNGIN_LOG_RELATIVE_PATH
#endif /* defined(ENABLE_LOG_RELATIVE_PATH) */
#if defined(ENABLE_USE_ASYNC_LOGGER)
#define KNGIN_USE_ASYNC_LOGGER
#define KNGIN_ASYNC_LOGGER_TIMEOUT ASYNC_LOGGER_TIMEOUT
#endif /* defined(ENABLE_USE_ASYNC_LOGGER) */

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

#endif /* KNGIN_CONFIG_H */