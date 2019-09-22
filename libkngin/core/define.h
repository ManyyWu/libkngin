#ifndef _DEFINE_H_
#define _DEFINE_H_

#if __cplusplus < 201103L
//#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cassert>
#include <cstdint>

#define __LOG_MUTEX

#define __NAMESPACE_BEGIN namespace k {
#define __NAMESPACE_END   };
#define __USING_NAMESPACE using namespace k;

#define QUEUE_MAX     (size_t)(SIZE_MAX)
#define QUEUE_MIN     (size_t)10

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#endif /* _DEFINE_H_ */