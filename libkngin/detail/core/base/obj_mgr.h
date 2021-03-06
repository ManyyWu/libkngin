#ifndef KNGIN_OBJ_MGR_H
#define KNGIN_OBJ_MGR_H

#include "kngin/core/base/noncopyable.h"
#include "kngin/core/base/memory.h"
#include "kngin/core/base/common.h"
#include "detail/core/base/kernel_list.h"
#include <cassert>

namespace k::detail {

template<typename Tp>
class obj_entry;

template<typename Tp>
class obj_node : public noncopyable {
  template<typename T,
      typename std::enable_if<std::is_base_of<obj_entry<T>, T>{}, int>::type>
  friend class obj_mgr;

public:
  typedef std::shared_ptr<Tp> ptr_type;

  obj_node () noexcept
   : ptr_(nullptr),
     head_(LIST_HEAD_INIT(head_)) {
  }

  obj_node (ptr_type ptr) noexcept
   : ptr_(ptr),
     head_(LIST_HEAD_INIT(head_)) {
  }

  void
  reset_ptr (ptr_type ptr) noexcept {
    ptr_ = ptr;
  }

protected:
  ptr_type ptr_;

  list_head head_;
};

template<typename Tp>
class obj_entry
  : public noncopyable,
    public std::enable_shared_from_this<Tp> {
  template<typename T,
      typename std::enable_if<std::is_base_of<obj_entry<T>, T>{}, int>::type>
  friend class obj_mgr;

private:
  void
  reset_node (obj_node<Tp> *ptr) noexcept {
    node_ = ptr;
  }

  obj_node<Tp> *
  get_node () noexcept {
    return node_;
  }

private:
  obj_node<Tp> *node_;
};

template <typename Tp,
    typename std::enable_if<std::is_base_of<obj_entry<Tp>, Tp>{}, int>::type = 0>
class obj_mgr {
  static_assert(std::is_base_of<obj_entry<Tp>, Tp>::value,
                "class Tp must be based on class obj_entry");

public:
  typedef std::shared_ptr<Tp> value_type;
  typedef obj_node<Tp> node_type;

  obj_mgr (size_t size = KNGIN_DEFAULT_PREALLOC_SIZE) {
    INIT_LIST_HEAD(&list_);
    INIT_LIST_HEAD(&free_);
    size_ = size_free_ = 0;
    resize(size);
  }

  ~obj_mgr () noexcept {
    if (max_size()) {
      clear();
      shrink_to_fix();
    }
  }

  void
  insert (const value_type& ptr) {
    assert(ptr);
    assert(!exist(ptr));
    node_type *n = nullptr;
    if (size_free_) {
      assert(n = free_to_list(ptr));
    } else {
      auto_fix();
      n = make_node(ptr);
      list_add_tail(&n->head_, &list_);
      ++size_;
    }
    ptr->reset_node(n);
  }

  void
  remove (const value_type& ptr) noexcept {
    assert(ptr);
    assert(exist(ptr));
    if (ptr->node_) {
      auto *head = &ptr->node_->head_;
      if (head) {
        move_to_free(ptr->node_);
        auto temp = ptr;
        ptr->node_->reset_ptr(nullptr);
        temp->reset_node(nullptr);
      }
    }
    auto_fix();
  }

  bool
  exist (const value_type& ptr) const noexcept {
    assert(ptr);
    if (ptr->get_node()) {
      auto *head = &ptr->get_node()->head_;
      if (head) {
        node_type *pos, *n;
        list_for_each_entry_safe(pos, n, &list_, head_, node_type)
          if (&pos->head_ == head)
            return true;
      }
    }
    return false;
  }

  template <typename Fn>
  void
  for_each (Fn fn) noexcept {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, node_type) {
      fn(pos->ptr_);
    }
  }

  size_t
  size () const noexcept {
    return size_;
  }

  size_t
  max_size () const noexcept {
    return (size_ + size_free_);
  }

  bool
  empty () const noexcept {
    assert(size_ ? true : list_empty_careful(&list_));
    return !size_;
  }

  void
  clear () noexcept {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, node_type) {
      move_to_free(pos);
      pos->ptr_->reset_node(nullptr);
      pos->reset_ptr(nullptr);
    }
  }

  void
  shrink_to_fix () noexcept {
    clear_free();
  }

  void
  resize (size_t size) {
    if (size > max_size()) {
      // add to tail of free_
      for (size_t num = size - max_size(), i = 0; i < num; ++i) {
        list_add_tail(&make_node(nullptr)->head_, &free_);
        ++size_free_;
      }
    } else if (size < size_ or size == max_size()) {
      return;
    } else {
      // remove nodes from free_
      for (size_t num = size_free_ - (size - size_), i = 0; i < num; ++i) {
        node_type *first_entry = list_first_entry_or_null(&free_, node_type, head_);
        assert(first_entry);
        list_del(&first_entry->head_);
        safe_release(first_entry);
        --size_free_;
      }
    }
  }

  void
  swap (obj_mgr & c) noexcept {
    std::swap(list_, c.head_);
    std::swap(free_, c.free_);
    std::swap(size_, c.size_);
    std::swap(size_free_, c.size_free_);
  }

protected:
  void
  auto_fix () {
    if (!size_free_)
      resize(max_size() * 2);
    else if (size_free_ > size_ * 2 and size_free_ > KNGIN_LIST_PRE_ALLOC_SIZE)
      resize(max_size() - (size_free_ - KNGIN_LIST_PRE_ALLOC_SIZE));
  }

  void
  clear_free () noexcept {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &free_, head_, obj_node<Tp>) {
      list_del(&pos->head_);
      safe_release(pos);
    }
    size_free_ = 0;
  }

  void
  move_to_free (node_type *node) noexcept {
    list_move_tail(&node->head_, &free_);
    --size_;
    ++size_free_;
  }

  node_type *
  free_to_list (const value_type& ptr) noexcept {
    if (size_free_) {
      node_type *first_entry = list_first_entry_or_null(&free_, node_type, head_);
      assert(first_entry);
      list_move_tail(&first_entry->head_, &list_);
      first_entry->reset_ptr(ptr);
      ++size_;
      --size_free_;
      return first_entry;
    } else {
      return nullptr;
    }
  }

  static
  node_type *
  make_node (const value_type& ptr) {
    auto *n = new node_type(ptr);
    return n;
  }

private:
  list_head list_;
  list_head free_;
  size_t    size_;
  size_t    size_free_;
};

} /* namespace k::detail */

#endif /* KNGIN_LIST_H */
