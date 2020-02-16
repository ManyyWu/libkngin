#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/base/buffer.h"

#define KNGIN_FILENAME "libkngin/core/base/buffer.cpp"

KNGIN_NAMESPACE_K_BEGIN

out_buffer::out_buffer() KNGIN_NOEXCP
    : m_arr(nullptr),
      m_size(0)
{
}

out_buffer::out_buffer (const void * _arr, size_t _size)
    try
    : m_arr(static_cast<const unsigned char *>(_arr)),
      m_size(_size)
{
    arg_check(_arr and _size);
} catch (...) {
    log_fatal("out_buffer::out_buffer() error");
}

out_buffer::out_buffer(const out_buffer &_buf) KNGIN_NOEXCP
    : m_arr(_buf.m_arr), m_size(_buf.m_size)
{
}

out_buffer::out_buffer(out_buffer &&_buf) KNGIN_NOEXCP
    : m_arr(nullptr), m_size(0)
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
}

size_t
out_buffer::read_bytes (void * _p, size_t _n)
{
    assert(_p);
    assert(_n);
    check_readable(_n);
    ::memcpy(_p, m_arr, _n);
    m_size -= _n;
    m_arr += _n;
    return _n;
}

void
out_buffer::reset (const void * _arr, size_t _size)
{
    assert(_arr);
    assert(_size);
    m_arr = static_cast<const unsigned char *>(_arr);
    m_size = _size;
}

void
out_buffer::swap (out_buffer &_buf) KNGIN_NOEXCP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
}

std::shared_ptr<out_buffer>
out_buffer::clone ()
{
    return std::make_shared<out_buffer>(m_arr, m_size);
}

std::string
out_buffer::dump ()
{
    std::string _result;
    _result.resize(m_size * 2 + 1);
    char _tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _result[2 * i] = _tmp[0];
        _result[2 * i + 1] = _tmp[1];
    }
    return _result;
}

in_buffer::in_buffer() KNGIN_NOEXCP
    : m_arr(nullptr),
      m_size(0),
      m_valid(0)
{
}

in_buffer::in_buffer (void * _arr, size_t _size)
    try
    : m_arr(static_cast<unsigned char *>(_arr)),
      m_size(_size),
      m_valid(0)
{
    arg_check(_arr and _size);
} catch (...) {
    log_fatal("in_buffer::in_buffer() error");
}

in_buffer::in_buffer (const in_buffer &_buf) KNGIN_NOEXCP
    : m_arr(_buf.m_arr),
      m_size(_buf.m_size),
      m_valid(_buf.m_valid)
{
}

in_buffer::in_buffer (in_buffer &&_buf) KNGIN_NOEXCP
    : m_arr(nullptr),
      m_size(0),
      m_valid(0)
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
    std::swap(m_valid, _buf.m_valid);
}

in_buffer &
in_buffer::write_bytes (const void * _p, size_t _n)
{
    assert(_p);
    assert(_n);
    check_writeable(_n);
    ::memcpy(m_arr + m_valid, _p, _n);
    m_valid += _n;
    return *this;
}

void
in_buffer::reset (void * _arr, size_t _size)
{
    assert(_arr);
    assert(_size);
    m_arr = static_cast<unsigned char *>(_arr);
    m_size = _size;
    m_valid = 0;
}

void
in_buffer::swap (in_buffer &_buf) KNGIN_NOEXCP
{
    std::swap(m_arr, _buf.m_arr);
    std::swap(m_size, _buf.m_size);
    std::swap(m_valid, _buf.m_valid);
}

std::string
in_buffer::dump ()
{
    std::string _result;
    _result.resize(m_size * 2 + 1);
    char _tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(_tmp, sizeof(_tmp), "%02x", m_arr[i]);
        _result[2 * i] = _tmp[0];
        _result[2 * i + 1] = _tmp[1];
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
    for (auto &_iter : m_list)
        _size += _iter.size();
    return _size;
}

std::vector<struct ::iovec> &
net_buffer::to_iovec ()
{
    std::vector<struct ::iovec> _iovec;
    std::swap(m_iovec, _iovec);
    size_t _size = size();
    size_t _remain = (std::min)(_size, m_windex);
    for (auto &_iter : m_list) {
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
