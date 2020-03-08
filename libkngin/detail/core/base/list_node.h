#ifndef KNGIN_LIST_NODE_H
#define KNGIN_LIST_NODE_H

#include "kngin/core/define.h"
#include "detail/core/base/kernel_list.h"
#include <memory>
#include <type_traits>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

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

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_LIST_NODE_H */