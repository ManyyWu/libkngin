#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <memory>
//#include <xtr1common>
//c#include <type_traits>
#include "define.h"

__NAMESPACE_BEGIN
/*
#if __cplusplus < 201402L

template<class _Ty,
std::enable_if_t<std::is_array_v<_Ty> && std::extent_v<_Ty> == 0, int> = 0>
_NODISCARD inline std::shared_ptr<_Ty> 
make_shared(size_t _Size)
{	// make a shared_ptr 
    typedef remove_extent_t<_Ty> _Elem;
    return (shared_ptr<_Ty>(new _Elem[_Size]()));
}

#endif
*/
__NAMESPACE_END

#endif /* _MEMORY_H_ */