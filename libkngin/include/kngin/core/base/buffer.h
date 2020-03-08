#ifndef KNGIN_BUFFER_H
#define KNGIN_BUFFER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/exception.h"
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

KNGIN_NAMESPACE_K_BEGIN

typedef std::shared_ptr<char> uint8_arr_ptr;
// XXX: use k::make_shared_array() to create uint8_arr_ptr
// FIXME: use std::shared<char []> in c++ 20

class out_buffer {
public:
  out_buffer () noexcept;

  out_buffer (const void *arr, size_t size);

  out_buffer (const out_buffer &buf) noexcept;

  out_buffer (out_buffer &&buf) noexcept;

  ~out_buffer () = default;

  const uint8_t &
  operator [] (size_t idx) const noexcept {
    return arr_[idx];
  }
  const uint8_t &
  at (size_t idx) const {
    check_readable(idx + 1);
    return arr_[idx];
  }
  const unsigned char *
  get (size_t idx) const {
    check_readable(idx + 1);
    return &arr_[idx];
  }
  const unsigned char *
  begin () const noexcept {
    return arr_;
  }
  size_t
  size () const noexcept {
    return size_;
  }
  bool
  eof () const noexcept {
    return !size_;
  }
  out_buffer &
  peek_uint8 (uint8_t &val) {
    val = read<uint8_t>(false);
    return *this;
  }
  out_buffer &
  peek_int8 (int8_t &val) {
    val = read<int8_t>(false);
    return *this;
  }
  out_buffer &
  peek_uint16 (uint16_t &val) {
    val = read<uint16_t>(false);
    return *this;
  }
  out_buffer &
  peek_int16 (int16_t &val) {
    val = read<int16_t>(false);
    return *this;
  }
  out_buffer &
  peek_uint32 (uint32_t &val) {
    val = read<uint32_t>(false);
    return *this;
  }
  out_buffer &
  peek_int32 (int32_t &val) {
    val = read<int32_t>(false);
    return *this;
  }
  out_buffer &
  peek_uint64 (uint64_t &val) {
    val = read<uint64_t>(false);
    return *this;
  }
  out_buffer &
  peek_int64 (int64_t &val) {
    val = read<int64_t>(false);
    return *this;
  }
  out_buffer &
  read_uint8 (uint8_t &val) {
    val = read<uint8_t>();
    return *this;
  }
  out_buffer &
  read_int8 (int8_t &val) {
    val = read<int8_t>();
    return *this;
  }
  out_buffer &
  read_uint16 (uint16_t &val) {
    val = read<uint16_t>();
    return *this;
  }
  out_buffer &
  read_int16 (int16_t &val) {
    val = read<int16_t>();
    return *this;
  }
  out_buffer &
  read_uint32 (uint32_t &val) {
    val = read<uint32_t>();
    return *this;
  }
  out_buffer &
  read_int32 (int32_t &val) {
    val = read<int32_t>();
    return *this;
  }
  out_buffer &
  read_uint64 (uint64_t &val) {
    val = read<uint64_t>();
    return *this;
  }
  out_buffer &
  read_int64 (int64_t &val) {
    val = read<int64_t>();
    return *this;
  }
  uint8_t
  peek_uint8 () {
    return read<uint8_t>(false);
  }
  int8_t
  peek_int8 () {
    return read<int8_t>(false);
  }
  uint16_t
  peek_uint16 () {
    return read<uint16_t>(false);
  }
  int16_t
  peek_int16 () {
    return read<int16_t>(false);
  }
  uint32_t
  peek_uint32 () {
    return read<uint32_t>(false);
  }
  int32_t
  peek_int32 () {
    return read<int32_t>(false);
  }
  uint64_t
  peek_uint64 () {
    return read<uint64_t>(false);
  }
  int64_t
  peek_int64 () {
    return read<int64_t>(false);
  }
  uint8_t
  read_uint8 () {
    return read<uint8_t>();
  }
  int8_t
  read_int8 () {
    return read<int8_t>();
  }
  uint16_t
  read_uint16 () {
    return read<uint16_t>();
  }
  int16_t
  read_int16 () {
    return read<int16_t>();
  }
  uint32_t
  read_uint32 () {
    return read<uint32_t>();
  }
  int32_t
  read_int32 () {
    return read<int32_t>();
  }
  uint64_t
  read_uint64 () {
    return read<uint64_t>();
  }
  int64_t
  read_int64 () {
    return read<int64_t>();
  }

  size_t
  read_bytes (void * p, size_t n);

  void
  reset (const void * buf, size_t size);

  void
  swap (out_buffer &buf) noexcept;

  std::shared_ptr<out_buffer>
  clone ();

  std::string
  dump ();

  out_buffer &
  operator -= (size_t size) {
    check_readable(size);
    size_ -= size;
    return *this;
  }

  out_buffer &
  operator = (const out_buffer &buf) noexcept {
    arr_ = buf.arr_;
    size_ = buf.size_;
    return *this;
  }

  void
  check_readable (size_t n) const {
    if (size_ < n)
      throw_exception("in_buffer::check_readable() - out of range");
  }

protected:
  template <typename Tp>
  Tp
  read (bool forward = true) {
    check_readable(sizeof(Tp));
    Tp val = *static_cast<const Tp *>(static_cast<const void *>(arr_));
    size_ -= forward ? sizeof(Tp) : 0;
    arr_ += forward ? sizeof(Tp) : 0;
    return val;
  }

private:
  const unsigned char * arr_;

  size_t size_;
};

class msg_buffer {
public:
  msg_buffer () noexcept
   : arr_(nullptr), buf_() {
  }

  msg_buffer (uint8_arr_ptr &arr, size_t offset, size_t size)
   : arr_(arr), buf_(arr.get() + offset, size) {
  }

  msg_buffer (const void *arr, size_t size)
   : arr_(nullptr), buf_(arr, size) {
  }
  // the arr must be constant string or stack space that life cycle is longer than this

  msg_buffer (const msg_buffer &buf)
   : arr_(buf.arr_), buf_(buf.buf_) {
  }

  msg_buffer (msg_buffer &&buf) noexcept
   : arr_(std::move(buf.arr_)), buf_(std::move(buf.buf_)) {
  }

  ~msg_buffer () = default;

  uint8_arr_ptr &
  get () noexcept {
    return arr_;
  }
  const uint8_arr_ptr &
  get () const noexcept {
    return arr_;
  }
  out_buffer &
  buffer () {
    return buf_;
  }
  const out_buffer &
  buffer () const noexcept {
    return buf_;
  }
  msg_buffer &
  operator = (const msg_buffer &buf) {
    arr_ = buf.arr_;
    buf_ = buf.buf_;
    return *this;
  }

private:
  uint8_arr_ptr arr_;

  out_buffer buf_;
};

class in_buffer {
public:
  in_buffer () noexcept;

  in_buffer (void * arr, size_t size);

  in_buffer (const in_buffer &buf) noexcept;

  in_buffer (in_buffer &&buf) noexcept;

  ~in_buffer () = default;

  uint8_t &
  operator [] (size_t idx) noexcept {
    return arr_[idx];
  }
  uint8_t &
  at (size_t idx) {
    check_readable(idx + 1);
    return arr_[idx];
  }
  unsigned char *
  get (size_t idx) {
    check_readable(idx + 1);
    return &arr_[idx];
  }
  unsigned char *
  begin () const noexcept {
    return arr_;
  }
  size_t
  size () const noexcept {
    return size_;
  }
  bool
  eof () const noexcept {
    return !size_;
  }
  size_t
  valid () const noexcept {
    return valid_;
  }
  size_t
  writeable () const noexcept {
    return (size_ - valid_);
  }

  in_buffer &
  write_uint8 (uint8_t val) {
    return write<uint8_t>(val);
  }
  in_buffer &
  write_int8 (int8_t val) {
    return write<int8_t>(val);
  }
  in_buffer &
  write_uint16 (uint16_t val) {
    return write<uint16_t>(val);
  }
  in_buffer &
  write_int16 (int16_t  val) {
    return write<int16_t>(val);
  }
  in_buffer &
  write_uint32 (uint32_t val) {
    return write<uint32_t>(val);
  }
  in_buffer &
  write_int32 (int32_t val) {
    return write<int32_t>(val);
  }
  in_buffer &
  write_uint64 (uint64_t val) {
    return write<uint64_t>(val);
  }
  in_buffer &
  write_int64 (int64_t val) {
    return write<int64_t>(val);
  }

  in_buffer &
  write_bytes (const void * p, size_t n);

  void
  reset (void * buf, size_t size);

  void
  swap (in_buffer &buf) noexcept;

  std::string
  dump ();

  in_buffer &
  operator += (size_t size) {
    check_writeable(size);
    valid_ += size;
    return *this;
  }

  in_buffer &
  operator = (const in_buffer &buf) noexcept {
    arr_ = buf.arr_;
    size_ = buf.size_;
    valid_ = buf.valid_;
    return *this;
  }

protected:
  void
  check_readable (size_t n) const {
    if (valid_ < n)
      throw_exception("in_buffer::check_readable() - out of range");
  }

  void
  check_writeable (size_t n) const {
    if (size_ - valid_ < n)
      throw_exception("in_buffer::check_writeable() - out of range");
  }

  template <typename Tp>
  in_buffer &
  write (Tp val) {
    check_writeable(sizeof(Tp));
    *static_cast<Tp *>(static_cast<void *>(arr_ + valid_)) = val;
    valid_ += sizeof(Tp);
    return *this;
  }

private:
  unsigned char * arr_;

  size_t size_;

  size_t valid_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_BUFFER_H */