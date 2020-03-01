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
    out_buffer   () noexcept;

    out_buffer   (const void *arr, size_t size);

    out_buffer   (const out_buffer &buf) noexcept;

    out_buffer   (out_buffer &&buf) noexcept;

    ~out_buffer  () = default;

public:
    const uint8_t &
    operator []  (size_t idx) const noexcept
    { return m_arr[idx]; }
    const uint8_t &
    at           (size_t idx) const
    { check_readable(idx + 1); return m_arr[idx]; }
    const unsigned char *
    get          (size_t idx) const
    { check_readable(idx + 1); return &m_arr[idx]; }
    const unsigned char *
    begin        () const noexcept
    { return m_arr; }
    size_t
    size         () const noexcept
    { return m_size; }

public:
    out_buffer &
    peek_uint8   (uint8_t &val)  { val = read<uint8_t>(false); return *this; }
    out_buffer &
    peek_int8    (int8_t &val)   { val = read<int8_t>(false); return *this; }
    out_buffer &
    peek_uint16  (uint16_t &val) { val = read<uint16_t>(false); return *this; }
    out_buffer &
    peek_int16   (int16_t &val)  { val = read<int16_t>(false); return *this; }
    out_buffer &
    peek_uint32  (uint32_t &val) { val = read<uint32_t>(false); return *this; }
    out_buffer &
    peek_int32   (int32_t &val)  { val = read<int32_t>(false); return *this; }
    out_buffer &
    peek_uint64  (uint64_t &val) { val = read<uint64_t>(false); return *this; }
    out_buffer &
    peek_int64   (int64_t &val)  { val = read<int64_t>(false); return *this; }
    out_buffer &
    read_uint8   (uint8_t &val)  { val = read<uint8_t>(); return *this; }
    out_buffer &
    read_int8    (int8_t &val)   { val = read<int8_t>(); return *this; }
    out_buffer &
    read_uint16  (uint16_t &val) { val = read<uint16_t>(); return *this; }
    out_buffer &
    read_int16   (int16_t &val)  { val = read<int16_t>(); return *this; }
    out_buffer &
    read_uint32  (uint32_t &val) { val = read<uint32_t>(); return *this; }
    out_buffer &
    read_int32   (int32_t &val)  { val = read<int32_t>(); return *this; }
    out_buffer &
    read_uint64  (uint64_t &val) { val = read<uint64_t>(); return *this; }
    out_buffer &
    read_int64   (int64_t &val)  { val = read<int64_t>(); return *this; }
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
    read_bytes   (void * p, size_t n);

    void
    reset        (const void * buf, size_t size);

    void
    swap         (out_buffer &buf) noexcept;

    std::shared_ptr<out_buffer>
    clone        ();

public:
    std::string
    dump         ();

public:
    out_buffer &
    operator -=    (size_t size)
    { check_readable(size); m_size -= size; return *this; }

public:
    out_buffer &
    operator =     (const out_buffer &buf) noexcept
    { m_arr = buf.m_arr; m_size = buf.m_size; return *this; }

public:
    void
    check_readable (size_t n) const
    { if (m_size < n) throw k::exception("in_buffer::check_readable() - out of range"); }

protected:
    template <typename Type>
    Type
    read           (bool forward = true)
    {
        check_readable(sizeof(Type));
        Type val = *static_cast<const Type *>(static_cast<const void *>(m_arr));
        m_size -= forward ? sizeof(Type) : 0;
        m_arr += forward ? sizeof(Type) : 0;
        return val;
    }

private:
    const unsigned char * m_arr;

    size_t                m_size;
};

class msg_buffer {
public:
    msg_buffer  () noexcept
        : m_arr(nullptr), m_buf() {}

    msg_buffer  (uint8_arr_ptr &arr, size_t offset, size_t size)
        : m_arr(arr), m_buf(arr.get() + offset, size) {}
        
    msg_buffer  (const void *arr, size_t size)
        : m_arr(nullptr), m_buf(arr, size) {}
    // arr must be constant string or stack space that life cycle is longer than this

