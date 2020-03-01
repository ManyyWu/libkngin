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
        thread_err_code (int _code) noexcept
        { ptr = nullptr; code = _code; }
    };

    struct thread_data {
        std::string   name;

        thr_fn        fn;

        crash_handler handler;

        thread_data (const std::string &_name,
                     thr_fn &&_fn,
                     crash_handler &&_handler)
            : name(_name), fn(std::move(_fn)), handler(std::move(_handler)) {}
    };

public:
    thread        ();

    explicit
    thread        (const char *_name);

    virtual
    ~thread       () noexcept;

public:
    void
    run           (thr_fn &&_fn, crash_handler &&_crash_handler = nullptr);

    int
    join          ();

    void
    cancel        ();

    bool
    joined        () const noexcept
    { return m_joined; }

    pthread_t
    get_interface () const noexcept
    { return m_thr; }

    const char *
    name          () const noexcept
    { return m_name.c_str(); }

public:
    static uint64_t
    tid           () noexcept;

    static pthread_t
    ptid          () noexcept;

    static void
    sleep         (timestamp _ms) noexcept;

    static bool
    equal         (pthread_t _thr1, pthread_t _thr2) noexcept;

    bool
    equal_to      (pthread_t _t) noexcept;

    static void
    exit          (int _err_code) noexcept;

protected:
    static void *
    start         (void *_args) noexcept;

    static void
    cleanup       (void *_args) noexcept;

protected:
    const std::string m_name;

    pthread_t         m_thr;

    std::atomic_bool  m_joined;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_THREAD_H */