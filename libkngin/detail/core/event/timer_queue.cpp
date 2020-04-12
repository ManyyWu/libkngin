#include "kngin/core/base/common.h"
#include "kngin/core/base/scoped_lock.h"
#include "kngin/core/event/event_loop.h"
#include "detail/core/event/timer_queue.h"
#include <memory>

namespace k::detail {

timer_queue::timer_queue (event_loop *loop)
 : heap_(),
   loop_(loop) {
}

timer_queue::~timer_queue () noexcept {
  TRY()
    clear();
  IGNORE_EXCP()
}

timer &
timer_queue::insert (timestamp initval, timestamp interval,
                     timeout_handler &&handler) {
  auto new_timer = std::make_shared<timer>(std::move(handler), initval, interval);
  new_timer->id_ = timer_id(new_timer);
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  heap_.push(new_timer);
#elif defined(KNGIN_USE_TIMERFD_TIMER)
  heap_.insert(new_timer);
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  return *new_timer;
}

void
timer_queue::remove (timer_ptr &ptr) {
  assert(ptr);
  ptr->close();
#if defined(KNGIN_USE_TIMERFD_TIMER)
  heap_.remove(ptr);
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  sort();
}

void
timer_queue::clear () {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  while (heap_.size())
    heap_.pop();
#elif defined(KNGIN_USE_TIMERFD_TIMER)
  heap_.for_each([&] (timer_ptr &ptr) {
    loop_->remove_event(ptr->event());
    remove(ptr);
  });
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

void
timer_queue::sort () {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  static auto temp = std::make_shared<timer>(nullptr, 0, 0);
  assert(temp->closed());

  heap_.push(temp);
/*  while (heap_.size()) {
    auto top = heap_.top();
    if (!top->closed())
      break;
    heap_.pop();
  }*/
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

timestamp
timer_queue::min_delay() const noexcept {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  return heap_.empty() ? timestamp::max : heap_.top()->get_timeout().remaining();
#elif defined(KNGIN_USE_TIMERFD_TIMER)
  return timestamp::zero;
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

timer_queue::timer_list &
timer_queue::get_ready_timers (timer_list &list) {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  list.clear();

  auto now = timestamp::monotonic();
  while (heap_.size()) {
    auto top = heap_.top();
    if (top->get_timeout().next() > now)
      break;
    if (!top->closed())
      list.push_back(top);
    heap_.pop();
  }
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
  return list;
}

void
timer_queue::process_ready_timers (timer_list &list, mutex &m) {
#if defined(KNGIN_USE_MONOTONIC_TIMER)
  auto now = timestamp::monotonic();
  for (auto iter : list) {
    auto &timeout = iter->get_timeout();
    if (iter->closed())
      continue;
    TRY()
      iter->on_events(now);
    CATCH_ERROR("timer_queue::process_ready_timers()");
    if (iter->closed())
      continue;

    if (timeout.persist()) {
      timeout.update();
      {
        mutex::scoped_lock lock(m);
        heap_.push(iter);
      }
    } else {
      iter->close();
    }
  }
#endif /* defined(KNGIN_USE_MONOTONIC_TIMER) */
}

} /* namespace k::detail */
