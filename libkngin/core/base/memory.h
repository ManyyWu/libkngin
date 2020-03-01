#ifndef KNGIN_MEMORY_H
#define KNGIN_MEMORY_H

#ifdef _WIN32
#include <xtr1common>
#endif
#include <memory>
#include <type_traits>
#include "core/base/define.h"

KNGIN_NAMESPACE_K_BEGIN

#if (cplusplus <= 202000)

template<typename _Tp>
inline
std::shared_ptr<_Tp>
make_shared_array (size_t num)
{
    return std::shared_ptr<_Tp>(new _Tp[num], std::default_delete<_Tp[]>());
}

#endif

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_MEMORY_H */