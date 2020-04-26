#include "kngin/core/define.h"
#include "kngin/core/base/error_code.h"

namespace k {

const char *
get_error_str (errno_type code) {
  switch (code) {
#define KNGIN_STRERROR_GEN(code, str) case KNGIN_##code: return str;
  KNGIN_ERRNO_MAP(KNGIN_STRERROR_GEN)
#undef KNGIN_STRERROR_GEN
  }
  return "unknown system error";
}

#if defined(KNGIN_SYSTEM_WIN32)
errno_type
translate_sys_error (DWORD code) {
  if (code <= 0)
    return code; // already a kngin code

  switch (code) {
    case ERROR_NOACCESS:                   return KNGIN_EACCES;
    case WSAEACCES:                        return KNGIN_EACCES;
    case ERROR_ELEVATION_REQUIRED:         return KNGIN_EACCES;
    case ERROR_ADDRESS_ALREADY_ASSOCIATED: return KNGIN_EADDRINUSE;
    case WSAEADDRINUSE:                    return KNGIN_EADDRINUSE;
    case WSAEADDRNOTAVAIL:                 return KNGIN_EADDRNOTAVAIL;
    case WSAEAFNOSUPPORT:                  return KNGIN_EAFNOSUPPORT;
    case WSAEWOULDBLOCK:                   return KNGIN_EAGAIN;
    case WSAEALREADY:                      return KNGIN_EALREADY;
    case ERROR_INVALID_FLAGS:              return KNGIN_EBADF;
    case ERROR_INVALID_HANDLE:             return KNGIN_EBADF;
    case ERROR_LOCK_VIOLATION:             return KNGIN_EBUSY;
    case ERROR_PIPE_BUSY:                  return KNGIN_EBUSY;
    case ERROR_SHARING_VIOLATION:          return KNGIN_EBUSY;
    case WAIT_TIMEOUT:                     return KNGIN_EBUSY;
    case ERROR_OPERATION_ABORTED:          return KNGIN_ECANCELED;
    case WSAEINTR:                         return KNGIN_ECANCELED;
    case ERROR_NO_UNICODE_TRANSLATION:     return KNGIN_ECHARSET;
    case ERROR_CONNECTION_ABORTED:         return KNGIN_ECONNABORTED;
    case WSAECONNABORTED:                  return KNGIN_ECONNABORTED;
    case ERROR_CONNECTION_REFUSED:         return KNGIN_ECONNREFUSED;
    case WSAECONNREFUSED:                  return KNGIN_ECONNREFUSED;
    case ERROR_NETNAME_DELETED:            return KNGIN_ECONNRESET;
    case WSAECONNRESET:                    return KNGIN_ECONNRESET;
    case ERROR_ALREADY_EXISTS:             return KNGIN_EEXIST;
    case ERROR_FILE_EXISTS:                return KNGIN_EEXIST;
    case ERROR_BUFFER_OVERFLOW:            return KNGIN_EFAULT;
    case WSAEFAULT:                        return KNGIN_EFAULT;
    case ERROR_HOST_UNREACHABLE:           return KNGIN_EHOSTUNREACH;
    case WSAEHOSTUNREACH:                  return KNGIN_EHOSTUNREACH;
    case ERROR_INSUFFICIENT_BUFFER:        return KNGIN_EINVAL;
    case ERROR_INVALID_DATA:               return KNGIN_EINVAL;
    case ERROR_INVALID_PARAMETER:          return KNGIN_EINVAL;
    case ERROR_SYMLINK_NOT_SUPPORTED:      return KNGIN_EINVAL;
    case WSAEINVAL:                        return KNGIN_EINVAL;
    case WSAEPFNOSUPPORT:                  return KNGIN_EINVAL;
    case WSAESOCKTNOSUPPORT:               return KNGIN_EINVAL;
    case ERROR_BEGINNING_OF_MEDIA:         return KNGIN_EIO;
    case ERROR_BUS_RESET:                  return KNGIN_EIO;
    case ERROR_CRC:                        return KNGIN_EIO;
    case ERROR_DEVICE_DOOR_OPEN:           return KNGIN_EIO;
    case ERROR_DEVICE_REQUIRES_CLEANING:   return KNGIN_EIO;
    case ERROR_DISK_CORRUPT:               return KNGIN_EIO;
    case ERROR_EOM_OVERFLOW:               return KNGIN_EIO;
    case ERROR_FILEMARK_DETECTED:          return KNGIN_EIO;
    case ERROR_GEN_FAILURE:                return KNGIN_EIO;
    case ERROR_INVALID_BLOCK_LENGTH:       return KNGIN_EIO;
    case ERROR_IO_DEVICE:                  return KNGIN_EIO;
    case ERROR_NO_DATA_DETECTED:           return KNGIN_EIO;
    case ERROR_NO_SIGNAL_SENT:             return KNGIN_EIO;
    case ERROR_OPEN_FAILED:                return KNGIN_EIO;
    case ERROR_SETMARK_DETECTED:           return KNGIN_EIO;
    case ERROR_SIGNAL_REFUSED:             return KNGIN_EIO;
    case WSAEISCONN:                       return KNGIN_EISCONN;
    case ERROR_CANT_RESOLVE_FILENAME:      return KNGIN_ELOOP;
    case ERROR_TOO_MANY_OPEN_FILES:        return KNGIN_EMFILE;
    case WSAEMFILE:                        return KNGIN_EMFILE;
    case WSAEMSGSIZE:                      return KNGIN_EMSGSIZE;
    case ERROR_FILENAME_EXCED_RANGE:       return KNGIN_ENAMETOOLONG;
    case ERROR_NETWORK_UNREACHABLE:        return KNGIN_ENETUNREACH;
    case WSAENETUNREACH:                   return KNGIN_ENETUNREACH;
    case WSAENOBUFS:                       return KNGIN_ENOBUFS;
    case ERROR_BAD_PATHNAME:               return KNGIN_ENOENT;
    case ERROR_DIRECTORY:                  return KNGIN_ENOENT;
    case ERROR_ENVVAR_NOT_FOUND:           return KNGIN_ENOENT;
    case ERROR_FILE_NOT_FOUND:             return KNGIN_ENOENT;
    case ERROR_INVALID_NAME:               return KNGIN_ENOENT;
    case ERROR_INVALID_DRIVE:              return KNGIN_ENOENT;
    case ERROR_INVALID_REPARSE_DATA:       return KNGIN_ENOENT;
    case ERROR_MOD_NOT_FOUND:              return KNGIN_ENOENT;
    case ERROR_PATH_NOT_FOUND:             return KNGIN_ENOENT;
    case WSAHOST_NOT_FOUND:                return KNGIN_ENOENT;
    case WSANO_DATA:                       return KNGIN_ENOENT;
    case ERROR_NOT_ENOUGH_MEMORY:          return KNGIN_ENOMEM;
    case ERROR_OUTOFMEMORY:                return KNGIN_ENOMEM;
    case ERROR_CANNOT_MAKE:                return KNGIN_ENOSPC;
    case ERROR_DISK_FULL:                  return KNGIN_ENOSPC;
    case ERROR_EA_TABLE_FULL:              return KNGIN_ENOSPC;
    case ERROR_END_OF_MEDIA:               return KNGIN_ENOSPC;
    case ERROR_HANDLE_DISK_FULL:           return KNGIN_ENOSPC;
    case ERROR_NOT_CONNECTED:              return KNGIN_ENOTCONN;
    case WSAENOTCONN:                      return KNGIN_ENOTCONN;
    case ERROR_DIR_NOT_EMPTY:              return KNGIN_ENOTEMPTY;
    case WSAENOTSOCK:                      return KNGIN_ENOTSOCK;
    case ERROR_NOT_SUPPORTED:              return KNGIN_ENOTSUP;
    case ERROR_BROKEN_PIPE:                return KNGIN_EOF;
    case ERROR_ACCESS_DENIED:              return KNGIN_EPERM;
    case ERROR_PRIVILEGE_NOT_HELD:         return KNGIN_EPERM;
    case ERROR_BAD_PIPE:                   return KNGIN_EPIPE;
    case ERROR_NO_DATA:                    return KNGIN_EPIPE;
    case ERROR_PIPE_NOT_CONNECTED:         return KNGIN_EPIPE;
    case WSAESHUTDOWN:                     return KNGIN_EPIPE;
    case WSAEPROTONOSUPPORT:               return KNGIN_EPROTONOSUPPORT;
    case ERROR_WRITE_PROTECT:              return KNGIN_EROFS;
    case ERROR_SEM_TIMEOUT:                return KNGIN_ETIMEDOUT;
    case WSAETIMEDOUT:                     return KNGIN_ETIMEDOUT;
    case ERROR_NOT_SAME_DEVICE:            return KNGIN_EXDEV;
    case ERROR_INVALID_FUNCTION:           return KNGIN_EISDIR;
    case ERROR_META_EXPANSION_TOO_LONG:    return KNGIN_E2BIG;
    default:                               return KNGIN_EUNKNOWN;
  }
}
#endif /* defined(KNGIN_SYSTEM_WIN32) */


} /* namespace k */
