#ifndef _MEMORY_H_
#define _MEMORY_H_

#ifdef _WIN32
#include <xtr1common>
#endif
#include <memory>
#include <type_traits>
#include "define.h"

__NAMESPACE_BEGIN

#if __cplusplus <= 201402

template<typename _Tp>
inline std::shared_ptr<_Tp>
make_shared_array (size_t __num)
{
    return std::shared_ptr<_Tp>(new _Tp[__num],
                                std::default_delete<_Tp[]>());
}

#endif

__NAMESPACE_END

#endif /* _MEMORY_H_ */