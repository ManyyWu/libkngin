#include <memory>
#include <string>
#include <cstring>
#include "define.h"
#include "exception.h"
#include "common.h"
#include "logfile.h"
#include "buffer.h"
#include "memory.h"

__NAMESPACE_BEGIN

basic_buffer::basic_buffer (const uint8_arr &_arr, size_t _reserve /* = 0 */)
    : m_arr(_arr), m_idx(0)
{
    m_arr.resize(std::max(_reserve, _arr.size()));
}

basic_buffer::basic_buffer (uint8_arr &&_arr, size_t _reserve /* = 0 */)
    : m_arr(std::move(_arr)), m_idx(0)
{
    m_arr.resize(std::max(_reserve, m_arr.size()));
}

const uint8_arr&
basic_buffer::get ()
{
    return m_arr;
}

uint8_t
basic_buffer::read_uint8 ()
{
    if (m_arr.size() - m_idx < sizeof(uint8_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    return (uint8_t)m_arr[m_idx++];
}

int8_t
basic_buffer::read_int8 ()
{
    if (m_arr.size() - m_idx < sizeof(int8_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    return m_arr[m_idx++];
}

uint16_t
basic_buffer::read_uint16 ()
{
    if (m_arr.size() - m_idx < sizeof(uint16_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint16_t _ret = *(uint16_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(uint16_t);
    return _ret;
}

int16_t
basic_buffer::read_int16 ()
{
    if (m_arr.size() - m_idx < sizeof(int16_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int16_t _ret = *(int16_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(int16_t);
    return _ret;
}

uint32_t
basic_buffer::read_uint32 ()
{
    if (m_arr.size() - m_idx < sizeof(uint32_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint32_t _ret = *(uint32_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(uint32_t);
    return _ret;
}

int32_t
basic_buffer::read_int32 ()
{
    if (m_arr.size() - m_idx < sizeof(int32_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int32_t _ret = *(int32_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(int32_t);
    return _ret;
}

uint64_t
basic_buffer::read_uint64 ()
{
    if (m_arr.size() - m_idx < sizeof(uint64_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    uint64_t _ret = *(uint64_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(uint64_t);
    return _ret;
}

int64_t
basic_buffer::read_int64 ()
{
    if (m_arr.size() - m_idx < sizeof(int64_t))
        throw logic_exception("no enough data", __FILE__, __LINE__);
    int64_t _ret = *(int64_t *)(m_arr.data() + m_idx);
    m_idx += sizeof(int64_t);
    return _ret;
}

size_t
basic_buffer::read_bytes (uint8_t *_p, size_t _n)
{
    kassert(_p);
    if (m_arr.size() - m_idx < _n)
        throw logic_exception("no enough data", __FILE__, __LINE__);
    ::memcpy(_p, m_arr.data() + m_idx, _n);
    m_idx += _n;
    return _n;
}

size_t
basic_buffer::write_uint8 (uint8_t _val)
{
    if (m_arr.size() - m_idx < sizeof(uint8_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    m_arr[m_idx++] = (uint8_t)_val;
    return sizeof(uint8_t);
}

size_t
basic_buffer::write_int8 (int8_t _val)
{
    if (m_arr.size() - m_idx < sizeof(uint8_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    m_arr[m_idx++] = _val;
    return sizeof(uint8_t);
}

size_t
basic_buffer::write_uint16 (uint16_t _val)
{
    if (m_arr.size() - m_idx < sizeof(uint16_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint16_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(uint16_t);
    return sizeof(uint16_t);
}

size_t
basic_buffer::write_int16 (int16_t _val)
{
    if (m_arr.size() - m_idx < sizeof(int16_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int16_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(int16_t);
    return sizeof(int16_t);
}

size_t
basic_buffer::write_uint32 (uint32_t _val)
{
    if (m_arr.size() - m_idx < sizeof(uint32_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint32_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(uint32_t);
    return sizeof(uint32_t);
}

size_t
basic_buffer::write_int32 (int32_t _val)
{
    if (m_arr.size() - m_idx < sizeof(int32_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int32_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(int32_t);
    return sizeof(int32_t);
}

size_t
basic_buffer::write_uint64 (uint64_t _val)
{
    if (m_arr.size() - m_idx < sizeof(uint64_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(uint64_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(uint64_t);
    return sizeof(uint64_t);
}

size_t
basic_buffer::write_int64 (int64_t _val)
{
    if (m_arr.size() - m_idx < sizeof(int64_t))
        throw logic_exception("no enough space", __FILE__, __LINE__);
    *(int64_t *)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(int64_t);
    return sizeof(int64_t);
}

size_t
basic_buffer::write_bytes (const uint8_t *_p, size_t _n)
{
    kassert(_p);
    if (m_arr.size() - m_idx < _n)
        throw logic_exception("no enough space", __FILE__, __LINE__);
    ::memcpy((void *)(m_arr.data() + m_idx), _p, _n);
    m_idx += _n;
    return _n;
}

size_t
basic_buffer::size () const
{
    return m_arr.size();
}

void
basic_buffer::resize (size_t _s)
{
    m_arr.resize(_s);
}

void
basic_buffer::shrink()
{
    m_arr.shrink_to_fit();
}

size_t
basic_buffer::next () const
{
    return (m_arr.size() - m_idx);
}

void
basic_buffer::reset (size_t _idx)
{
    assert(_idx < m_arr.size());
    m_idx = std::min(_idx, m_arr.size() - 1);
}

std::string &
basic_buffer::dump (std::string &_str)
{
    _str.clear();
    _str.reserve(m_arr.size() * 2 + 1);
    for (int i = 0; i < m_arr.size(); ++i) {
        char _tmp[3] = {0};
        snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _str += _tmp;
    }
    return _str;
}

buffer::buffer ()
    : basic_buffer(uint8_arr())
{

}
buffer::buffer (size_t _reserve)
    : basic_buffer(uint8_arr(), _reserve)
{
}

buffer::buffer (const uint8_arr &_arr, size_t _reserve /* = 0 */)
    : basic_buffer(_arr, _reserve)
{
}

buffer::buffer (uint8_arr &&_arr, size_t _reserve /* = 0 */)
    : basic_buffer(std::move(_arr), _reserve)
{
}

buffer::buffer (const buffer &_buf)
    : basic_buffer(_buf.m_arr, _buf.m_arr.size())
{
}

buffer::buffer (buffer &&_buf)
    : basic_buffer(std::move(_buf.m_arr), _buf.m_arr.size())
{
}

void
buffer::swap (buffer &_buf)
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_idx, _buf.m_idx);
}

__NAMESPACE_END
