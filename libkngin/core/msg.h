#ifndef _MSG_H_
#define _MSG_H_

#include <cstdint>
#include "define.h"
#include "copyable.h"

#define MAX_MSG_SIZE 4096
#define MAX_MSG_TYPE 0x7fffffff
#define INVALID_MSG_TYPE  0x0

__NAMESPACE_BEGIN

class msg : copyable {
protected:
#pragma pack(push, 1)
    struct msg_head {
        int version;
        int type;
        int size;
    };
#pragma pack(pop)

public:
    msg     () = delete;

    msg     (int _type);

    msg     (const uint8_t *_buf, int _size, int _type);

    msg     (const msg &_msg);

    msg     (msg &&_msg);

    ~msg    ();

public:
    virtual int
    process ();

private:
    virtual bool
    parse   ();

protected:
    uint8_t *m_buf;

    int      m_size;

    int      m_type;
};

__NAMESPACE_END

#endif /* _MSG_H_ */