#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/common.h"
#include "../libkngin/core/event/event_loop.h"

using namespace k;

void
event_loop_test ()
{
    event_loop _loop;

    thread _thr("event_loop_test");
    _thr.run([&_loop] () -> int {
        // task
        for (int i = 0; i < 10; i++) {
            _loop.run_in_loop([i] () {
                log_info("task %d", i);
            });
        }

        // timer
        _loop.run_every(100,
        [&] (const timer::timer_ptr _timer)
        {
            static int i = 0;
            if (i > 10) {
                _loop.cancel(_timer);
                return;
            }
                log_info("every 100ms - %d", ++i);
        });
        _loop.run_after(100,
        [&] (const timer::timer_ptr _timer)
        {
            log_warning("timer after 100ms");
            timestamp _current_time = timestamp::monotonic();
            _loop.run_at(_current_time + timestamp(1000),
                         [&] (const timer::timer_ptr &_timer) {
                log_warning("timer after current time + 1s");
            });
            _loop.run_at(_current_time + timestamp(2000),
                         [&] (const timer::timer_ptr &_timer) {
                log_warning("timer after current time + 2s");
            });
            _loop.run_at(_current_time + timestamp(3000),
                         [&] (const timer::timer_ptr &_timer) {
                log_warning("timer after current time + 3s");
                _loop.stop();
            });
        });
    });

    _loop.run([] () {
            log_info("event_loop started");
        }, [] () {
            log_info("event_loop stopped");
        }
    );
    _thr.join();
}
