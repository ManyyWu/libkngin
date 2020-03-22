#ifndef KNGIN_LIST_H
#define KNGIN_LIST_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/memory.h"
#include "kngin/core/base/common.h"
#include "detail/core/base/kernel_list.h"
#include <cassert>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

template<typename Tp>
class list_entry
 : public noncopyable,
   public std::enable_shared_from_this<Tp> {
  template<typename T,
      typename std::enable_if<std::is_base_of<list_entry<T>, T>{}, int>::type>
  friend class list;

private:
  list_head head_;
};

template <typename Tp,
    typename std::enable_if<std::is_base_of<list_entry<Tp>, Tp>{}, int>::type = 0>
class list {
  static_assert(std::is_base_of<list_entry<Tp>, Tp>::value,
                "class Tp must be based on class list_entry");

public:
  list () noexcept {
    INIT_LIST_HEAD(&list_);
    size_ = 0;
  }

  ~list () noexcept {
    if (size())
      clear();
  }

  void
  insert (Tp &value) noexcept {
    assert(!exist(value));
    list_add_tail(&value.head_, &list_);
    ++size_;
  }

  void
  remove (Tp &value) noexcept {
    assert(exist(value));
    list_del(&value.head_);
    --size_;
  }

  Tp *
  begin () const noexcept {
    return list_first_entry_or_null(&list_, Tp, head_);
  }

  Tp *
  end () const noexcept {
    return size_ ? list_last_entry(&list_, Tp, head_) : nullptr;
  }

  bool
  exist (const Tp &value) const noexcept {
    Tp *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, Tp)
      if (pos == &value)
        return true;
    return false;
  }

  template <class Fn>
  void
  for_each (Fn fn) noexcept {
    Tp *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, Tp) {
      fn(pos->ptr_);
    }
  }

  size_t
  size () const noexcept {
    return size_;
  }

  bool
  empty () const noexcept {
    assert(size_ ? list_.next != list_.prev : list_.next == list_.prev);
    return !size_;
  }

  void
  clear () noexcept {
    Tp *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, Tp) {
      list_del(&pos->head_);
    }
    size_ = 0;
  }

  void
  swap (list &c) noexcept {
    std::swap(list_, c.head_);
    std::swap(size_, c.size_);
  }

private:
  list_head list_;
  size_t size_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_LIST_H */
