#include "../libkngin/core/thread.h"
#include "../libkngin/core/common.h"
#include "../libkngin/net/event_loop.h"

using namespace k;

void
event_loop_test ()
{
    event_loop _loop;

    thread _thr("");
    _thr.run([&_loop] () -> int {
        thread::sleep(1000);
        for (int i = 0; i < 10; i++) {
            _loop.run_in_loop([i] () {
                log_info("%d", i);
            });
        }
        thread::sleep(1000);
        _loop.stop();
        return 0;
    });

    _loop.run(nullptr, nullptr);
    _thr.join();
}
