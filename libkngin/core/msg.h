#ifndef _MSG_H_
#define _MSG_H_

#include <cstdint>
#include "define.h"
#include "noncopyable.h"

#define MAX_MSG_SIZE      4096U
#define MAX_MSG_TYPE      0x7fffffffU
#define INVALID_MSG_TYPE  0x0U

__NAMESPACE_BEGIN

class msg : public noncopyable {
protected:
    msg ();

    virtual
    ~msg    ();

public:
    static msg *
    create  (uint32_t _type);

    static msg *
    create  (const uint8_t *_buf, uint32_t _size, uint32_t _type);

    static msg *
    create  (const msg *_msg);

    virtual void 
    release ();

protected:
    virtual bool
    process ();

protected:
    uint8_t *m_buf;

    uint32_t m_size;

    uint32_t m_type;
};

__NAMESPACE_END

#endif /* _MSG_H_ */