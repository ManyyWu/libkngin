#ifndef KNGIN_DEFINE_H
#define KNGIN_DEFINE_H

// includes
#include "kngin/config.h"
#include <ciso646>
#include <cinttypes>
#include <climits>
#include <cstddef>

// version
#define KNGIN_VERSION 0.10lf
#define KNGIN_VERSION_STR "0.1.0"

// for MSVC
#if defined(KNGIN_SYSTEM_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  pragma comment(lib,"ws2_32.lib")
#  pragma warning(disable: 4996)
#  pragma warning(disable: 4068)
#  pragma execution_character_set("utf-8")
#endif /* defined(KNGIN_SYSTEM_WIN32) */

// types
#if defined(KNGIN_SYSTEM_WIN32)
#  if defined(KNGIN_SYSTEM_WIN64)
  typedef __int64           ssize_t;
#  else
  typedef _W64 unsigned int ssize_t;
#  endif /* defined(KNGIN_SYSTEM_WIN64) */
#endif /* defined(KNGIN_SYSTEM_WIN32) */

// log
#define LINE static_cast<int>(__LINE__)
#if defined(KNGIN_USE_RELATIVE_LOG_PATH)
#  undef FILENAME
#  define FILENAME ((const char *)(__FILE__ + k::g_path_prefix_size))
#endif /* defined(KNGIN_USE_RELATIVE_LOG_PATH) */

// impl
#define declare_detail_impl_class(type) \
    namespace k::detail::impl { class type; }
#define typedef_detail_impl(src_type, dst_type)   \
    namespace k { typedef k::detail::impl::src_type dst_type; }

// detail
#define declare_detail_class(type) \
    namespace k::detail { class type; }
#define typedef_detail(src_type, dst_type)  \
    namespace k { typedef k::detail::src_type dst_type; }

namespace k {

// handle_t type
#if defined(KNGIN_SYSTEM_WIN32)
  typedef HANDLE handle_t;
#  define HANDLE_INVALID(h) (!h)
#  define HANDLE_VALID(h)   (h)
#  define INVALID_HANDLE    (nullptr)
#else
  typedef int handle_t;
#  define HANDLE_INVALID(h) ((h) < 0)
#  define HANDLE_VALID(h)   ((h) >= 0)
#  define INVALID_HANDLE    (-1)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

} /* namespace k */


#endif /* KNGIN_DEFINE_H */

