#ifndef KNGIN_BUFFER_H
#define KNGIN_BUFFER_H

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include "core/base/define.h"
#include "core/base/noncopyable.h"
#include "core/base/exception.h"

KNGIN_NAMESPACE_K_BEGIN

typedef std::shared_ptr<char> uint8_arr_ptr;
// XXX: use k::make_shared_array() to create uint8_arr_ptr
// FIXME: use std::shared<char []> in c++ 20

class out_buffer {
public:
    out_buffer   () KNGIN_NOEXCP;

    out_buffer   (const void *_arr, size_t _size);

    out_buffer   (const out_buffer &_buf) KNGIN_NOEXCP;

    out_buffer   (out_buffer &&_buf) KNGIN_NOEXCP;

    ~out_buffer  () = default;

public:
    const uint8_t &
    operator []  (size_t _idx) const KNGIN_NOEXCP
    { return m_arr[_idx]; }
    const uint8_t &
    at           (size_t _idx) const
    { check_readable(_idx + 1); return m_arr[_idx]; }
    const unsigned char *
    get          (size_t _idx) const
    { check_readable(_idx + 1); return &m_arr[_idx]; }
    const unsigned char *
    begin        () const KNGIN_NOEXCP
    { return m_arr; }
    size_t
    size         () const KNGIN_NOEXCP
    { return m_size; }

public:
    out_buffer &
    peek_uint8   (uint8_t &_val)  { _val = read<uint8_t>(false); return *this; }
    out_buffer &
    peek_int8    (int8_t &_val)   { _val = read<int8_t>(false); return *this; }
    out_buffer &
    peek_uint16  (uint16_t &_val) { _val = read<uint16_t>(false); return *this; }
    out_buffer &
    peek_int16   (int16_t &_val)  { _val = read<int16_t>(false); return *this; }
    out_buffer &
    peek_uint32  (uint32_t &_val) { _val = read<uint32_t>(false); return *this; }
    out_buffer &
    peek_int32   (int32_t &_val)  { _val = read<int32_t>(false); return *this; }
    out_buffer &
    peek_uint64  (uint64_t &_val) { _val = read<uint64_t>(false); return *this; }
    out_buffer &
    peek_int64   (int64_t &_val)  { _val = read<int64_t>(false); return *this; }
    out_buffer &
    read_uint8   (uint8_t &_val)  { _val = read<uint8_t>(); return *this; }
    out_buffer &
    read_int8    (int8_t &_val)   { _val = read<int8_t>(); return *this; }
    out_buffer &
    read_uint16  (uint16_t &_val) { _val = read<uint16_t>(); return *this; }
    out_buffer &
    read_int16   (int16_t &_val)  { _val = read<int16_t>(); return *this; }
    out_buffer &
    read_uint32  (uint32_t &_val) { _val = read<uint32_t>(); return *this; }
    out_buffer &
    read_int32   (int32_t &_val)  { _val = read<int32_t>(); return *this; }
    out_buffer &
    read_uint64  (uint64_t &_val) { _val = read<uint64_t>(); return *this; }
    out_buffer &
    read_int64   (int64_t &_val)  { _val = read<int64_t>(); return *this; }
    uint8_t
    peek_uint8   () { return read<uint8_t>(false); }
    int8_t
    peek_int8    () { return read<int8_t>(false); }
    uint16_t
    peek_uint16  () { return read<uint16_t>(false); }
    int16_t
    peek_int16   () { return read<int16_t>(false); }
    uint32_t
    peek_uint32  () { return read<uint32_t>(false); }
    int32_t
    peek_int32   () { return read<int32_t>(false); }
    uint64_t
    peek_uint64  () { return read<uint64_t>(false); }
    int64_t
    peek_int64   () { return read<int64_t>(false); }
    uint8_t
    read_uint8   () { return read<uint8_t>(); }
    int8_t
    read_int8    () { return read<int8_t>(); }
    uint16_t
    read_uint16  () { return read<uint16_t>(); }
    int16_t
    read_int16   () { return read<int16_t>(); }
    uint32_t
    read_uint32  () { return read<uint32_t>(); }
    int32_t
    read_int32   () { return read<int32_t>(); }
    uint64_t
    read_uint64  () { return read<uint64_t>(); }
    int64_t
    read_int64   () { return read<int64_t>(); }

public:
    size_t
    read_bytes   (void * _p, size_t _n);

    void
    reset        (const void * _buf, size_t _size);

    void
    swap         (out_buffer &_buf) KNGIN_NOEXCP;

    std::shared_ptr<out_buffer>
    clone        ();

public:
    std::string
    dump         ();

public:
    out_buffer &
    operator -=    (size_t _size)
    { check_readable(_size); m_size -= _size; return *this; }

public:
    out_buffer &
    operator =     (const out_buffer &_buf) KNGIN_NOEXCP
    { m_arr = _buf.m_arr; m_size = _buf.m_size; return *this; }

public:
    void
    check_readable (size_t _n) const
    { if (m_size < _n) throw k::exception("in_buffer::check_readable() - out of range"); }

protected:
    template <typename Type>
    Type
    read           (bool _forward = true)
    {
        check_readable(sizeof(Type));
        Type _val = *static_cast<const Type *>(static_cast<const void *>(m_arr));
        m_size -= _forward ? sizeof(Type) : 0;
        m_arr += _forward ? sizeof(Type) : 0;
        return _val;
    }

private:
    const unsigned char * m_arr;

    size_t                m_size;
};

