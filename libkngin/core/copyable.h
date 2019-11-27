#ifndef _COPYABLE_H_
#define _COPYABLE_H_

#include "core/define.h"

KNGIN_NAMESPACE_K_BEGIN

class copyable {
protected:
    copyable  () = default;

    ~copyable () = default;
};

KNGIN_NAMESPACE_K_END

#endif /* _COPYABLE_H_ */