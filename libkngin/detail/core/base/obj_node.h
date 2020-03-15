#ifndef KNGIN_OBJ_NODE_H
#define KNGIN_OBJ_NODE_H

#include "kngin/core/define.h"
#include "detail/core/base/list_head.h"
#include <memory>
#include <type_traits>

KNGIN_NAMESPACE_K_DETAIL_BEGIN
namespace obj_node {

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

template<typename Tp>
class entry_base;

template<typename Tp>
class obj_node : public noncopyable {
  template<typename T,
      typename std::enable_if<std::is_base_of<entry_base<T>, T>{}, int>::type>
  friend
  class obj_mgr;

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
class entry_base
  : public noncopyable,
    public std::enable_shared_from_this<Tp> {
  template<typename T,
      typename std::enable_if<std::is_base_of<entry_base<T>, T>{}, int>::type>
  friend
  class obj_mgr;

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

};
KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_OBJ_NODE_H */