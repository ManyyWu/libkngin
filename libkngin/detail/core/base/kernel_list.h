#ifndef KNGIN_KERNEL_LIST_H
#define KNGIN_KERNEL_LIST_H

#include "kngin/core/define.h"

#if defined(KNGIN_SYSTEM_WIN32)
#include "detail/core/base/win_utils.h"
#endif /* defined(KNGIN_SYSTEM_WIN32) */

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

/*
 * Architectures might want to move the poison pointer offset
 * into some well-recognized area such as 0xdead000000000000,
 * that is also not mappable by user-space exploits:
 */
#ifdef CONFIG_ILLEGAL_POINTER_VALUE
# define POISON_POINTER_DELTA _AC(CONFIG_ILLEGAL_POINTER_VALUE, UL)
#else
# define POISON_POINTER_DELTA (0)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/*
 * These are non-NULL pointers that will result in page faults
 * under normal circumstances, used to verify that nobody uses
 * non-initialized list entries.
 */
#define LIST_POISON1  (void *)((char *) 0x00100100 + POISON_POINTER_DELTA)
#define LIST_POISON2  (void *)((char *) 0x00200200 + POISON_POINTER_DELTA)

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new_node,
                              struct list_head *prev,
                              struct list_head *next) {
  next->prev = new_node;
  new_node->next = next;
  new_node->prev = prev;
  prev->next = new_node;
}

static inline void list_add(struct list_head *new_node, struct list_head *head) {
  __list_add(new_node, head, head->next);
}

