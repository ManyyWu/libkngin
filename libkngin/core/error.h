#ifndef _ERROR_H_
#define _ERROR_H_

#include <system_error>
#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

enum APPLICATION_ERROR_CODE {
    APPE_SUCCESS = 0,   // success
    APPE_NOMEM   = 1,   // out of memory

    APPE_UNKNOWN = 255, // unknown error
};

KNGIN_NAMESPACE_K_END

#endif /* _ERROR_H_ */