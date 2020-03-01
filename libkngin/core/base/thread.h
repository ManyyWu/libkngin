#ifndef KNGIN_THREAD_H
#define KNGIN_THREAD_H

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <cstring>
#include <atomic>
#include <memory>
#include <functional>
#include "core/base/define.h"
#include "core/base/noncopyable.h"
#include "core/base/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
public:
    typedef std::function<void (pthread_t)> crash_handler;

    typedef std::function<int (void)>       thr_fn;

    union thread_err_code {
        void *ptr;

        int   code;

        thread_err_code () noexcept
        { ptr = nullptr; code = 0; }

        explicit
        thread_err_code (int code) noexcept
        { ptr = nullptr; code = code; }
    };

    struct thread_data {
        std::string   name;

        thr_fn        fn;

        crash_handler handler;

        thread_data (const std::string &name,
                     thr_fn &&fn,
                     crash_handler &&handler)
            : name(name), fn(std::move(fn)), handler(std::move(handler)) {}
    };

public:
    thread        ();

    explicit
    thread        (const char *name);

    virtual
    ~thread       () noexcept;

public:
    void
    run           (thr_fn &&fn, crash_handler &&crash_handler = nullptr);

    int
    join          ();

    void
    cancel        ();

    bool
    joined        () const noexcept
    { return joined_; }

    pthread_t
    get_interface () const noexcept
    { return thr_; }

    const char *
    name          () const noexcept
    { return name_.c_str(); }

public:
    static uint64_t
    tid           () noexcept;

    static pthread_t
    ptid          () noexcept;

    static void
    sleep         (timestamp ms) noexcept;

    static bool
    equal         (pthread_t thr1, pthread_t thr2) noexcept;

    bool
    equal_to      (pthread_t t) noexcept;

    static void
    exit          (int err_code) noexcept;

protected:
    static void *
    start         (void *args) noexcept;

    static void
    cleanup       (void *args) noexcept;

protected:
    const std::string name_;

    pthread_t         thr_;

    std::atomic_bool  joined_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_THREAD_H */