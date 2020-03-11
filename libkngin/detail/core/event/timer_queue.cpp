#include "kngin/core/base/common.h"
#include "kngin/core/base/scoped_lock.h"
#include "detail/core/event/timer_queue.h"
#include "detail/core/event/timer.h"
#include <memory>

KNGIN_NAMESPACE_K_DETAIL_BEGIN

timer_queue::timer_queue ()
 : heap_() {
}

timer_queue::~timer_queue () {
  TRY()
    clear();
  IGNORE()
}

timer &
timer_queue::insert (timestamp initval, timestamp interval,
                     timeout_handler &&handler) {
  auto new_timer = std::make_shared<timer>(std::move(handler));
  heap_.push(new_timer);
  new_timer->set_time(initval, interval);
  return *new_timer;
}

void
timer_queue::remove (timer_ptr &ptr) {
  assert(ptr);
  static auto temp = std::make_shared<timer>(nullptr);
  if (!temp->closed())
    temp->close();

  ptr->close();
  heap_.push(temp);
  while (heap_.size()) {
    auto top = heap_.top();
    if (!top->closed())
      break;
    heap_.pop();
  }
}

void
timer_queue::clear () {
  while (heap_.size())
    heap_.pop();
}

timestamp
timer_queue::min_delay() const noexcept {
  return heap_.empty() ? timestamp::max : heap_.top()->get_timeout().remaining();
}

timer_queue::timer_list &
timer_queue::get_ready_timers (timer_list &list) {
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
    CATCH_ERROR("timer_queue::process_ready_timers() error");
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

KNGIN_NAMESPACE_K_DETAIL_END
