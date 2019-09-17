#ifndef _TASK_BASE_H_
#define _TASK_BASE_H_

#include <atomic>
#include "define.h"
#include "noncopyable.h"
#include "msg.h"

__NAMESPACE_BEGIN

class task_base {
public:
    task_base (msg *_msg)
        : m_msg (_msg)
    {
        assert(_msg);
    }

    virtual
    ~task_base ()
    {
        delete m_msg;
    }

public:
    virtual bool
    create     (int _type) = 0;

    virtual int
    process    () = 0;

protected:
    msg *m_msg;
};

__NAMESPACE_END

#endif /* _TASK_BASE_H_ */