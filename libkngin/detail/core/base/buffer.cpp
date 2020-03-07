#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "kngin/core/base/exception.h"
#include "kngin/core/base/buffer.h"

KNGIN_NAMESPACE_K_BEGIN

out_buffer::out_buffer() noexcept
    : arr_(nullptr),
      size_(0)
{
}

out_buffer::out_buffer (const void * arr, size_t size)
try
    : arr_(static_cast<const unsigned char *>(arr)),
      size_(size)
{
  //arg_check(arr and size);
} catch (...) {
  //log_fatal("out_buffer::out_buffer() error");
}

out_buffer::out_buffer(const out_buffer &buf) noexcept
    : arr_(buf.arr_), size_(buf.size_)
{
}

out_buffer::out_buffer(out_buffer &&buf) noexcept
    : arr_(nullptr), size_(0)
{
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
}

size_t
out_buffer::read_bytes (void * p, size_t n)
{
  assert(p);
  assert(n);
  check_readable(n);
  ::memcpy(p, arr_, n);
  size_ -= n;
  arr_ += n;
  return n;
}

void
out_buffer::reset (const void * arr, size_t size)
{
  assert(arr);
  assert(size);
  arr_ = static_cast<const unsigned char *>(arr);
  size_ = size;
}

void
out_buffer::swap (out_buffer &buf) noexcept
{
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
}

std::shared_ptr<out_buffer>
out_buffer::clone ()
{
  return std::make_shared<out_buffer>(arr_, size_);
}

std::string
out_buffer::dump ()
{
  std::string result;
  result.resize(size_ * 2 + 1);
  char tmp[3] = {0};
  for (size_t i = 0; i < size_; ++i) {
    ::snprintf(tmp, sizeof(tmp), "%02x", arr_[i]);
    result[2 * i] = tmp[0];
    result[2 * i + 1] = tmp[1];
  }
  return result;
}

in_buffer::in_buffer() noexcept
    : arr_(nullptr),
      size_(0),
      valid_(0)
{
}

in_buffer::in_buffer (void * arr, size_t size)
try
    : arr_(static_cast<unsigned char *>(arr)),
      size_(size),
      valid_(0)
{
  //arg_check(arr and size);
} catch (...) {
  //log_fatal("in_buffer::in_buffer() error");
}

in_buffer::in_buffer (const in_buffer &buf) noexcept
    : arr_(buf.arr_),
      size_(buf.size_),
      valid_(buf.valid_)
{
}

in_buffer::in_buffer (in_buffer &&buf) noexcept
    : arr_(nullptr),
      size_(0),
      valid_(0)
{
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
  std::swap(valid_, buf.valid_);
}

in_buffer &
in_buffer::write_bytes (const void * p, size_t n)
{
  assert(p);
  assert(n);
  check_writeable(n);
  ::memcpy(arr_ + valid_, p, n);
  valid_ += n;
  return *this;
}

void
in_buffer::reset (void * arr, size_t size)
{
  assert(arr);
  assert(size);
  arr_ = static_cast<unsigned char *>(arr);
  size_ = size;
  valid_ = 0;
}

void
in_buffer::swap (in_buffer &buf) noexcept
{
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
  std::swap(valid_, buf.valid_);
}

std::string
in_buffer::dump ()
{
  std::string result;
  result.resize(size_ * 2 + 1);
  char tmp[3] = {0};
  for (size_t i = 0; i < size_; ++i) {
    ::snprintf(tmp, sizeof(tmp), "%02x", arr_[i]);
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
    windex_ += n;
}

void
net_buffer::send (size_t n)
{
    check_writeable(n);
    rindex_ += n;
}

size_t
net_buffer::readable ()
{
    size_t size = writeable();
    rindex_ = (std::min)(size, rindex_);
    return size - rindex_;
}

size_t
net_buffer::writeable ()
{
    size_t size = size();
    windex_ = (std::min)(size, windex_);
    return size - windex_;
}

size_t
net_buffer::size ()
{
    size_t size = 0;
    for (auto &iter : list_)
        size += iter.size();
    return size;
}

std::vector<struct ::iovec> &
net_buffer::to_iovec ()
{
    std::vector<struct ::iovec> iovec;
    std::swap(iovec_, iovec);
    size_t size = size();
    size_t remain = (std::min)(size, windex_);
    for (auto &iter : list_) {
        if (iter.size() > remain)
            iovec_.push_back({iter.data() + remain, iter.size() - remain});
        remain -= (std::min)(iter.size(), remain);
    }
    return iovec_;
}
void
net_buffer::swap (net_buffer &buf)
{
    std::swap(list_, buf.list_);
    std::swap(rindex_, buf.rindex_);
    std::swap(windex_, buf.windex_);
    std::swap(iovec_, buf.iovec_);
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