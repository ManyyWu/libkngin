#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/common.h"
#include "../libkngin/core/base/system_error.h"
#include "../libkngin/core/event/event_loop.h"

#define KNGIN_FILENAME "libkngin_test/event_loop_test.cpp"

using namespace k;

void
event_loop_test ()
{
    event_loop loop;

    thread thr("event_loop_test");
    thr.run([&loop] () -> int {
        // task
        for (int i = 0; i < 10; i++) {
            loop.run_in_loop([i] () {
                log_info("task %d", i);
            });
        }

        // timer
        loop.run_every(100,
        [&] (const timer::timer_ptr timer)
        {
            static int i = 0;
            if (i > 10) {
                loop.cancel(timer);
                return;
            }
            log_info("every 100ms - %d", ++i);
        });
        loop.run_after(100,
        [&] (const timer::timer_ptr timer)
        {
            log_warning("timer after 100ms");
            timestamp current_time = timestamp::realtime();
            loop.run_at(current_time + timestamp(1000),
                         [&] (const timer::timer_ptr &timer) {
                log_warning("timer after current time + 1s");
            });
            loop.run_at(current_time + timestamp(2000),
                         [&] (const timer::timer_ptr &timer) {
                log_warning("timer after current time + 2s");
            });
            loop.run_at(current_time + timestamp(3000),
                         [&] (const timer::timer_ptr &timer) {
                log_warning("timer after current time + 3s");
                loop.stop();
            });
        });
        return 0;
    });

    loop.run(
        [] () {
            log_info("event_loop started");
        },
        [] () {
            log_info("event_loop stopped");
        }
    );
    thr.join();
}
