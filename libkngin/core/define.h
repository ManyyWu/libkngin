#ifndef _DEFINE_H_
#define _DEFINE_H_

#if __cplusplus < 201103L
//#error c++ version is too low, please build the project using c++11 standard.
#endif

#include <cassert>

#define __LOG_MUTEX

#define __NAMESPACE_BEGIN namespace k {
#define __NAMESPACE_END   };
#define __USING_NAMESPACE using namespace k;

#define INFINITE 0xFFFFFFFF

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#endif /* _DEFINE_H_ */