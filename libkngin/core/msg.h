#ifndef _MSG_H_
#define _MSG_H_

#include <cstdint>
#include "define.h"
#include "copyable.h"

#define MAX_MSG_SIZE      4096U
#define MAX_MSG_TYPE      0x7fffffffU
#define INVALID_MSG_TYPE  0x0U

__NAMESPACE_BEGIN

class msg : copyable {
public:
    msg     ();

    virtual
    ~msg    ();

public:
    virtual bool
    create  (uint32_t _type);

    virtual bool
    create  (const uint8_t *_buf, uint32_t _size, uint32_t _type);

    virtual bool
    create  (const msg &_msg);

    virtual bool
    create  (msg &&_msg);

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