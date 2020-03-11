#ifndef KNGIN_LIST_NODE_H
#define KNGIN_LIST_NODE_H

#include "kngin/core/define.h"
#include <memory>
#include <type_traits>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

struct list_head {
  struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
  list->next = list;
  list->prev = list;
}

template <typename Tp>
class entry_base;

template <typename Tp>
class node : public noncopyable {
  template<typename T,
      typename std::enable_if<std::is_base_of<entry_base<T>, T>{}, int>::type>
  friend class list;

public:
  typedef std::shared_ptr<Tp> ptr_type;

  node () noexcept
   : ptr_(nullptr),
     head_(LIST_HEAD_INIT(head_)) {
  }

  node (ptr_type ptr) noexcept
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

template <typename Tp>
class entry_base
  : public noncopyable,
    public std::enable_shared_from_this<entry_base<Tp>> {
  template <typename T,
      typename std::enable_if<std::is_base_of<entry_base<T>, T>{}, int>::type>
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

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_LIST_NODE_H */