#include <memory>
#include <vector>
#include <string>
#include "cstring"
#include "exception.h"
#include "common.h"
#include "buffer.h"
#include "memory.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/buffer.cpp"

KNGIN_NAMESPACE_K_BEGIN

basic_buffer::basic_buffer (const uint8_arr &_arr, size_t _reserve /* = 0 */)
    try
    : m_arr(_arr), m_widx(0), m_ridx(0)
{
    m_arr.resize(std::max(_reserve, _arr.size()));
} catch (...) {
    log_fatal("basic_buffer::basic_buffer() error");
    throw;
}

basic_buffer::basic_buffer (uint8_arr &&_arr, size_t _reserve /* = 0 */)
    try
    : m_arr(std::move(_arr)), m_widx(0), m_ridx(0)
{
    m_arr.resize(std::max(_reserve, m_arr.size()));
} catch (...) {
    log_fatal("basic_buffer::basic_buffer() error");
    throw;
}

size_t
basic_buffer::read_bytes (uint8_t *_p, size_t _n)
{
    check(_p);
    check_readable(sizeof(_n));
    ::memcpy(_p, m_arr.data() + m_ridx, _n);
    m_ridx += _n;
    return _n;
}

size_t
basic_buffer::write_bytes (const uint8_t *_p, size_t _n)
{
    check(_p);
    check_writeable(sizeof(_n));
    ::memcpy((void *)(m_arr.data() + m_widx), _p, _n);
    m_widx += _n;
    return _n;
}

void
basic_buffer::copy_to (basic_buffer &_buf) const
{
    _buf.m_widx = _buf.m_ridx = 0;
    _buf.m_arr.assign(m_arr.begin(), m_arr.end());
}

void
basic_buffer::swap (basic_buffer &_buf)
{
    std::swap(m_ridx, _buf.m_ridx);
    std::swap(m_widx, _buf.m_widx);
    m_arr.swap(_buf.m_arr);
}

const std::string &
basic_buffer::dump ()
{
    m_dump_str.clear();
    m_dump_str.reserve(m_arr.size() * 2 + 1);
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
    if (m_widx - m_ridx < _n)
        throw k::exception((std::string("basic_buffer::readable() error - widx is ")
                        + std::to_string(m_widx) + ", ridx is "
                        + std::to_string(m_ridx)).c_str());
}

void
basic_buffer::check_writeable (size_t _n) const
{
    if (m_arr.size() - m_widx < _n)
        throw k::exception((std::string("basic_buffer::writeable() error - size is ")
                        + std::to_string(m_arr.size()) + ", widx is "
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

KNGIN_NAMESPACE_K_END
