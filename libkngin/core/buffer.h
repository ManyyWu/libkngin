#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <cstdint>
#include <memory>
#include "define.h"
#include "exception.h"
#include "error.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

class basic_buffer : noncopyable {
protected:
    basic_buffer   (uint8_uarr &_arr, size_t _s) __EXP;

    basic_buffer   (uint8_uarr &&_arr, size_t _s) __EXP;

    ~basic_buffer  () __NOEXP = default;

public:
    const uint8_uarr &
    get            () __EXP;

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

    void
    read_bytes     (uint8_t *_p, size_t _n) __EXP;

    void
    write_uint8    (uint8_t _val) __EXP;

    void
    write_int8     (int8_t _val) __EXP;

    void
    write_uint16   (uint16_t _val) __EXP;

    void
    write_int16    (int16_t _val) __EXP;

    void
    write_uint32   (uint32_t _val) __EXP;

    void
    write_int32    (int32_t _val) __EXP;

    void
    write_uint64   (uint64_t _val) __EXP;

    void
    write_int64    (int64_t _val) __EXP;

    void
    write_bytes    (const uint8_t *_p, size_t _n) __EXP;

public:
    size_t
    size           () const __NOEXP;

    size_t
    next           () const __NOEXP;

    void
    reset          (size_t _idx) __NOEXP;

public:
    void
    dump           (std::string &_str) __EXP;

protected:
    uint8_uarr m_arr;

    size_t     m_size;

    size_t     m_index;
};


class buffer : public basic_buffer {
public:
    buffer         (size_t _s) __EXP;

    buffer         (uint8_uarr &_arr, size_t _s) __EXP;

    buffer         (uint8_uarr &&_arr, size_t _s) __EXP;

    buffer         (buffer &_buf) __EXP;

    buffer         (buffer &&_buf) __EXP;

    ~buffer        () __NOEXP;

public:

    void
    swap           (buffer &_buf) __NOEXP;
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */