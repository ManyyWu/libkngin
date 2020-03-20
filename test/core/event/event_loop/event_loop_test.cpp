#include "kngin/core/base/log.h"
#include "kngin/core/base/common.h"
#include "kngin/core/event/event_loop.h"

using namespace std;

int
main () {
  TRY()
    k::event_loop loop;
    loop.run_in_loop([] () {
      debug("task0");
    });
    loop.run_at(k::timestamp::realtime() + k::timestamp(2000),
      [&] (const k::timer_id &timer, const k::timestamp &ts) {
      debug("timer0: %" PRIu64, ts.value());
      loop.run_after(5000,
          [&] (const k::timer_id &timer, const k::timestamp &ts) {
        debug("timer1: %" PRIu64, ts.value());
        loop.run_in_loop([&] () {
          debug("task1");
          loop.stop();
        });
      });
      loop.run_after(3000,
          [&] (const k::timer_id &timer, const k::timestamp &ts) {
        debug("timer2: %" PRIu64, ts.value());
      });
      loop.run_every(1000,
          [&] (const k::timer_id &timer, const k::timestamp &ts) {
        int times = 3;
        debug("timer3: %" PRIu64, ts.value());
        if (--times <= 0) {
          loop.cancel(timer);
        }
      });
    });
    loop.run();
  IGNORE_EXCP("main()")

  return 0;
}