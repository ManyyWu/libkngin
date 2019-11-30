#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include "core/define.h"
#include "core/exception.h"

KNGIN_NAMESPACE_K_BEGIN

class out_buffer {
public:
    typedef int8_t *        uint8_arr;

    typedef uint8_t *       int8_arr;

    typedef const int8_t *  cuint8_arr;

    typedef const uint8_t * cint8_arr;

public:
    out_buffer   () KNGIN_NOEXP;

    explicit
    out_buffer   (cuint8_arr _arr, size_t _size) KNGIN_EXP;

    explicit
    out_buffer   (cint8_arr _arr, size_t _size) KNGIN_EXP;

    out_buffer   (out_buffer &&_buf) KNGIN_NOEXP;

    ~out_buffer  () = delete;

public:
    uint8_t
    operator []  (size_t _idx) const KNGIN_NOEXP
    { check_readable(_idx + 1); return m_arr[_idx]; }
    cuint8_arr
    get          (size_t _idx) const KNGIN_NOEXP
    { check_readable(_idx + 1); return &m_arr[_idx]; }
    cuint8_arr
    begin        () const KNGIN_NOEXP
    { return m_arr; }
    size_t
    size         () const KNGIN_NOEXP
    { return m_size; }

public:
    uint8_t
    peek_uint8   () KNGIN_EXP { return read<uint8_t>(false); }
    int8_t
    peek_int8    () KNGIN_EXP { return read<int8_t>(false); }
    uint16_t
    peek_uint16  () KNGIN_EXP { return read<uint16_t>(false); }
    int16_t
    peek_int16   () KNGIN_EXP { return read<int16_t>(false); }
    uint32_t
    peek_uint32  () KNGIN_EXP { return read<uint32_t>(false); }
    int32_t
    peek_int32   () KNGIN_EXP { return read<int32_t>(false); }
    uint64_t
    peek_uint64  () KNGIN_EXP { return read<uint64_t>(false); }
    int64_t
    peek_int64   () KNGIN_EXP { return read<int64_t>(false); }
    uint8_t
    read_uint8   () KNGIN_EXP { return read<uint8_t>(); }
    int8_t
    read_int8    () KNGIN_EXP { return read<int8_t>(); }
    uint16_t
    read_uint16  () KNGIN_EXP { return read<uint16_t>(); }
    int16_t
    read_int16   () KNGIN_EXP { return read<int16_t>(); }
    uint32_t
    read_uint32  () KNGIN_EXP { return read<uint32_t>(); }
    int32_t
    read_int32   () KNGIN_EXP { return read<int32_t>(); }
    uint64_t
    read_uint64  () KNGIN_EXP { return read<uint64_t>(); }
    int64_t
    read_int64   () KNGIN_EXP { return read<int64_t>(); }

public:
    size_t
    read_bytes   (uint8_arr _p, size_t _n) KNGIN_EXP;

    void
    reset        (cuint8_arr _buf, size_t _size) KNGIN_EXP;

    void
    reset        (cint8_arr _buf, size_t _size) KNGIN_EXP;

    void
    swap         (out_buffer &_buf) KNGIN_NOEXP;

public:
    std::string
    dump         () KNGIN_EXP;

public:
    out_buffer &
    operator =   (out_buffer &) = delete;

    out_buffer &
    operator -=  (size_t _size) KNGIN_EXP
    { check_readable(_size); m_size -= _size; return *this; }

protected:
    void
    check_readable  (size_t _n) const KNGIN_EXP
    { if (m_size < _n) throw k::exception("in_buffer::check_readable() - out of range"); }

    template <typename Type>
    Type
    read            (bool _forward = true) KNGIN_EXP
    {
        check_readable(sizeof(Type));
        m_size -= _forward ? sizeof(Type) : 0;
        return *static_cast<const Type *>(static_cast<const void *>(m_arr));
    }

private:
    cuint8_arr m_arr;

    size_t     m_size;
};

class in_buffer {
public:
    typedef int8_t *  uint8_arr;

    typedef uint8_t * int8_arr;

public:
    in_buffer    () KNGIN_NOEXP;

    explicit
    in_buffer    (uint8_arr _arr, size_t _size) KNGIN_EXP;

    explicit
    in_buffer    (int8_arr _arr, size_t _size) KNGIN_EXP;

    in_buffer    (in_buffer &&_buf) KNGIN_NOEXP;

    ~in_buffer   () = delete;

public:
    uint8_arr
    get          (size_t _idx) KNGIN_EXP
    { check_readable(_idx + 1); return &m_arr[_idx]; }
    uint8_arr
    begin        () const KNGIN_NOEXP
    { return m_arr; }
    size_t
    size         () const KNGIN_NOEXP
    { return m_size; }
    size_t
    valid        () const KNGIN_NOEXP
    { return m_valid; }
    size_t
    writeable    () const KNGIN_NOEXP
    { return m_size - m_valid; }

public:
    void
    write_uint8  (uint8_t  _val) KNGIN_EXP { write<uint8_t>(_val); }
    void
    write_int8   (int8_t   _val) KNGIN_EXP { write<int8_t>(_val); }
    void
    write_uint16 (uint16_t _val) KNGIN_EXP { write<uint16_t>(_val); }
    void
    write_int16  (int16_t  _val) KNGIN_EXP { write<int16_t>(_val); }
    void
    write_uint32 (uint32_t _val) KNGIN_EXP { write<uint32_t>(_val); }
    void
    write_int32  (int32_t  _val) KNGIN_EXP { write<int32_t>(_val); }
    void
    write_uint64 (uint64_t _val) KNGIN_EXP { write<uint64_t>(_val); }
    void
    write_int64  (int64_t  _val) KNGIN_EXP { write<int64_t>(_val); }

public:
    size_t
    write_bytes  (uint8_arr _p, size_t _n) KNGIN_EXP;

    void
    reset        (uint8_arr _buf, size_t _size) KNGIN_EXP;

    void
    reset        (int8_arr _buf, size_t _size) KNGIN_EXP;

    void
    swap         (in_buffer &_buf) KNGIN_NOEXP;

public:
    std::string
    dump         () KNGIN_EXP;

public:
    in_buffer &
    operator =   (in_buffer &) = delete;

    in_buffer &
    operator +=  (size_t _size) KNGIN_EXP
    { check_writeable(_size); m_valid += _size; return *this; }

protected:
    void
    check_readable  (size_t _n) const KNGIN_EXP
    { if (m_size < _n) throw k::exception("in_buffer::check_readable() - out of range"); }

    void
    check_writeable (size_t _n) const KNGIN_EXP
    { if (m_size - m_valid < _n) throw k::exception("in_buffer::check_writeable() - out of range"); }

    template <typename Type>
    void
    write (Type _val) KNGIN_EXP
    {
        check_writeable(sizeof(Type));
        m_valid += sizeof(Type);
        *static_cast<Type *>(static_cast<void *>(m_arr)) = _val;
    }

private:
    uint8_arr m_arr;

    size_t    m_size;

    size_t    m_valid;
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

    std::vector<struct iovec> &
    to_iovec        ();

    void
    swap            (net_buffer &_buf);

protected:
    void
    check_readable  (size_t _n);

    void
    check_writeable (size_t _n);

protected:
    buffer_list               m_list;

    size_t                    m_rindex;

    size_t                    m_windex;

    std::vector<struct iovec> m_iovec;
};
*/
KNGIN_NAMESPACE_K_END

#endif /* _BUFFER_H_ */