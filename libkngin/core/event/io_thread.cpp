#ifndef _WIN32
//#include <signal.h>
#endif
#include <memory>
#include "core/base/common.h"
#include "core/event/io_thread.h"

#define KNGIN_FILENAME "libkngin/net/work_thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

io_thread::io_thread (const char *name)
    try
    : thread(name),
      loop_(nullptr),
      mutex_(),
      cond_(&mutex_)
{
} catch (...) {
    log_fatal("io_thread::iothread() error");
    throw;
}

io_thread::~io_thread ()
{
    if (loop_ and loop_->looping())
        ignore_excp(loop_->stop());
}

void
io_thread::run (crash_handler &&crash_handler /* = nullptr */)
{
    {
        local_lock lock(mutex_);
        loop_ = std::make_shared<event_loop>();
        thread::run([this] () -> int {
            return process();
        }, std::move(crash_handler));
        while (!loop_->looping())
            cond_.wait();
    }
}

void
io_thread::stop ()
{
    if (!equal_to(thread::ptid()))
    {
        local_lock lock(mutex_);
        loop_->stop();
        join();
    }
}

int
io_thread::process ()
{
    // shielding SIGPIPE signal
/*
#ifndef _WIN32
    sigset_t signal_mask;
    ::sigemptyset(&signal_mask);
    ::sigaddset(&signal_mask, SIGPIPE);
    ::pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
#endif
*/

    loop_->run([this] () {
        {
            local_lock lock(mutex_);
            cond_.signal();
        }
    }, nullptr);

    return 0;
}

KNGIN_NAMESPACE_K_END
