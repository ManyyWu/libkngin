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
class entry_base
  : public noncopyable,
    public std::enable_shared_from_this<entry_base<Tp>> {
  template <typename T,
      typename std::enable_if<std::is_base_of<entry_base<T>, T>{}, int>::type>
  friend class list;

private:
  list_head head_;
};

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_LIST_NODE_H */