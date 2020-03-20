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

// namespace
#define KNGIN_NAMESPACE_END                 };
#define KNGIN_NAMESPACE_K_BEGIN             namespace k {
#define KNGIN_NAMESPACE_K_END               KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_DETAIL_BEGIN        namespace detail {
#define KNGIN_NAMESPACE_DETAIL_END          KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_IMPL_BEGIN          namespace impl {
#define KNGIN_NAMESPACE_IMPL_END            KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_TCP_BEGIN           namespace tcp {
#define KNGIN_NAMESPACE_TCP_END             KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_UDP_BEGIN           namespace udp {
#define KNGIN_NAMESPACE_UDP_END             KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_K_DETAIL_BEGIN      namespace k::detail {
#define KNGIN_NAMESPACE_K_DETAIL_END        KNGIN_NAMESPACE_END
#define KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN namespace k::detail::impl {
#define KNGIN_NAMESPACE_K_DETAIL_IMPL_END   KNGIN_NAMESPACE_END

// log
#define LINE static_cast<int>(__LINE__)
#if defined(KNGIN_USE_RELATIVE_LOG_PATH)
#  undef FILENAME
#  define FILENAME ((const char *)(__FILE__ + k::g_path_prefix_size))
#endif /* defined(KNGIN_USE_RELATIVE_LOG_PATH) */

// impl
#define declare_detail_impl_class(type) \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_BEGIN \
      class type;                       \
    KNGIN_NAMESPACE_K_DETAIL_IMPL_END
#define typedef_detail_impl(src_type, dst_type)   \
    KNGIN_NAMESPACE_K_BEGIN                       \
      typedef k::detail::impl::src_type dst_type; \
    KNGIN_NAMESPACE_K_END

// detail
#define declare_detail_class(type) \
    KNGIN_NAMESPACE_K_DETAIL_BEGIN \
      class type;                  \
    KNGIN_NAMESPACE_K_DETAIL_END
#define typedef_detail(src_type, dst_type)  \
    KNGIN_NAMESPACE_K_BEGIN                 \
      typedef k::detail::src_type dst_type; \
    KNGIN_NAMESPACE_K_END


#endif /* KNGIN_DEFINE_H */

