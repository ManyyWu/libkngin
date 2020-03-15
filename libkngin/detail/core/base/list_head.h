#ifndef KNGI_LIST_HEAD_H
#define KNGI_LIST_HEAD_H

#include "kngin/core/define.h"

KNGIN_NAMESPACE_K_DETAIL_BEGIN

#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
    struct list_head name = LIST_HEAD_INIT(name)

struct list_head {
  struct list_head *next, *prev;
};

static inline void INIT_LIST_HEAD(struct list_head *list) {
  list->next = list;
  list->prev = list;
}

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGI_LIST_HEAD_H */