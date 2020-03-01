#include <vector>
#include <atomic>
#include <memory>
#include <algorithm>
#include "core/base/exception.h"
#include "core/base/common.h"
#include "core/event/io_thread.h"
#include "core/event/io_threadpool.h"

#define KNGIN_FILENAME "libkngin/core/event/io_threadpool.cpp"

KNGIN_NAMESPACE_K_BEGIN

io_threadpool::io_threadpool (uint16_t num)
    try
    : num_((std::max<uint16_t>)(num, 1)),
      threads_(),
      stopped_(true),
      crash_(false),
      mutex_(),
      next_(1)
{
    threads_.reserve(num_);
} catch (...) {
    log_fatal("io_threadpool::io_threadpool() error");
    throw;
}

io_threadpool::~io_threadpool ()
{
    if (!stopped_)
        ignore_excp(stop());
}

void
io_threadpool::start (crash_handler handler)
{
    assert(stopped_);

    for (int i = 0; i < num_; ++i) {
        auto name = std::string("io_thread_") + std::to_string(i);
        threads_.push_back(std::make_unique<io_thread>(name.c_str()));
        try {
            //log_debug("%s", threads_.back()->name());
            threads_.back()->run([this, handler] (pthread_t t) {
                if (crash_)
                    return;
                crash_ = true;
                ignore_excp(handler());
                stopped_ = false;
            }); // end of crash_handler
        } catch (...) {
            stopped_ = false;
            ignore_excp(stop());
            throw k::exception("thread pool startup failed");
        }
    }

    stopped_ = false;
    log_info("thread pool started");
}

void
io_threadpool::stop ()
{
    assert(!stopped_);

    {
        local_lock lock(mutex_);
        auto size = threads_.size();
        for (uint16_t i = 0; i < size; ++i)
            threads_[i]->stop();
        threads_.clear();
    }
    stopped_ = true;
    log_info("thread pool stopped");
}

void
io_threadpool::add_task (task &&task)
{
    assert(task);
    assert(!stopped_);

    event_loop &next = next_loop();
    {
        local_lock lock(mutex_);
        if (next.looping())
            next.run_in_loop(std::move(task));
    }
}

event_loop &
io_threadpool::next_loop ()
{
    assert(!stopped_);
    {
        local_lock lock(mutex_);
        size_t size = threads_.size();
        assert(size);
        if (next_ >= size)
            next_ = std::max<size_t>(size - 1, 0);
        return *(threads_[next_++]->get_loop());
    }
}

event_loop &
io_threadpool::get_loop (size_t idx)
{
    assert(!stopped_);
    {
        local_lock lock(mutex_);
        size_t size = threads_.size();
        assert(idx < size);
        return *(threads_[idx]->get_loop());
    }
}

KNGIN_NAMESPACE_K_END
