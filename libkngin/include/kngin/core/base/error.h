/*
 * Copy from libuv
 *
 * */

#ifndef KNGIN_ERROR_H
#define KNGIN_ERROR_H

#include <cerrno>

namespace k {

#if EDOM > 0
# define KNGIN_ERRNO(x) (errno_type)(-(x))
# define KNGIN__ERRNO(x) (-(x))
#else
# define KNGIN_ERRNO(x) (errno_type)(x)
# define KNGIN__ERRNO(x) (x)
#endif

#define KNGIN__ESUCCESS        (0)
#define KNGIN__EUNKNOWN        (-4095)
#define KNGIN__EOF             (-4094)

#define KNGIN__EAI_ADDRFAMILY  (-3000)
#define KNGIN__EAI_AGAIN       (-3001)
#define KNGIN__EAI_BADFLAGS    (-3002)
#define KNGIN__EAI_CANCELED    (-3003)
#define KNGIN__EAI_FAIL        (-3004)
#define KNGIN__EAI_FAMILY      (-3005)
#define KNGIN__EAI_MEMORY      (-3006)
#define KNGIN__EAI_NODATA      (-3007)
#define KNGIN__EAI_NONAME      (-3008)
#define KNGIN__EAI_OVERFLOW    (-3009)
#define KNGIN__EAI_SERVICE     (-3010)
#define KNGIN__EAI_SOCKTYPE    (-3011)
#define KNGIN__EAI_BADHINTS    (-3013)
#define KNGIN__EAI_PROTOCOL    (-3014)

/* Only map to the system errno on non-Windows platforms. It's apparently
 * a fairly common practice for Windows programmers to redefine errno codes.
 */
#if defined(E2BIG) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__E2BIG KNGIN__ERRNO(E2BIG)
#else
# define KNGIN__E2BIG (-4093)
#endif

#if defined(EACCES) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EACCES KNGIN__ERRNO(EACCES)
#else
# define KNGIN__EACCES (-4092)
#endif

#if defined(EADDRINUSE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EADDRINUSE KNGIN__ERRNO(EADDRINUSE)
#else
# define KNGIN__EADDRINUSE (-4091)
#endif

#if defined(EADDRNOTAVAIL) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EADDRNOTAVAIL KNGIN__ERRNO(EADDRNOTAVAIL)
#else
# define KNGIN__EADDRNOTAVAIL (-4090)
#endif

#if defined(EAFNOSUPPORT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EAFNOSUPPORT KNGIN__ERRNO(EAFNOSUPPORT)
#else
# define KNGIN__EAFNOSUPPORT (-4089)
#endif

#if defined(EAGAIN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EAGAIN KNGIN__ERRNO(EAGAIN)
#else
# define KNGIN__EAGAIN (-4088)
#endif

#if defined(EALREADY) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EALREADY KNGIN__ERRNO(EALREADY)
#else
# define KNGIN__EALREADY (-4084)
#endif

#if defined(EBADF) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EBADF KNGIN__ERRNO(EBADF)
#else
# define KNGIN__EBADF (-4083)
#endif

#if defined(EBUSY) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EBUSY KNGIN__ERRNO(EBUSY)
#else
# define KNGIN__EBUSY (-4082)
#endif

#if defined(ECANCELED) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ECANCELED KNGIN__ERRNO(ECANCELED)
#else
# define KNGIN__ECANCELED (-4081)
#endif

#if defined(ECHARSET) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ECHARSET KNGIN__ERRNO(ECHARSET)
#else
# define KNGIN__ECHARSET (-4080)
#endif

#if defined(ECONNABORTED) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ECONNABORTED KNGIN__ERRNO(ECONNABORTED)
#else
# define KNGIN__ECONNABORTED (-4079)
#endif

#if defined(ECONNREFUSED) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ECONNREFUSED KNGIN__ERRNO(ECONNREFUSED)
#else
# define KNGIN__ECONNREFUSED (-4078)
#endif

#if defined(ECONNRESET) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ECONNRESET KNGIN__ERRNO(ECONNRESET)
#else
# define KNGIN__ECONNRESET (-4077)
#endif

#if defined(EDESTADDRREQ) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EDESTADDRREQ KNGIN__ERRNO(EDESTADDRREQ)
#else
# define KNGIN__EDESTADDRREQ (-4076)
#endif

#if defined(EEXIST) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EEXIST KNGIN__ERRNO(EEXIST)
#else
# define KNGIN__EEXIST (-4075)
#endif

#if defined(EFAULT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EFAULT KNGIN__ERRNO(EFAULT)
#else
# define KNGIN__EFAULT (-4074)
#endif

#if defined(EHOSTUNREACH) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EHOSTUNREACH KNGIN__ERRNO(EHOSTUNREACH)
#else
# define KNGIN__EHOSTUNREACH (-4073)
#endif

#if defined(EINTR) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EINTR KNGIN__ERRNO(EINTR)
#else
# define KNGIN__EINTR (-4072)
#endif

#if defined(EINVAL) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EINVAL KNGIN__ERRNO(EINVAL)
#else
# define KNGIN__EINVAL (-4071)
#endif

#if defined(EIO) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EIO KNGIN__ERRNO(EIO)
#else
# define KNGIN__EIO (-4070)
#endif

#if defined(EISCONN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EISCONN KNGIN__ERRNO(EISCONN)
#else
# define KNGIN__EISCONN (-4069)
#endif

#if defined(EISDIR) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EISDIR KNGIN__ERRNO(EISDIR)
#else
# define KNGIN__EISDIR (-4068)
#endif

#if defined(ELOOP) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ELOOP KNGIN__ERRNO(ELOOP)
#else
# define KNGIN__ELOOP (-4067)
#endif

#if defined(EMFILE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EMFILE KNGIN__ERRNO(EMFILE)
#else
# define KNGIN__EMFILE (-4066)
#endif

#if defined(EMSGSIZE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EMSGSIZE KNGIN__ERRNO(EMSGSIZE)
#else
# define KNGIN__EMSGSIZE (-4065)
#endif

#if defined(ENAMETOOLONG) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENAMETOOLONG KNGIN__ERRNO(ENAMETOOLONG)
#else
# define KNGIN__ENAMETOOLONG (-4064)
#endif

#if defined(ENETDOWN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENETDOWN KNGIN__ERRNO(ENETDOWN)
#else
# define KNGIN__ENETDOWN (-4063)
#endif

#if defined(ENETUNREACH) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENETUNREACH KNGIN__ERRNO(ENETUNREACH)
#else
# define KNGIN__ENETUNREACH (-4062)
#endif

#if defined(ENFILE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENFILE KNGIN__ERRNO(ENFILE)
#else
# define KNGIN__ENFILE (-4061)
#endif

#if defined(ENOBUFS) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOBUFS KNGIN__ERRNO(ENOBUFS)
#else
# define KNGIN__ENOBUFS (-4060)
#endif

#if defined(ENODEV) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENODEV KNGIN__ERRNO(ENODEV)
#else
# define KNGIN__ENODEV (-4059)
#endif

#if defined(ENOENT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOENT KNGIN__ERRNO(ENOENT)
#else
# define KNGIN__ENOENT (-4058)
#endif

#if defined(ENOMEM) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOMEM KNGIN__ERRNO(ENOMEM)
#else
# define KNGIN__ENOMEM (-4057)
#endif

#if defined(ENONET) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENONET KNGIN__ERRNO(ENONET)
#else
# define KNGIN__ENONET (-4056)
#endif

#if defined(ENOSPC) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOSPC KNGIN__ERRNO(ENOSPC)
#else
# define KNGIN__ENOSPC (-4055)
#endif

#if defined(ENOSYS) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOSYS KNGIN__ERRNO(ENOSYS)
#else
# define KNGIN__ENOSYS (-4054)
#endif

#if defined(ENOTCONN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTCONN KNGIN__ERRNO(ENOTCONN)
#else
# define KNGIN__ENOTCONN (-4053)
#endif

#if defined(ENOTDIR) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTDIR KNGIN__ERRNO(ENOTDIR)
#else
# define KNGIN__ENOTDIR (-4052)
#endif

#if defined(ENOTEMPTY) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTEMPTY KNGIN__ERRNO(ENOTEMPTY)
#else
# define KNGIN__ENOTEMPTY (-4051)
#endif

#if defined(ENOTSOCK) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTSOCK KNGIN__ERRNO(ENOTSOCK)
#else
# define KNGIN__ENOTSOCK (-4050)
#endif

#if defined(ENOTSUP) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTSUP KNGIN__ERRNO(ENOTSUP)
#else
# define KNGIN__ENOTSUP (-4049)
#endif

#if defined(EPERM) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EPERM KNGIN__ERRNO(EPERM)
#else
# define KNGIN__EPERM (-4048)
#endif

#if defined(EPIPE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EPIPE KNGIN__ERRNO(EPIPE)
#else
# define KNGIN__EPIPE (-4047)
#endif

#if defined(EPROTO) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EPROTO KNGIN__ERRNO(EPROTO)
#else
# define KNGIN__EPROTO KNGIN__ERRNO(4046)
#endif

#if defined(EPROTONOSUPPORT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EPROTONOSUPPORT KNGIN__ERRNO(EPROTONOSUPPORT)
#else
# define KNGIN__EPROTONOSUPPORT (-4045)
#endif

#if defined(EPROTOTYPE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EPROTOTYPE KNGIN__ERRNO(EPROTOTYPE)
#else
# define KNGIN__EPROTOTYPE (-4044)
#endif

#if defined(EROFS) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EROFS KNGIN__ERRNO(EROFS)
#else
# define KNGIN__EROFS (-4043)
#endif

#if defined(ESHUTDOWN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ESHUTDOWN KNGIN__ERRNO(ESHUTDOWN)
#else
# define KNGIN__ESHUTDOWN (-4042)
#endif

#if defined(ESPIPE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ESPIPE KNGIN__ERRNO(ESPIPE)
#else
# define KNGIN__ESPIPE (-4041)
#endif

#if defined(ESRCH) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ESRCH KNGIN__ERRNO(ESRCH)
#else
# define KNGIN__ESRCH (-4040)
#endif

#if defined(ETIMEDOUT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ETIMEDOUT KNGIN__ERRNO(ETIMEDOUT)
#else
# define KNGIN__ETIMEDOUT (-4039)
#endif

#if defined(ETXTBSY) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ETXTBSY KNGIN__ERRNO(ETXTBSY)
#else
# define KNGIN__ETXTBSY (-4038)
#endif

#if defined(EXDEV) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EXDEV KNGIN__ERRNO(EXDEV)
#else
# define KNGIN__EXDEV (-4037)
#endif

#if defined(EFBIG) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EFBIG KNGIN__ERRNO(EFBIG)
#else
# define KNGIN__EFBIG (-4036)
#endif

#if defined(ENOPROTOOPT) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOPROTOOPT KNGIN__ERRNO(ENOPROTOOPT)
#else
# define KNGIN__ENOPROTOOPT (-4035)
#endif

#if defined(ERANGE) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ERANGE KNGIN__ERRNO(ERANGE)
#else
# define KNGIN__ERANGE (-4034)
#endif

#if defined(ENXIO) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENXIO KNGIN__ERRNO(ENXIO)
#else
# define KNGIN__ENXIO (-4033)
#endif

#if defined(EMLINK) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EMLINK KNGIN__ERRNO(EMLINK)
#else
# define KNGIN__EMLINK (-4032)
#endif

/* EHOSTDOWN is not visible on BSD-like systems when _POSIX_C_SOURCE is
 * defined. Fortunately, its value is always 64 so it's possible albeit
 * icky to hard-code it.
 */
#if defined(EHOSTDOWN) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EHOSTDOWN KNGIN__ERRNO(EHOSTDOWN)
#elif defined(__APPLE__) or \
      defined(__DragonFly__) or \
      defined(__FreeBSD__) or \
      defined(__FreeBSD_kernel__) or \
      defined(__NetBSD__) or \
      defined(__OpenBSD__)
# define KNGIN__EHOSTDOWN (-64)
#else
# define KNGIN__EHOSTDOWN (-4031)
#endif

#if defined(EREMOTEIO) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EREMOTEIO KNGIN__ERRNO(EREMOTEIO)
#else
# define KNGIN__EREMOTEIO (-4030)
#endif

#if defined(ENOTTY) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__ENOTTY KNGIN__ERRNO(ENOTTY)
#else
# define KNGIN__ENOTTY (-4029)
#endif

#if defined(EILSEQ) and !defined(KNGIN_SYSTEM_WIN32)
# define KNGIN__EILSEQ KNGIN__ERRNO(EILSEQ)
#else
# define KNGIN__EILSEQ (-4028)
#endif

#define KNGIN_ERRNO_MAP(XX)                                            \
  XX(ESUCCESS,        "success")                                       \
  XX(E2BIG,           "argument list too long")                        \
  XX(EACCES,          "permission denied")                             \
  XX(EADDRINUSE,      "address already in use")                        \
  XX(EADDRNOTAVAIL,   "address not available")                         \
  XX(EAFNOSUPPORT,    "address family not supported")                  \
  XX(EAGAIN,          "resource temporarily unavailable")              \
  XX(EAI_ADDRFAMILY,  "address family not supported")                  \
  XX(EAI_AGAIN,       "temporary failure")                             \
  XX(EAI_BADFLAGS,    "bad ai_flags value")                            \
  XX(EAI_BADHINTS,    "invalid value for hints")                       \
  XX(EAI_CANCELED,    "request canceled")                              \
  XX(EAI_FAIL,        "permanent failure")                             \
  XX(EAI_FAMILY,      "ai_family not supported")                       \
  XX(EAI_MEMORY,      "out of memory")                                 \
  XX(EAI_NODATA,      "no address")                                    \
  XX(EAI_NONAME,      "unknown node or service")                       \
  XX(EAI_OVERFLOW,    "argument buffer overflow")                      \
  XX(EAI_PROTOCOL,    "resolved protocol is unknown")                  \
  XX(EAI_SERVICE,     "service not available for socket type")         \
  XX(EAI_SOCKTYPE,    "socket type not supported")                     \
  XX(EALREADY,        "connection already in progress")                \
  XX(EBADF,           "bad file descriptor")                           \
  XX(EBUSY,           "resource busy or locked")                       \
  XX(ECANCELED,       "operation canceled")                            \
  XX(ECHARSET,        "invalid Unicode character")                     \
  XX(ECONNABORTED,    "software caused connection abort")              \
  XX(ECONNREFUSED,    "connection refused")                            \
  XX(ECONNRESET,      "connection reset by peer")                      \
  XX(EDESTADDRREQ,    "destination address required")                  \
  XX(EEXIST,          "file already exists")                           \
  XX(EFAULT,          "bad address in system call argument")           \
  XX(EFBIG,           "file too large")                                \
  XX(EHOSTUNREACH,    "host is unreachable")                           \
  XX(EINTR,           "interrupted system call")                       \
  XX(EINVAL,          "invalid argument")                              \
  XX(EIO,             "i/o error")                                     \
  XX(EISCONN,         "socket is already connected")                   \
  XX(EISDIR,          "illegal operation on a directory")              \
  XX(ELOOP,           "too many symbolic links encountered")           \
  XX(EMFILE,          "too many open files")                           \
  XX(EMSGSIZE,        "message too long")                              \
  XX(ENAMETOOLONG,    "name too long")                                 \
  XX(ENETDOWN,        "network is down")                               \
  XX(ENETUNREACH,     "network is unreachable")                        \
  XX(ENFILE,          "file table overflow")                           \
  XX(ENOBUFS,         "no buffer space available")                     \
  XX(ENODEV,          "no such device")                                \
  XX(ENOENT,          "no such file or directory")                     \
  XX(ENOMEM,          "not enough memory")                             \
  XX(ENONET,          "machine is not on the network")                 \
  XX(ENOPROTOOPT,     "protocol not available")                        \
  XX(ENOSPC,          "no space left on device")                       \
  XX(ENOSYS,          "function not implemented")                      \
  XX(ENOTCONN,        "socket is not connected")                       \
  XX(ENOTDIR,         "not a directory")                               \
  XX(ENOTEMPTY,       "directory not empty")                           \
  XX(ENOTSOCK,        "socket operation on non-socket")                \
  XX(ENOTSUP,         "operation not supported on socket")             \
  XX(EPERM,           "operation not permitted")                       \
  XX(EPIPE,           "broken pipe")                                   \
  XX(EPROTO,          "protocol error")                                \
  XX(EPROTONOSUPPORT, "protocol not supported")                        \
  XX(EPROTOTYPE,      "protocol wrong type for socket")                \
  XX(ERANGE,          "result too large")                              \
  XX(EROFS,           "read-only file system")                         \
  XX(ESHUTDOWN,       "cannot send after transport endpoint shutdown") \
  XX(ESPIPE,          "invalid seek")                                  \
  XX(ESRCH,           "no such process")                               \
  XX(ETIMEDOUT,       "connection timed out")                          \
  XX(ETXTBSY,         "text file is busy")                             \
  XX(EXDEV,           "cross-device link not permitted")               \
  XX(ENXIO,           "no such device or address")                     \
  XX(EMLINK,          "too many links")                                \
  XX(EHOSTDOWN,       "host is down")                                  \
  XX(EREMOTEIO,       "remote I/O error")                              \
  XX(ENOTTY,          "inappropriate ioctl for device")                \
  XX(EILSEQ,          "illegal byte sequence")                         \
  XX(EOF,             "end of file")                                   \
  XX(EUNKNOWN,        "unknown error")                                 \

typedef enum {
#define KNGIN_ERRNO_ENUM_GEN(code, _) KNGIN_##code = KNGIN__##code,
  KNGIN_ERRNO_MAP(KNGIN_ERRNO_ENUM_GEN)
#undef KNGIN_ERRNO_ENUM_GEN
} errno_type;

const char *
get_error_str (errno_type code);

#if defined(KNGIN_SYSTEM_WIN32)
errno_type
translate_sys_error (DWORD code);
#endif /* defined(KNGIN_SYSTEM_WIN32) */

} /* namespace k */

#endif /* KNGIN_ERROR_H */
