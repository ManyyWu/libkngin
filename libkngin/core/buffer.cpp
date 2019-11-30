#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "core/common.h"
#include "core/exception.h"
#include "core/buffer.h"
#include "core/memory.h"

#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin/core/buffer.cpp"

KNGIN_NAMESPACE_K_BEGIN

out_buffer::out_buffer() KNGIN_NOEXP
    : m_arr(nullptr),
      m_size(0)
{
}

out_buffer::out_buffer (cuint8_arr _arr, size_t _size) KNGIN_EXP
    try
    : m_arr(_arr),
      m_size(_size)
{
    arg_check(_arr && _size);
} catch (...) {
    log_fatal("out_buffer::out_buffer() error");
}

out_buffer::out_buffer (cint8_arr _arr, size_t _size) KNGIN_EXP
    try
    : m_arr(static_cast<cuint8_arr>(static_cast<const void *>(_arr))),
      m_size(_size)
{
    arg_check(_arr && _size);
} catch (...) {
    log_fatal("out_buffer::out_buffer() error");
}

out_buffer::out_buffer(out_buffer &&_buf) KNGIN_NOEXP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
}

size_t
out_buffer::read_bytes (uint8_arr _p, size_t _n) KNGIN_EXP
{
    arg_check(_p && _n);
    check_readable(_n);
    ::memcpy(_p, m_arr, _n);
    m_size -= _n;
    return _n;
}

void
out_buffer::reset (cuint8_arr _arr, size_t _size) KNGIN_EXP
{
    arg_check(_arr && _size);
    m_arr = _arr;
    m_size = _size;
}

void
out_buffer::reset (cint8_arr _arr, size_t _size) KNGIN_EXP
{
    arg_check(_arr && _size);
    m_arr = static_cast<cuint8_arr>(static_cast<const void *>(_arr));
    m_size = _size;
}

void
out_buffer::swap (out_buffer &_buf) KNGIN_NOEXP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
}

std::string
out_buffer::dump () KNGIN_EXP
{
    std::string _result;
    _result.reserve(m_size * 2 + 1);
    char _tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _result += _tmp;
    }
    return _result;
}

in_buffer::in_buffer() KNGIN_NOEXP
    : m_arr(nullptr),
      m_size(0),
      m_valid(0)
{
}

in_buffer::in_buffer (uint8_arr _arr, size_t _size) KNGIN_EXP
    try
    : m_arr(_arr),
      m_size(_size),
      m_valid(0)
{
    arg_check(_arr && _size);
} catch (...) {
    log_fatal("in_buffer::in_buffer() error");
}

in_buffer::in_buffer (int8_arr _arr, size_t _size) KNGIN_EXP
    try
    : m_arr(static_cast<uint8_arr>(static_cast<void *>(_arr))),
      m_size(_size),
      m_valid(0)
{
    arg_check(_arr && _size);
} catch (...) {
    log_fatal("in_buffer::in_buffer() error");
}

in_buffer::in_buffer (in_buffer &&_buf) KNGIN_NOEXP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
    std::swap(m_valid, _buf.m_valid);
}

size_t
in_buffer::write_bytes (uint8_arr _p, size_t _n) KNGIN_EXP
{
    arg_check(_p && _n);
    check_readable(_n);
    ::memcpy(m_arr, _p, _n);
    m_valid += _n;
    return _n;
}

void
in_buffer::reset (uint8_arr _arr, size_t _size) KNGIN_EXP
{
    arg_check(_arr && _size);
    m_arr = _arr;
    m_size = _size;
    m_valid = 0;
}


void
in_buffer::reset (int8_arr _arr, size_t _size) KNGIN_EXP
{
    arg_check(_arr && _size);
    m_arr = static_cast<uint8_arr>(static_cast<void *>(_arr));
    m_size = _size;
    m_valid = 0;
}

void
in_buffer::swap (in_buffer &_buf) KNGIN_NOEXP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
    std::swap(m_valid, _buf.m_valid);
}

std::string
in_buffer::dump () KNGIN_EXP
{
    std::string _result;
    _result.reserve(m_size * 2 + 1);
    char _tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _result += _tmp;
    }
    return _result;
}

/*
void
net_buffer::receive (size_t _n)
{
    check_readable(_n);
    m_windex += _n;
}

void
net_buffer::send (size_t _n)
{
    check_writeable(_n);
    m_rindex += _n;
}

size_t
net_buffer::readable ()
{
    size_t _size = writeable();
    m_rindex = (std::min)(_size, m_rindex);
    return _size - m_rindex;
}

size_t
net_buffer::writeable ()
{
    size_t _size = size();
    m_windex = (std::min)(_size, m_windex);
    return _size - m_windex;
}

size_t
net_buffer::size ()
{
    size_t _size = 0;
    for (auto _iter : m_list)
        _size += _iter.size();
    return _size;
}

std::vector<struct iovec> &
net_buffer::to_iovec ()
{
    std::vector<struct iovec> _iovec;
    std::swap(m_iovec, _iovec);
    size_t _size = size();
    size_t _remain = (std::min)(_size, m_windex);
    for (auto _iter : m_list) {
        if (_iter.size() > _remain)
            m_iovec.push_back({_iter.data() + _remain, _iter.size() - _remain});
        _remain -= (std::min)(_iter.size(), _remain);
    }
    return m_iovec;
}
void
net_buffer::swap (net_buffer &_buf)
{
    std::swap(m_list, _buf.m_list);
    std::swap(m_rindex, _buf.m_rindex);
    std::swap(m_windex, _buf.m_windex);
    std::swap(m_iovec, _buf.m_iovec);
}

void
net_buffer::check_readable (size_t _n)
{
    if (readable() < _n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}

void
net_buffer::check_writeable (size_t _n)
{
    if (writeable() < _n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}
*/
KNGIN_NAMESPACE_K_END
