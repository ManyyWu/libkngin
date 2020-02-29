#ifndef LIST_H
#define LIST_H

#ifndef NDEBUG
#include <iostream>
#endif

#include <cassert>
#include <memory>
#include <type_traits>
#include "kernel_list.h"

// start of temporary
#define KNGIN_AUTO_FIX_THRESHOLD 100
#define KNGIN_PRE_ALLOC_SIZE     15

class noncopyable {};

#define safe_release(ptr)\
  do {                   \
    delete (ptr);        \
    (ptr) = nullptr;     \
  } while (false)
// end of temporary

template <typename Tp>
class value_base;

template <typename Tp>
class node : public noncopyable {
  template<typename T,
    typename std::enable_if<std::is_base_of<value_base<T>, T>{}, int>::type>
  friend class list;

protected:
  std::shared_ptr<Tp> ptr_;

  list_head head_;
};

template <typename Tp>
class value_base : public noncopyable {
  template <typename T,
    typename std::enable_if<std::is_base_of<value_base<T>, T>{}, int>::type>
  friend class list;

private:
  void
  reset_node (node<Tp> *ptr) noexcept {
    node_ = ptr;
  }

  node<Tp> *
  get_node () noexcept {
    return node_;
  }

private:
  node<Tp> *node_;
};

template <typename Tp,
    typename std::enable_if<std::is_base_of<value_base<Tp>, Tp>{}, int>::type = 0>
class list {
  static_assert(std::is_base_of<value_base<Tp>, Tp>::value, "class Tp must be based on class value_base");

public:
  typedef std::size_t size_type;
  typedef std::shared_ptr<Tp> value_type;
  typedef node<Tp> node_type;

  list (size_type size) noexcept {
    INIT_LIST_HEAD(&list_);
    INIT_LIST_HEAD(&free_);
    size_ = size_free_ = 0;
    resize(size);
  }

  ~list () noexcept {
    if (max_size()) {
      clear();
      shrink_to_fix();
    }
  }

#ifndef NDEBUG
  void
  show () {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, node_type)
    std::cerr << "(" << pos->ptr_->value() << ")|" << (int64_t)&pos->head_ << "|" << (int64_t)pos->head_.next << "->";
    std::cerr << std::endl;
  }

  void
  show_free () {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &free_, head_, node_type)
      std::cerr << (int64_t)&pos->head_ << "|" << (int64_t)pos->head_.next << "->";
    std::cerr << std::endl;
  }
#endif

  void
  add (const value_type& ptr) {
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
    assert(exist(ptr));
    if (ptr->node_) {
      auto *head = &ptr->node_->head_;
      if (head) {
        ptr->node_->ptr_ = nullptr;
        move_to_free(ptr->node_);
        ptr->reset_node(nullptr);
      }
    }
    auto_fix();
  }

  bool
  exist (const value_type& ptr) const noexcept {
    if (ptr->get_node()) {
      auto *head = &ptr->get_node()->head_;
      if (head) {
        node_type *pos, *n;
        list_for_each_entry_safe(pos, n, &list_, head_, node_type)if (&pos->head_ == head)
            return true;
      }
    }
    return false;
  }

  size_type
  size () const noexcept {
    return size_;
  }

  size_type
  max_size () const noexcept {
    return (size_ + size_free_);
  }

  bool
  empty () const noexcept {
    assert(size_ && list_.next != list_.prev || list_.next == list_.prev);
    return !size_;
  }

  void
  clear () {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &list_, head_, node_type) {
      move_to_free(pos);
      pos->ptr_->reset_node(nullptr);
      pos->ptr_ = nullptr;
    }
  }

  void
  shrink_to_fix () {
    clear_free();
  }

  void
  resize (size_type size) {
    if (size > max_size()) {
      // add to free_
      for (size_type num = size - max_size(), i = 0; i < num; ++i) {
        list_add_tail(&make_node(nullptr)->head_, &free_);
        ++size_free_;
      }
    } else if (size < size_ || size == max_size()) {
      return;
    } else {
      // remove nodes from free_
      node_type *pos;
      for (size_type num = size_free_ - (size - size_), i = 0; i < num; ++i) {
        node_type *first_entry = list_first_entry_or_null(&free_, node_type, head_);
        assert(first_entry);
        list_del(&first_entry->head_);
        safe_release(first_entry);
        --size_free_;
      }
    }
  }

  void
  swap (list & c) noexcept {
    std::swap(list_, c.head_);
    std::swap(free_, c.free_);
    std::swap(size_, c.size_);
    std::swap(size_free_, c.size_free_);
  }

protected:
  void
  auto_fix () {
    if (!size_free_)
      resize(max_size() + KNGIN_PRE_ALLOC_SIZE);
    else if (size_free_ > KNGIN_AUTO_FIX_THRESHOLD)
      resize(max_size() - (size_free_ - KNGIN_PRE_ALLOC_SIZE));
  }

  void
  clear_free () {
    node_type *pos, *n;
    list_for_each_entry_safe(pos, n, &free_, head_, node<Tp>) {
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
      first_entry->ptr_ = ptr;
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
    auto *n = new node_type();
    n->ptr_ = ptr;
    return n;
  }

private:
  list_head list_;
  list_head free_;
  size_type size_;
  size_type size_free_;
};

#endif /* LIST_H */