static inline void list_add_tail(struct list_head *new_node, struct list_head *head) {
  __list_add(new_node, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
  next->prev = prev;
  prev->next = next;
}

static inline void __list_del_entry(struct list_head *entry) {
  __list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry) {
  __list_del(entry->prev, entry->next);
  entry->next = (list_head *) LIST_POISON1;
  entry->prev = (list_head *) LIST_POISON2;
}

static inline void list_replace(struct list_head *old,
                                struct list_head *new_node) {
  new_node->next = old->next;
  new_node->next->prev = new_node;
  new_node->prev = old->prev;
  new_node->prev->next = new_node;
}

static inline void list_replace_init(struct list_head *old,
                                     struct list_head *new_node) {
  list_replace(old, new_node);
  INIT_LIST_HEAD(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 */
static inline void list_del_init(struct list_head *entry) {
  __list_del_entry(entry);
  INIT_LIST_HEAD(entry);
}

/**
 * list_move - delete from one list and add as another's head
 */
static inline void list_move(struct list_head *list, struct list_head *head) {
  __list_del_entry(list);
  list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 */
static inline void list_move_tail(struct list_head *list,
                                  struct list_head *head) {
  __list_del_entry(list);
  list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 */
static inline int list_is_last(const struct list_head *list,
                               const struct list_head *head) {
  return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 */
static inline int list_empty(const struct list_head *head) {
  return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
static inline int list_empty_careful(const struct list_head *head) {
  struct list_head *next = head->next;
  return (next == head) and (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 */
static inline void list_rotate_left(struct list_head *head) {
  struct list_head *first;

  if (!list_empty(head)) {
    first = head->next;
    list_move_tail(first, head);
  }
}

/**
 * list_is_singular - tests whether a list has just one entry.
 */
static inline int list_is_singular(const struct list_head *head) {
  return !list_empty(head) and (head->next == head->prev);
}

static inline void __list_cut_position(struct list_head *list,
                                       struct list_head *head, struct list_head *entry) {
  struct list_head *new_node_first = entry->next;
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry;
  entry->next = list;
  head->next = new_node_first;
  new_node_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 */
static inline void list_cut_position(struct list_head *list,
                                     struct list_head *head, struct list_head *entry) {
  if (list_empty(head)) {
    return;
  }
  if (list_is_singular(head) and
      (head->next != entry and head != entry)) {
    return;
  }
  if (entry == head) {
    INIT_LIST_HEAD(list);
  } else {
    __list_cut_position(list, head, entry);
  }
}

static inline void __list_splice(const struct list_head *list,
                                 struct list_head *prev,
                                 struct list_head *next) {
  struct list_head *first = list->next;
  struct list_head *last = list->prev;

  first->prev = prev;
  prev->next = first;

  last->next = next;
  next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 */
static inline void list_splice(const struct list_head *list,
                               struct list_head *head) {
  if (!list_empty(list)) {
    __list_splice(list, head, head->next);
  }
}

/**
 * list_splice_tail - join two lists, each list being a queue
 */
static inline void list_splice_tail(struct list_head *list,
                                    struct list_head *head) {
  if (!list_empty(list)) {
    __list_splice(list, head->prev, head);
  }
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head *list,
                                    struct list_head *head) {
  if (!list_empty(list)) {
    __list_splice(list, head, head->next);
    INIT_LIST_HEAD(list);
  }
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void list_splice_tail_init(struct list_head *list,
                                         struct list_head *head) {
  if (!list_empty(list)) {
    __list_splice(list, head->prev, head);
    INIT_LIST_HEAD(list);
  }
}

/**
 * list_entry - get the struct for this entry
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
    list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
    (!list_empty(ptr) ? list_first_entry(ptr, type, member) : nullptr)

/**
 * list_next_entry - get the next element in list
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_next_entry(pos, member, pos_type) \
    list_entry((pos)->member.next, pos_type, member)
#else
#define list_next_entry(pos, member, ...) \
    list_entry((pos)->member.next, typeof(*(pos)), member)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_prev_entry - get the prev element in list
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_prev_entry(pos, member, pos_type) \
    list_entry((pos)->member.prev, pos_type, member)
#else
#define list_prev_entry(pos, member, ...) \
    list_entry((pos)->member.prev, typeof(*(pos)), member)
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_for_each	-	iterate over a list
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 */
#define list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; \
         pos != (head); \
         pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_for_each_entry(pos, head, member, pos_type) \
    for (pos = list_first_entry(head, pos_type, member, pos_type); \
         &pos->member != (head); \
         pos = list_next_entry(pos, member, pos_type))
#else
#define list_for_each_entry(pos, head, member, ...) \
    for (pos = list_first_entry(head, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_next_entry(pos, member))
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_for_each_entry_reverse(pos, head, member, pos_type) \
    for (pos = list_last_entry(head, pos_type, member, pos_type); \
         &pos->member != (head); \
         pos = list_prev_entry(pos, member, pos_type))
#else
#define list_for_each_entry_reverse(pos, head, member, ...) \
    for (pos = list_last_entry(head, typeof(*pos), member); \
         &pos->member != (head); \
         pos = list_prev_entry(pos, member))
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_prepare_entry(pos, head, member, pos_type) \
    ((pos) ? : list_entry(head, pos_type, member))
#else
#define list_prepare_entry(pos, head, member, ...) \
    ((pos) ? : list_entry(head, typeof(*pos), member))
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) \
    for (pos = list_next_entry(pos, member); \
         &pos->member != (head); \
         pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member) \
    for (pos = list_prev_entry(pos, member); \
         &pos->member != (head); \
         pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) \
    for (; &pos->member != (head); \
         pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_for_each_entry_safe(pos, n, head, member, pos_type) \
    for (pos = list_first_entry(head, pos_type, member), \
        n = list_next_entry(pos, member, pos_type); \
         &pos->member != (head); \
         pos = n, n = list_next_entry(n, member, pos_type))
#else
#define list_for_each_entry_safe(pos, n, head, member, ...) \
    for (pos = list_first_entry(head, typeof(*pos), member), \
        n = list_next_entry(pos, member); \
         &pos->member != (head); \
         pos = n, n = list_next_entry(n, member))
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) \
    for (pos = list_next_entry(pos, member), \
        n = list_next_entry(pos, member); \
         &pos->member != (head); \
         pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) \
    for (n = list_next_entry(pos, member); \
         &pos->member != (head); \
         pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#if defined(KNGIN_SYSTEM_WIN32)
#define list_for_each_entry_safe_reverse(pos, n, head, member, pos_type) \
    for (pos = list_last_entry(head, pos_type, member, pos_type), \
        n = list_prev_entry(pos, member, pos_type); \
         &pos->member != (head); \
         pos = n, n = list_prev_entry(n, member, pos_type))
#else
#define list_for_each_entry_safe_reverse(pos, n, head, member, ...) \
    for (pos = list_last_entry(head, typeof(*pos), member), \
        n = list_prev_entry(pos, member); \
         &pos->member != (head); \
         pos = n, n = list_prev_entry(n, member))
#endif /* defined(KNGIN_SYSTEM_WIN32) */

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(pos, n, member) \
    n = list_next_entry(pos, member)

KNGIN_NAMESPACE_K_DETAIL_END

#endif /* KNGIN_KERNEL_LIST_H */