class msg_buffer {
public:
    msg_buffer  () KNGIN_NOEXCP
        : m_arr(nullptr), m_buf() {}

    msg_buffer  (uint8_arr_ptr &_arr, size_t _offset, size_t _size)
        : m_arr(_arr), m_buf(_arr.get() + _offset, _size) {}
        
    msg_buffer  (const void *_arr, size_t _size)
        : m_arr(nullptr), m_buf(_arr, _size) {}
    // _arr must be constant string or stack space that life cycle is longer than this

    msg_buffer  (const msg_buffer &_buf)
        : m_arr(_buf.m_arr), m_buf(_buf.m_buf) {}

    msg_buffer  (msg_buffer &&_buf) KNGIN_NOEXCP
        : m_arr(std::move(_buf.m_arr)), m_buf(std::move(_buf.m_buf)) {  }

    ~msg_buffer () = default;

public:
    uint8_arr_ptr &
    get         () KNGIN_NOEXCP
    { return m_arr; }
    const uint8_arr_ptr &
    get         () const KNGIN_NOEXCP
    { return m_arr; }
    out_buffer &
    buffer      ()
    { return m_buf; }
    const out_buffer &
    buffer      () const KNGIN_NOEXCP
    { return m_buf; }

public:
    msg_buffer &
    operator =  (const msg_buffer &_buf)
    { m_arr = _buf.m_arr; m_buf = _buf.m_buf; return *this; }

private:
    uint8_arr_ptr m_arr;

    out_buffer    m_buf;
};

class in_buffer {
public:
    in_buffer    () KNGIN_NOEXCP;

    in_buffer    (void * _arr, size_t _size);

    in_buffer    (const in_buffer &_buf) KNGIN_NOEXCP;

    in_buffer    (in_buffer &&_buf) KNGIN_NOEXCP;

    ~in_buffer   () = default;

public:
    uint8_t &
    operator []  (size_t _idx) KNGIN_NOEXCP
    { return m_arr[_idx]; }
    uint8_t &
    at           (size_t _idx)
    { check_readable(_idx + 1); return m_arr[_idx]; }
    unsigned char *
    get          (size_t _idx)
    { check_readable(_idx + 1); return &m_arr[_idx]; }
    unsigned char *
    begin        () const KNGIN_NOEXCP
    { return m_arr; }
    size_t
    size         () const KNGIN_NOEXCP
    { return m_size; }
    size_t
    valid        () const KNGIN_NOEXCP
    { return m_valid; }
    size_t
    writeable    () const KNGIN_NOEXCP
    { return m_size - m_valid; }

public:
    in_buffer &
    write_uint8  (uint8_t  _val) { return write<uint8_t>(_val); }
    in_buffer &
    write_int8   (int8_t   _val) { return write<int8_t>(_val); }
    in_buffer &
    write_uint16 (uint16_t _val) { return write<uint16_t>(_val); }
    in_buffer &
    write_int16  (int16_t  _val) { return write<int16_t>(_val); }
    in_buffer &
    write_uint32 (uint32_t _val) { return write<uint32_t>(_val); }
    in_buffer &
    write_int32  (int32_t  _val) { return write<int32_t>(_val); }
    in_buffer &
    write_uint64 (uint64_t _val) { return write<uint64_t>(_val); }
    in_buffer &
    write_int64  (int64_t  _val) { return write<int64_t>(_val); }

public:
    in_buffer &
    write_bytes  (const void * _p, size_t _n);

    void
    reset        (void * _buf, size_t _size);

    void
    swap         (in_buffer &_buf) KNGIN_NOEXCP;

public:
    std::string
    dump         ();

public:
    in_buffer &
    operator +=  (size_t _size)
    { check_writeable(_size); m_valid += _size; return *this; }

public:

    in_buffer &
    operator =   (const in_buffer &_buf) KNGIN_NOEXCP
    { m_arr = _buf.m_arr; m_size = _buf.m_size; m_valid = _buf.m_valid; return *this; }

protected:
    void
    check_readable  (size_t _n) const
    { if (m_valid < _n) throw k::exception("in_buffer::check_readable() - out of range"); }

    void
    check_writeable (size_t _n) const
    { if (m_size - m_valid < _n) throw k::exception("in_buffer::check_writeable() - out of range"); }

    template <typename Type>
    in_buffer &
    write           (Type _val)
    {
        check_writeable(sizeof(Type));
        *static_cast<Type *>(static_cast<void *>(m_arr + m_valid)) = _val;
        m_valid += sizeof(Type);
        return *this;
    }

private:
    unsigned char * m_arr;

    size_t          m_size;

    size_t          m_valid;
};

/*
class in_vector {
public:
    typedef std::list<in_buffer> buffer_vector;

public:
    in_vector      () = delete;

    in_vector      () : m_list(), m_rindex(0), m_windex(0), m_iovec() {}

    ~in_vector     () = default;

public:
    buffer_vector &
    vector          () { return m_list; }

    void
    receive         (size_t _n);

    void
    send            (size_t _n);

    size_t
    readable        ();

    size_t
    writeable       ();

    size_t
    size            ();

    std::vector<struct ::iovec> &
    to_iovec        ();

    void
    swap            (net_buffer &_buf);

protected:
    void
    check_readable  (size_t _n);

    void
    check_writeable (size_t _n);

protected:
    buffer_list                 m_list;

    size_t                      m_rindex;

    size_t                      m_windex;

    std::vector<struct ::iovec> m_iovec;
};
*/
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BUFFER_H */