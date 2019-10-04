#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <vector>
#include <memory>
#include "define.h"
#include "exception.h"
#include "error.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

typedef std::vector<uint8_t> uint8_arr;

class basic_buffer {
protected:
    basic_buffer   (const uint8_arr &_arr, size_t _reserve = 0);

    basic_buffer   (uint8_arr &&_arr, size_t _reserve = 0);

    ~basic_buffer  () = default;

public:
    const uint8_arr&
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

    size_t
    read_bytes     (uint8_t *_p, size_t _n);

    size_t
    write_uint8    (uint8_t _val);

    size_t
    write_int8     (int8_t _val);

    size_t
    write_uint16   (uint16_t _val);

    size_t
    write_int16    (int16_t _val);

    size_t
    write_uint32   (uint32_t _val);

    size_t
    write_int32    (int32_t _val);

    size_t
    write_uint64   (uint64_t _val);

    size_t
    write_int64    (int64_t _val);

    size_t
    write_bytes    (const uint8_t *_p, size_t _n);

public:
    size_t
    size           () const;

    void
    resize         (size_t _s);

    void
    shrink         ();

    size_t
    next           () const;

    void
    reset          (size_t _idx);

public:
    std::string &
    dump           ();

public:
    void
    operator =     (const basic_buffer &) = delete;

protected:
    bool
    readable       (size_t _n);

    bool
    writeable      (size_t _n);

protected:
    uint8_arr   m_arr;

    size_t      m_idx;

    std::string m_dump_str;
};


class buffer : public basic_buffer {
public:
    buffer         ();

    buffer         (size_t _reserve);

    buffer         (const uint8_arr &_arr, size_t _reserve = 0);

    buffer         (uint8_arr &&_arr, size_t _reserve = 0);

    buffer         (const buffer &_buf);

    buffer         (buffer &&_buf);

    ~buffer        () = default;

public:
    buffer &
    append         (const buffer &_buf);

    buffer &
    operator +     (const buffer &_buf);

    void
    operator =     (const buffer &) = delete;

    void
    swap           (buffer &_buf);
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */