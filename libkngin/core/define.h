#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <string>
#include <memory>

#if __cplusplus < 201103L
//#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cassert>
#include <cstdint>
#include <climits>

// compile options
#define __LOG_MUTEX
//#define __DUMP_ERROR_MSG

// namespace
#define __NAMESPACE_BEGIN namespace k {
#define __NAMESPACE_END   };
#define __USING_NAMESPACE using namespace k;

// queue
#define QUEUE_MAX     (size_t)(SIZE_MAX)
#define QUEUE_MIN     (size_t)10

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

// auto ptr
typedef std::unique_ptr<uint8_t[]> uint8_uarr;
typedef std::shared_ptr<uint8_t>   uint8_sarr;
#define make_uint8_uarray std::make_unique<uint8_t[]>
#define make_uint8_sarray std::make_shared<uint8_t>

#endif /* _DEFINE_H_ */