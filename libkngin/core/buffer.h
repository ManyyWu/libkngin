#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include "core/define.h"
#include "core/noncopyable.h"
#include "core/exception.h"

KNGIN_NAMESPACE_K_BEGIN

class out_buffer : public noncopyable {
public:
    out_buffer   () KNGIN_NOEXCP;

    out_buffer   (const void *_arr, size_t _size);

    out_buffer   (out_buffer &&_buf) KNGIN_NOEXCP;

    ~out_buffer  () = default;

public:
    uint8_t
    operator []  (size_t _idx) const
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

public:
    std::string
    dump         ();

public:
    out_buffer &
    operator -=    (size_t _size)
    { check_readable(_size); m_size -= _size; return *this; }

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
        m_size -= _forward ? sizeof(Type) : 0;
        return *static_cast<const Type *>(static_cast<const void *>(m_arr));
    }

private:
    const unsigned char * m_arr;

    size_t                m_size;
};

class in_buffer : noncopyable {
public:
    in_buffer    () KNGIN_NOEXCP;

    in_buffer    (void * _arr, size_t _size);

    in_buffer    (in_buffer &&_buf) KNGIN_NOEXCP;

    ~in_buffer   () = default;

public:
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

protected:
    void
    check_readable  (size_t _n) const
    { if (m_size < _n) throw k::exception("in_buffer::check_readable() - out of range"); }

    void
    check_writeable (size_t _n) const
    { if (m_size - m_valid < _n) throw k::exception("in_buffer::check_writeable() - out of range"); }

    template <typename Type>
    in_buffer &
    write           (Type _val)
    {
        check_writeable(sizeof(Type));
        m_valid += sizeof(Type);
        *static_cast<Type *>(static_cast<void *>(m_arr)) = _val;
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