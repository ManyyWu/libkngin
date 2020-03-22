#include "kngin/core/base/exception.h"
#include "kngin/core/base/buffer.h"
#include "kngin/core/base/common.h"
#include <memory>
#include <vector>
#include <string>
#include <cstring>

KNGIN_NAMESPACE_K_BEGIN

out_buffer::out_buffer() noexcept
 : arr_(nullptr),
   size_(0) {
}

out_buffer::out_buffer (const void * arr, size_t size) noexcept
 : arr_(static_cast<const unsigned char *>(arr)),
   size_(size) {
  assert(arr);
  assert(size);
}

out_buffer::out_buffer(const out_buffer &buf) noexcept
 : arr_(buf.arr_),
   size_(buf.size_) {
}

out_buffer::out_buffer(out_buffer &&buf) noexcept
 : arr_(nullptr),
   size_(0) {
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
}

size_t
out_buffer::read_bytes (void * p, size_t n) {
  assert(p);
  assert(n);
  check_readable(n);
  ::memcpy(p, arr_, n);
  size_ -= n;
  arr_ += n;
  return n;
}

void
out_buffer::reset (const void * arr, size_t size) noexcept {
  assert(arr);
  assert(size);
  arr_ = static_cast<const unsigned char *>(arr);
  size_ = size;
}

void
out_buffer::swap (out_buffer &buf) noexcept {
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
}

std::string
out_buffer::dump () {
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

in_buffer::in_buffer (void * arr, size_t size) noexcept
 : arr_(static_cast<unsigned char *>(arr)),
   size_(size),
   valid_(0) {
  assert(arr);
  assert(size);
}

in_buffer::in_buffer (const in_buffer &buf) noexcept
 : arr_(buf.arr_),
   size_(buf.size_),
   valid_(buf.valid_) {
}

in_buffer::in_buffer (in_buffer &&buf) noexcept
 : arr_(nullptr),
   size_(0),
   valid_(0) {
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
  std::swap(valid_, buf.valid_);
}

in_buffer &
in_buffer::write_bytes (const void * p, size_t n) {
  assert(p);
  assert(n);
  check_writeable(n);
  ::memcpy(arr_ + valid_, p, n);
  valid_ += n;
  return *this;
}

void
in_buffer::reset (void * arr, size_t size) noexcept {
  assert(arr);
  assert(size);
  arr_ = static_cast<unsigned char *>(arr);
  size_ = size;
  valid_ = 0;
}

void
in_buffer::swap (in_buffer &buf) noexcept {
  std::swap(arr_, buf.arr_);
  std::swap(size_, buf.size_);
  std::swap(valid_, buf.valid_);
}

std::string
in_buffer::dump () {
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

KNGIN_NAMESPACE_K_END