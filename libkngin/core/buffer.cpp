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
    check_readable(sizeof(_n));
    ::memcpy(_p, m_arr.data() + m_ridx, _n);
    m_ridx += _n;
    return _n;
}

size_t
basic_buffer::write_bytes (const uint8_t *_p, size_t _n)
{
    kassert(_p);
    check_writeable(sizeof(_n));
    ::memcpy((void *)(m_arr.data() + m_widx), _p, _n);
    m_widx += _n;
    return _n;
}

void
basic_buffer::reset (size_t _idx)
{
    assert(_idx < m_arr.size());
    m_widx = std::min(_idx, m_arr.size() - 1);
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

void
basic_buffer::check_readable (size_t _n) const
{
    if (m_arr.size() - m_ridx < _n)
        throw exception((std::string("basic_buffer::readable: size is ")
                        + std::to_string(m_arr.size()) + ", index is "
                        + std::to_string(m_ridx)).c_str());
}

void
basic_buffer::check_writeable (size_t _n) const
{
    if (m_arr.size() - m_widx < _n)
        throw exception((std::string("basic_buffer::writeable: size is ")
                        + std::to_string(m_arr.size()) + ", index is "
                        + std::to_string(m_widx)).c_str());
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
