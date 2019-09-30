#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <exception>
#include <cstdint>
#include <memory>
#include "define.h"
#include "error.h"
#include "copyable.h"

__NAMESPACE_BEGIN

class buffer : public copyable {
public:
    buffer         () __NOEXP;

    buffer         (uint8_t *_buf, size_t _n) __NOEXP;

    buffer         (uint8_t **_buf, size_t _n) __NOEXP;

    buffer         (buffer &_buf) __NOEXP;

    buffer         (buffer &&_buf) __NOEXP;

    ~buffer        () __NOEXP;

public:
    uint8_t
    read_uint8     () __EXP;

    int8_t
    read_int8      () __EXP;

    uint16_t
    read_uint16    () __EXP;

    int16_t
    read_int16     () __EXP;

    uint32_t
    read_uint32    () __EXP;

    int32_t
    read_int32     () __EXP;

    uint64_t
    read_uint64    () __EXP;

    int64_t
    read_int64     () __EXP;

    sys_err
    read_bytes     (uint8_t *_p, size_t _n) __EXP;

public:
    size_t
    size           ();

    size_t
    next           ();

    void
    clear          ();

    void
    swap           (buffer &_buf);

protected:
    uint8_uptr m_buf;

    size_t     m_max;
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */