#ifndef _MSG_H_
#define _MSG_H_

#include <cstdint>
#include "define.h"
#include "common.h"
#include "noncopyable.h"

#define MAX_MSG_SIZE      4096U
#define MAX_MSG           0x7fffffffU
#define INVALID_MSG       0x0U
#define MSG_BEGIN         0x1U
#define MSG_END           MAX_MSG
#define __msg_valid(_msg) ((_msg) >= MSG_BEGIN && (_msg) <= MSG_END)
#define __both(_a, _b)    (((_a) && (_b)) || (!(_a) || !(_b)))

__NAMESPACE_BEGIN

class task_base;
class msg : public noncopyable {
public:
    explicit
    msg (task_base *task);

protected:
    virtual
    ~msg    ();

public:
    virtual bool
    create  (uint32_t _type);

    virtual bool
    create  (const uint8_t *_buf, uint32_t _size, uint32_t _type);

    virtual bool
    create (uint8_t **_buf, uint32_t _size, uint32_t _type);

    virtual bool
    create  (const msg *_msg);

    virtual bool
    create  (msg **_msg);

    virtual void
    release ();

public:
    virtual bool
    process ();

    const uint8_t *
    buf     ();

    uint32_t
    size    ();

    uint32_t
    type    ();

    task_base *
    task    ();

    virtual void
    dump    ();

protected:
    uint8_t *  m_buf;

    uint32_t   m_size;

    uint32_t   m_type;

    task_base *m_task;
};

__NAMESPACE_END

#endif /* _MSG_H_ */