    msg_buffer  (const msg_buffer &buf)
        : m_arr(buf.m_arr), m_buf(buf.m_buf) {}

    msg_buffer  (msg_buffer &&buf) noexcept
        : m_arr(std::move(buf.m_arr)), m_buf(std::move(buf.m_buf)) {  }

    ~msg_buffer () = default;

public:
    uint8_arr_ptr &
    get         () noexcept
    { return m_arr; }
    const uint8_arr_ptr &
    get         () const noexcept
    { return m_arr; }
    out_buffer &
    buffer      ()
    { return m_buf; }
    const out_buffer &
    buffer      () const noexcept
    { return m_buf; }

public:
    msg_buffer &
    operator =  (const msg_buffer &buf)
    { m_arr = buf.m_arr; m_buf = buf.m_buf; return *this; }

private:
    uint8_arr_ptr m_arr;

    out_buffer    m_buf;
};

class in_buffer {
public:
    in_buffer    () noexcept;

    in_buffer    (void * arr, size_t size);

    in_buffer    (const in_buffer &buf) noexcept;

    in_buffer    (in_buffer &&buf) noexcept;

    ~in_buffer   () = default;

public:
    uint8_t &
    operator []  (size_t idx) noexcept
    { return m_arr[idx]; }
    uint8_t &
    at           (size_t idx)
    { check_readable(idx + 1); return m_arr[idx]; }
    unsigned char *
    get          (size_t idx)
    { check_readable(idx + 1); return &m_arr[idx]; }
    unsigned char *
    begin        () const noexcept
    { return m_arr; }
    size_t
    size         () const noexcept
    { return m_size; }
    size_t
    valid        () const noexcept
    { return m_valid; }
    size_t
    writeable    () const noexcept
    { return m_size - m_valid; }

public:
    in_buffer &
    write_uint8  (uint8_t  val) { return write<uint8_t>(val); }
    in_buffer &
    write_int8   (int8_t   val) { return write<int8_t>(val); }
    in_buffer &
    write_uint16 (uint16_t val) { return write<uint16_t>(val); }
    in_buffer &
    write_int16  (int16_t  val) { return write<int16_t>(val); }
    in_buffer &
    write_uint32 (uint32_t val) { return write<uint32_t>(val); }
    in_buffer &
    write_int32  (int32_t  val) { return write<int32_t>(val); }
    in_buffer &
    write_uint64 (uint64_t val) { return write<uint64_t>(val); }
    in_buffer &
    write_int64  (int64_t  val) { return write<int64_t>(val); }

public:
    in_buffer &
    write_bytes  (const void * p, size_t n);

    void
    reset        (void * buf, size_t size);

    void
    swap         (in_buffer &buf) noexcept;

public:
    std::string
    dump         ();

public:
    in_buffer &
    operator +=  (size_t size)
    { check_writeable(size); m_valid += size; return *this; }

public:

    in_buffer &
    operator =   (const in_buffer &buf) noexcept
    { m_arr = buf.m_arr; m_size = buf.m_size; m_valid = buf.m_valid; return *this; }

protected:
    void
    check_readable  (size_t n) const
    { if (m_valid < n) throw k::exception("in_buffer::check_readable() - out of range"); }

    void
    check_writeable (size_t n) const
    { if (m_size - m_valid < n) throw k::exception("in_buffer::check_writeable() - out of range"); }

    template <typename Type>
    in_buffer &
    write           (Type val)
    {
        check_writeable(sizeof(Type));
        *static_cast<Type *>(static_cast<void *>(m_arr + m_valid)) = val;
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
    receive         (size_t n);

    void
    send            (size_t n);

    size_t
    readable        ();

    size_t
    writeable       ();

    size_t
    size            ();

    std::vector<struct ::iovec> &
    to_iovec        ();

    void
    swap            (net_buffer &buf);

protected:
    void
    check_readable  (size_t n);

    void
    check_writeable (size_t n);

protected:
    buffer_list                 m_list;

    size_t                      m_rindex;

    size_t                      m_windex;

    std::vector<struct ::iovec> m_iovec;
};
*/
KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BUFFER_H */