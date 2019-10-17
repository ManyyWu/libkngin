#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "define.h"
#include "exception.h"
#include "common.h"
#include "logfile.h"
#include "buffer.h"
#include "memory.h"

__NAMESPACE_BEGIN

size_t
basic_buffer::read_bytes (uint8_t *_p, size_t _n)
{
    kassert(_p);
    readable(sizeof(_n));
    ::memcpy(_p, m_arr.data() + m_idx, _n);
    m_idx += _n;
    return _n;
}

size_t
basic_buffer::write_bytes (const uint8_t *_p, size_t _n)
{
    kassert(_p);
    writeable(sizeof(_n));
    ::memcpy((void *)(m_arr.data() + m_idx), _p, _n);
    m_idx += _n;
    return _n;
}

void
basic_buffer::reset (size_t _idx)
{
    assert(_idx < m_arr.size());
    m_idx = std::min(_idx, m_arr.size() - 1);
}

std::string &
basic_buffer::dump ()
{
    m_dump_str.clear();
    m_dump_str.reserve(m_arr.size() * 1);
    for (size_t i = 0; i < m_arr.size(); ++i) {
        char _tmp[3] = {0};
        ::snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        m_dump_str += _tmp;
    }
    return m_dump_str;
}

bool
basic_buffer::readable (size_t _n)
{
    if (m_arr.size() - m_idx < _n)
        throw exception((std::string("basic_buffer::readable: size is ")
                        + std::to_string(m_arr.size()) + ", index is"
                        + std::to_string(m_idx)).c_str());
    return (m_arr.size() - m_idx >= _n);
}

bool
basic_buffer::writeable (size_t _n)
{
    if (m_arr.size() - m_idx < _n)
        throw exception((std::string("basic_buffer::writeable: size is ")
                        + std::to_string(m_arr.size()) + ", index is"
                        + std::to_string(m_idx)).c_str());
    return (m_arr.size() - m_idx >= _n);
}

template <typename __T> __T
basic_buffer::read ()
{
    readable(sizeof(__T));
    __T _val = *(__T *)(m_arr.data() + m_idx);
    m_idx += sizeof(__T);
    return _val;
}

template <typename __T> void
basic_buffer::write (__T _val)
{
    writeable(sizeof(__T));
    *(__T*)(m_arr.data() + m_idx) = _val;
    m_idx += sizeof(__T);
}

buffer &
buffer::append (const buffer &_buf)
{
    const uint8_arr &_arr = _buf.m_arr;
    m_arr.reserve(_arr.size());
    m_arr.insert(m_arr.end(), _arr.begin(), _arr.end());
    return *this;
}

__NAMESPACE_END
