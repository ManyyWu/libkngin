#ifndef KNGIN_COMMON_H
#define KNGIN_COMMON_H

#include "kngin/core/define.h"
#include <cassert>

KNGIN_NAMESPACE_K_BEGIN

#define throw_system_error(what, ec) throw k::system_error((what), (ec))

#define throw_exception(what) throw k::exception((what))

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_COMMON_H */
