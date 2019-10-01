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

class basic_buffer {
protected:
    basic_buffer   (const uint8_uarr &_arr, size_t _s);

    basic_buffer   (uint8_uarr &&_arr, size_t _s);

    ~basic_buffer  () = default;

public:
    const uint8_uarr &
    get            ();

public:
    uint8_t
    read_uint8     ();

    int8_t
    read_int8      ();

    uint16_t
    read_uint16    ();

    int16_t
    read_int16     ();

    uint32_t
    read_uint32    ();

    int32_t
    read_int32     ();

    uint64_t
    read_uint64    ();

    int64_t
    read_int64     ();

    void
    read_bytes     (uint8_t *_p, size_t _n);

    void
    write_uint8    (uint8_t _val);

    void
    write_int8     (int8_t _val);

    void
    write_uint16   (uint16_t _val);

    void
    write_int16    (int16_t _val);

    void
    write_uint32   (uint32_t _val);

    void
    write_int32    (int32_t _val);

    void
    write_uint64   (uint64_t _val);

    void
    write_int64    (int64_t _val);

    void
    write_bytes    (const uint8_t *_p, size_t _n);

public:
    size_t
    size           () const;

    size_t
    next           () const;

    void
    reset          (size_t _idx);

public:
    void
    dump           (std::string &_str);

public:
    void
    operator =     (const basic_buffer &) = delete;

protected:
    uint8_uarr m_arr;

    size_t     m_size;

    size_t     m_index;
};


class buffer : public basic_buffer {
public:
    buffer         (size_t _s);

    buffer         (const uint8_uarr &_arr, size_t _s);

    buffer         (uint8_uarr &&_arr, size_t _s);

    buffer         (const buffer &_buf);

    buffer         (buffer &&_buf);

    ~buffer        ();

public:
    void
    operator =     (const buffer &) = delete;

    void
    swap           (buffer &_buf);
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */