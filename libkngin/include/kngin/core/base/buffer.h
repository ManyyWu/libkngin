#ifndef KNGIN_BUFFER_H
#define KNGIN_BUFFER_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/exception.h"
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

namespace k {

typedef std::shared_ptr<char> uint8_arr_ptr;
// XXX: use k::make_shared_array() to create uint8_arr_ptr
// FIXME: use std::shared<char []> in c++ 20

class out_buffer {
public:
  out_buffer () noexcept;

  out_buffer (const void *arr, size_t size) noexcept;

  out_buffer (const out_buffer &buf) noexcept;

  out_buffer (out_buffer &&buf) noexcept;

  ~out_buffer () = default;

  const uint8_t &
  operator [] (size_t idx) const noexcept {
    return arr_[idx];
  }
  const uint8_t &
  at (size_t idx) const { check_readable(idx + 1); return arr_[idx]; }
  const uint8_t *
  get (size_t idx) const { check_readable(idx + 1); return &arr_[idx]; }
  const uint8_t *
  begin () const noexcept { return arr_; }
  const uint8_t *
  end () const noexcept { return (arr_ + size_ - 1); }
  size_t
  size () const noexcept { return size_; }
  bool
  eof () const noexcept { return !size_; }
  out_buffer &
  peek (uint8_t &val) { val = read<uint8_t>(false); return *this; }
  out_buffer &
  peek (int8_t &val) { val = read<int8_t>(false); return *this; }
  out_buffer &
  peek (uint16_t &val) { val = read<uint16_t>(false); return *this; }
  out_buffer &
  peek (int16_t &val) { val = read<int16_t>(false); return *this; }
  out_buffer &
  peek (uint32_t &val) { val = read<uint32_t>(false); return *this; }
  out_buffer &
  peek (int32_t &val) { val = read<int32_t>(false); return *this; }
  out_buffer &
  peek (uint64_t &val) { val = read<uint64_t>(false); return *this; }
  out_buffer &
  peek (int64_t &val) { val = read<int64_t>(false); return *this; }
  out_buffer &
  read (uint8_t &val) { val = read<uint8_t>(); return *this; }
  out_buffer &
  read (int8_t &val) { val = read<int8_t>(); return *this; }
  out_buffer &
  read (uint16_t &val) { val = read<uint16_t>(); return *this; }
  out_buffer &
  read (int16_t &val) { val = read<int16_t>(); return *this; }
  out_buffer &
  read (uint32_t &val) { val = read<uint32_t>(); return *this; }
  out_buffer &
  read (int32_t &val) { val = read<int32_t>(); return *this; }
  out_buffer &
  read (uint64_t &val) { val = read<uint64_t>(); return *this; }
  out_buffer &
  read (int64_t &val) { val = read<int64_t>(); return *this; }
  uint8_t
  peek_uint8 () { return read<uint8_t>(false); }
  int8_t
  peek_int8 () { return read<int8_t>(false); }
  uint16_t
  peek_uint16 () { return read<uint16_t>(false); }
  int16_t
  peek_int16 () { return read<int16_t>(false); }
  uint32_t
  peek_uint32 () { return read<uint32_t>(false); }
  int32_t
  peek_int32 () { return read<int32_t>(false); }
  uint64_t
  peek_uint64 () { return read<uint64_t>(false); }
  int64_t
  peek_int64 () { return read<int64_t>(false); }
  uint8_t
  read_uint8 () { return read<uint8_t>(); }
  int8_t
  read_int8 () { return read<int8_t>(); }
  uint16_t
  read_uint16 () { return read<uint16_t>(); }
  int16_t
  read_int16 () { return read<int16_t>(); }
  uint32_t
  read_uint32 () { return read<uint32_t>(); }
  int32_t
  read_int32 () { return read<int32_t>(); }
  uint64_t
  read_uint64 () { return read<uint64_t>(); }
  int64_t
  read_int64 () { return read<int64_t>(); }

  size_t
  read (void *p, size_t n);

  void
  reset (const void *buf, size_t size) noexcept;

  void
  swap (out_buffer &buf) noexcept;

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
  const uint8_t *arr_;

  size_t size_;
};

class in_buffer {
public:
  in_buffer () noexcept;

  in_buffer (void *arr, size_t size) noexcept;

  in_buffer (const in_buffer &buf) noexcept;

  in_buffer (in_buffer &&buf) noexcept;

  ~in_buffer () = default;

  uint8_t &
  operator [] (size_t idx) noexcept { return arr_[idx]; }
  uint8_t &
  at (size_t idx) { check_readable(idx + 1); return arr_[idx]; }
  uint8_t *
  get (size_t idx) { check_readable(idx + 1); return &arr_[idx]; }
  uint8_t *
  begin () const noexcept { return arr_; }
  uint8_t *
  end () const noexcept { return (arr_ + size_ - 1); }
  size_t
  size () const noexcept { return size_; }
  size_t
  valid () const noexcept { return valid_; }
  size_t
  writeable () const noexcept { return (size_ - valid_); }

  in_buffer &
  write (uint8_t val) { return write_value<uint8_t>(val); }
  in_buffer &
  write (int8_t val) { return write_value<int8_t>(val); }
  in_buffer &
  write (uint16_t val) { return write_value<uint16_t>(val); }
  in_buffer &
  write (int16_t  val) { return write_value<int16_t>(val); }
  in_buffer &
  write (uint32_t val) { return write_value<uint32_t>(val); }
  in_buffer &
  write (int32_t val) { return write_value<int32_t>(val); }
  in_buffer &
  write (uint64_t val) { return write_value<uint64_t>(val); }
  in_buffer &
  write (int64_t val) { return write_value<int64_t>(val); }

  in_buffer &
  write (const void *p, size_t n);

  void
  reset (void *buf, size_t size) noexcept;

  void
  swap (in_buffer &buf) noexcept;

  std::string
  dump () const;

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
  write_value (Tp val) {
    check_writeable(sizeof(Tp));
    *static_cast<Tp *>(static_cast<void *>(arr_ + valid_)) = val;
    valid_ += sizeof(Tp);
    return *this;
  }

private:
  uint8_t *arr_;

  size_t size_;

  size_t valid_;
};

} /* namespace k */

#endif /* KNGIN_BUFFER_H */