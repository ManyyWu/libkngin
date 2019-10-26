#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <algorithm>
#include <string>
#include <vector>
#include <memory>
#include "define.h"
#include "exception.h"
#include "error.h"
#include "buffer.h"

__NAMESPACE_BEGIN

class basic_buffer {
public:
    typedef std::vector<uint8_t> uint8_arr;

public:
    basic_buffer  () = delete;

    basic_buffer  (const basic_buffer &) = delete;

protected:
    explicit
    basic_buffer  (const uint8_arr &_arr, size_t _reserve = 0) : m_arr(_arr), m_widx(0), m_ridx(0)
    { m_arr.resize(std::max(_reserve, _arr.size())); }

    explicit
    basic_buffer  (uint8_arr &&_arr, size_t _reserve = 0) : m_arr(std::move(_arr)), m_widx(0), m_ridx(0)
    { m_arr.resize(std::max(_reserve, m_arr.size())); }

    ~basic_buffer () = default;

public:
    uint8_arr &
    get           ()              { return m_arr; }

    const uint8_arr &
    get           () const        { return m_arr; }

    const uint8_t *
    rindex        () const        { return &m_arr[m_ridx]; }

    uint8_t *
    windex        ()              { return &m_arr[m_widx]; }

public:
    uint8_t
    peek_uint8   ()              { return read<uint8_t>(false); }
    int8_t
    peek_int8    ()              { return read<int8_t>(false); }
    uint16_t
    peek_uint16  ()              { return read<uint16_t>(false); }
    int16_t
    peek_int16   ()              { return read<int16_t>(false); }
    uint32_t
    peek_uint32  ()              { return read<uint32_t>(false); }
    int32_t
    peek_int32   ()              { return read<int32_t>(false); }
    uint64_t
    peek_uint64  ()              { return read<uint64_t>(false); }
    int64_t
    peek_int64   ()              { return read<int64_t>(false); }
    uint8_t
    read_uint8   ()              { return read<uint8_t>(); }
    int8_t
    read_int8    ()              { return read<int8_t>(); }
    uint16_t
    read_uint16  ()              { return read<uint16_t>(); }
    int16_t
    read_int16   ()              { return read<int16_t>(); }
    uint32_t
    read_uint32  ()              { return read<uint32_t>(); }
    int32_t
    read_int32   ()              { return read<int32_t>(); }
    uint64_t
    read_uint64  ()              { return read<uint64_t>(); }
    int64_t
    read_int64   ()              { return read<int64_t>(); }
    void
    write_uint8  (uint8_t  _val) { write<uint8_t>(_val); }
    void
    write_int8   (int8_t   _val) { write<int8_t>(_val); }
    void
    write_uint16 (uint16_t _val) { write<uint16_t>(_val); }
    void
    write_int16  (int16_t  _val) { write<int16_t>(_val); }
    void
    write_uint32 (uint32_t _val) { write<uint32_t>(_val); }
    void
    write_int32  (int32_t  _val) { write<int32_t>(_val); }
    void
    write_uint64 (uint64_t _val) { write<uint64_t>(_val); }
    void
    write_int64  (int64_t  _val) { write<int64_t>(_val); }

public:
    size_t
    read_bytes  (uint8_t *_p, size_t _n);

    size_t
    write_bytes (const uint8_t *_p, size_t _n);

public:
    size_t
    size      () const          { return m_arr.size(); }

    void
    resize    (size_t _s)       { m_arr.resize(_s); m_arr.shrink_to_fit(); }

    void
    rforward   (size_t _s)      { m_ridx = std::min(m_arr.size(), _s + m_ridx); }

    void
    wforward   (size_t _s)      { m_widx = std::min(m_arr.size(), _s + m_widx); }

    void
    rback (size_t _s)           { m_ridx = std::max((size_t)0, _s - m_ridx); }

    void
    wback (size_t _s)           { m_widx = std::max((size_t)0, _s - m_widx); }

    void
    shrink    ()                { m_arr.shrink_to_fit(); }

    size_t
    readable  () const          { return (m_arr.size() - m_ridx); }

    size_t
    writeable () const          { return (m_arr.size() - m_widx); }

    bool
    readable  (size_t _n) const { return (m_arr.size() - m_ridx >= _n); }

    bool
    writeable (size_t _n) const { return (m_arr.size() - m_widx >= _n); }

    size_t
    rreset     (size_t _idx);

    size_t
    wreset     (size_t _idx);

public:
    std::string &
    dump          ();

protected:
    void
    check_readable  (size_t _n) const;

    void
    check_writeable (size_t _n) const;

    template <typename __T> __T
    read (bool _forward = true)
    {
        check_readable(sizeof(__T));
        __T _val = *(__T *)(m_arr.data() + m_ridx);
        m_ridx += _forward ? sizeof(__T) : 0;
        return _val;
    }

    template <typename __T> void
    write (__T _val)
    {
        check_writeable(sizeof(__T));
        *(__T*)(m_arr.data() + m_widx) = _val;
        m_widx += sizeof(__T);
    }

protected:
    uint8_arr   m_arr;

    size_t      m_widx;

    size_t      m_ridx;

    std::string m_dump_str;
};

class buffer : public basic_buffer {
public:
    buffer     () : basic_buffer(uint8_arr()) {}

    explicit
    buffer     (size_t _reserve) : basic_buffer(uint8_arr(), _reserve) {}

    explicit
    buffer     (const uint8_arr &_arr, size_t _reserve = 0) : basic_buffer(_arr, _reserve) {}

    explicit
    buffer     (uint8_arr &&_arr, size_t _reserve = 0) : basic_buffer(std::move(_arr), _reserve) {}

    buffer     (const buffer &_buf) : basic_buffer(_buf.m_arr, _buf.m_arr.size()) {}

    explicit
    buffer     (buffer &&_buf) : basic_buffer(std::move(_buf.m_arr), _buf.m_arr.size()) {}

    ~buffer    () = default;

public:
    buffer &
    append     (const buffer &_buf);

    buffer &
    operator + (const buffer &_buf) { return append(_buf); }

    void
    swap       (buffer &_buf) { std::swap(m_arr, _buf.m_arr); std::swap(m_widx, _buf.m_widx); }
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */