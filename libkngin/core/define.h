#ifndef _DEFINE_H_
#define _DEFINE_H_

#include <cassert>

#define __LOG_MUTEX

#define __NAMESPACE_BEGIN namespace k {
#define __NAMESPACE_END   };
#define __USING_NAMESPACE using namespace k;

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#endif /* _DEFINE_H_ */