#ifndef _MEMORY_H_
#define _MEMORY_H_

#ifdef _WIN32
#include <xtr1common>
#endif
#include <memory>
#include <type_traits>
#include "define.h"

KNGIN_NAMESPACE_K_BEGIN

#if (__cplusplus <= 201402)

template<typename _Tp>
inline std::shared_ptr<_Tp>
make_shared_array (size_t _num)
{
    return std::shared_ptr<_Tp>(new _Tp[_num],
                                std::default_delete<_Tp[]>());
}

#endif

KNGIN_NAMESPACE_K_END

#endif /* _MEMORY_H_ */