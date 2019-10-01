#include "exception.h"
#include <memory>
#include <cstring>
#include "define.h"
#include "common.h"
#include "logfile.h"
#include "buffer.h"
#include "memory.h"

__NAMESPACE_BEGIN

basic_buffer::basic_buffer (const uint8_uarr &_arr, size_t _s)
    : m_arr(make_uint8_uarray(_s)), m_size(_s), m_index(0)
{
    assert(_arr && _s && _arr.get());
    ::memcpy(m_arr.get(), _arr.get(), _s);
}

basic_buffer::basic_buffer (uint8_uarr &&_arr, size_t _s)
    : m_arr(std::move(_arr)),
      m_size(_s),
      m_index(0)
{
    assert(m_arr && m_size && m_arr.get());
}

const uint8_uarr &
basic_buffer::get ()
{
    return m_arr;
}

uint8_t
basic_buffer::read_uint8 ()
{
    if (m_size - m_index < 1)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    return m_arr[m_index++];
}

int8_t
basic_buffer::read_int8 ()
{
    if (m_size - m_index < 1)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    return (int8_t)m_arr[m_index++];
}

uint16_t
basic_buffer::read_uint16 ()
{
    if (m_size - m_index < 2)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint16_t _ret = *(uint16_t *)&m_arr[m_index];
    m_index += 2;
    return _ret;
}

int16_t
basic_buffer::read_int16 ()
{
    if (m_size - m_index < 2)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int16_t _ret = *(int16_t *)&m_arr[m_index];
    m_index += 2;
    return _ret;
}

uint32_t
basic_buffer::read_uint32 ()
{
    if (m_size - m_index < 4)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint32_t _ret = *(uint32_t *)&m_arr[m_index];
    m_index += 4;
    return _ret;
}

int32_t
basic_buffer::read_int32 ()
{
    if (m_size - m_index < 4)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int32_t _ret = *(int32_t *)&m_arr[m_index];
    m_index += 4;
    return _ret;
}

uint64_t
basic_buffer::read_uint64 ()
{
    if (m_size - m_index < 8)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint64_t _ret = *(uint64_t *)&m_arr[m_index];
    m_index += 8;
    return _ret;
}

int64_t
basic_buffer::read_int64 ()
{
    if (m_size - m_index < 8)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int64_t _ret = *(int64_t *)&m_arr[m_index];
    m_index += 8;
    return _ret;
}

void
basic_buffer::read_bytes (uint8_t *_p, size_t _n)
{
    kassert(_p);
    if (m_size - m_index < _n)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    ::memcpy(_p, &m_arr[m_index], _n);
    m_index += _n;
}

void
basic_buffer::write_uint8 (uint8_t _val)
{
    if (m_size - m_index < 1)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    m_arr[m_index++] = _val;
}

void
basic_buffer::write_int8 (int8_t _val)
{
    if (m_size - m_index < 1)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    m_arr[m_index++] = (uint8_t)_val;
}

void
basic_buffer::write_uint16 (uint16_t _val)
{
    if (m_size - m_index < 2)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint16_t *)&m_arr[m_index] = (uint16_t)_val;
    m_index += 2;
}

void
basic_buffer::write_int16 (int16_t _val)
{
    if (m_size - m_index < 2)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int16_t *)&m_arr[m_index] = (int16_t)_val;
    m_index += 2;
}

void
basic_buffer::write_uint32 (uint32_t _val)
{
    if (m_size - m_index < 4)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint32_t *)&m_arr[m_index] = (uint32_t)_val;
    m_index += 4;
}

void
basic_buffer::write_int32 (int32_t _val)
{
    if (m_size - m_index < 4)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int32_t *)&m_arr[m_index] = (int32_t)_val;
    m_index += 4;
}

void
basic_buffer::write_uint64 (uint64_t _val)
{
    if (m_size - m_index < 8)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint64_t *)&m_arr[m_index] = (uint64_t)_val;
    m_index += 8;
}

void
basic_buffer::write_int64 (int64_t _val)
{
    if (m_size - m_index < 8)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int64_t *)&m_arr[m_index] = (int64_t)_val;
    m_index += 8;
}

void
basic_buffer::write_bytes (const uint8_t *_p, size_t _n)
{
    kassert(_p);
    if (m_size - m_index < _n)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    ::memcpy(&m_arr[m_index], _p, _n);
    m_index += _n;
}

size_t
basic_buffer::size () const
{
    return m_size;
}

size_t
basic_buffer::next () const
{
    return (m_size - m_index);
}

void
basic_buffer::reset (size_t _idx)
{
    assert(_idx < m_size);
    m_index = std::min(_idx, m_size - 1);
}

void
basic_buffer::dump (std::string &_str)
{
    _str.clear();
    _str.reserve(m_size * 2 + 1);
    for (int i = 0; i < m_size; ++i) {
        char _tmp[3] = {0};
        snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _str += _tmp;
    }
}

buffer::buffer (size_t _s)
    : basic_buffer(std::move(make_uint8_uarray(_s)), _s)
{
}

buffer::buffer (const uint8_uarr &_arr, size_t _s)
    : basic_buffer(_arr, _s)
{
}

buffer::buffer (uint8_uarr &&_arr, size_t _s)
    : basic_buffer(std::move(_arr), _s)
{
}

buffer::buffer (const buffer &_buf)
    : basic_buffer(_buf.m_arr, _buf.m_size)
{
}

buffer::buffer (buffer &&_buf)
    : basic_buffer(std::move(_buf.m_arr), _buf.m_size)
{
}

buffer::~buffer ()
{
}

void
buffer::swap (buffer &_buf)
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
    std::swap(m_index, _buf.m_index);
}

__NAMESPACE_END
