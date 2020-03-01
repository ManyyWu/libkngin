#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "core/base/common.h"
#include "core/base/exception.h"
#include "core/base/buffer.h"

#define KNGIN_FILENAME "libkngin/core/base/buffer.cpp"

KNGIN_NAMESPACE_K_BEGIN

out_buffer::out_buffer() noexcept
    : m_arr(nullptr),
      m_size(0)
{
}

out_buffer::out_buffer (const void * arr, size_t size)
    try
    : m_arr(static_cast<const unsigned char *>(arr)),
      m_size(size)
{
    arg_check(arr and size);
} catch (...) {
    log_fatal("out_buffer::out_buffer() error");
}

out_buffer::out_buffer(const out_buffer &buf) noexcept
    : m_arr(buf.m_arr), m_size(buf.m_size)
{
}

out_buffer::out_buffer(out_buffer &&buf) noexcept
    : m_arr(nullptr), m_size(0)
{
    std::swap(m_arr, buf.m_arr);
    std::swap(m_size, buf.m_size);
}

size_t
out_buffer::read_bytes (void * p, size_t n)
{
    assert(p);
    assert(n);
    check_readable(n);
    ::memcpy(p, m_arr, n);
    m_size -= n;
    m_arr += n;
    return n;
}

void
out_buffer::reset (const void * arr, size_t size)
{
    assert(arr);
    assert(size);
    m_arr = static_cast<const unsigned char *>(arr);
    m_size = size;
}

void
out_buffer::swap (out_buffer &buf) noexcept
{
    std::swap(m_arr, buf.m_arr);
    std::swap(m_size, buf.m_size);
}

std::shared_ptr<out_buffer>
out_buffer::clone ()
{
    return std::make_shared<out_buffer>(m_arr, m_size);
}

std::string
out_buffer::dump ()
{
    std::string result;
    result.resize(m_size * 2 + 1);
    char tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(tmp, sizeof(tmp), "%02x", m_arr[i]);
        result[2 * i] = tmp[0];
        result[2 * i + 1] = tmp[1];
    }
    return result;
}

in_buffer::in_buffer() noexcept
    : m_arr(nullptr),
      m_size(0),
      m_valid(0)
{
}

in_buffer::in_buffer (void * arr, size_t size)
    try
    : m_arr(static_cast<unsigned char *>(arr)),
      m_size(size),
      m_valid(0)
{
    arg_check(arr and size);
} catch (...) {
    log_fatal("in_buffer::in_buffer() error");
}

in_buffer::in_buffer (const in_buffer &buf) noexcept
    : m_arr(buf.m_arr),
      m_size(buf.m_size),
      m_valid(buf.m_valid)
{
}

in_buffer::in_buffer (in_buffer &&buf) noexcept
    : m_arr(nullptr),
      m_size(0),
      m_valid(0)
{
    std::swap(m_arr, buf.m_arr);
    std::swap(m_size, buf.m_size);
    std::swap(m_valid, buf.m_valid);
}

in_buffer &
in_buffer::write_bytes (const void * p, size_t n)
{
    assert(p);
    assert(n);
    check_writeable(n);
    ::memcpy(m_arr + m_valid, p, n);
    m_valid += n;
    return *this;
}

void
in_buffer::reset (void * arr, size_t size)
{
    assert(arr);
    assert(size);
    m_arr = static_cast<unsigned char *>(arr);
    m_size = size;
    m_valid = 0;
}

void
in_buffer::swap (in_buffer &buf) noexcept
{
    std::swap(m_arr, buf.m_arr);
    std::swap(m_size, buf.m_size);
    std::swap(m_valid, buf.m_valid);
}

std::string
in_buffer::dump ()
{
    std::string result;
    result.resize(m_size * 2 + 1);
    char tmp[3] = {0};
    for (size_t i = 0; i < m_size; ++i) {
        ::snprintf(tmp, sizeof(tmp), "%02x", m_arr[i]);
        result[2 * i] = tmp[0];
        result[2 * i + 1] = tmp[1];
    }
    return result;
}

/*
void
net_buffer::receive (size_t n)
{
    check_readable(n);
    m_windex += n;
}

void
net_buffer::send (size_t n)
{
    check_writeable(n);
    m_rindex += n;
}

size_t
net_buffer::readable ()
{
    size_t size = writeable();
    m_rindex = (std::min)(size, m_rindex);
    return size - m_rindex;
}

size_t
net_buffer::writeable ()
{
    size_t size = size();
    m_windex = (std::min)(size, m_windex);
    return size - m_windex;
}

size_t
net_buffer::size ()
{
    size_t size = 0;
    for (auto &iter : m_list)
        size += iter.size();
    return size;
}

std::vector<struct ::iovec> &
net_buffer::to_iovec ()
{
    std::vector<struct ::iovec> iovec;
    std::swap(m_iovec, iovec);
    size_t size = size();
    size_t remain = (std::min)(size, m_windex);
    for (auto &iter : m_list) {
        if (iter.size() > remain)
            m_iovec.push_back({iter.data() + remain, iter.size() - remain});
        remain -= (std::min)(iter.size(), remain);
    }
    return m_iovec;
}
void
net_buffer::swap (net_buffer &buf)
{
    std::swap(m_list, buf.m_list);
    std::swap(m_rindex, buf.m_rindex);
    std::swap(m_windex, buf.m_windex);
    std::swap(m_iovec, buf.m_iovec);
}

void
net_buffer::check_readable (size_t n)
{
    if (readable() < n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}

void
net_buffer::check_writeable (size_t n)
{
    if (writeable() < n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}
*/
KNGIN_NAMESPACE_K_END
