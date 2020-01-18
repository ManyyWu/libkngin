#ifndef KNGIN_THREAD_H
#define KNGIN_THREAD_H

#include <cstring>
#include <atomic>
#include <memory>
#include <functional>
#include "core/define.h"
#include "core/noncopyable.h"
#include "core/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
    public:
#ifdef _WIN32
        struct thread_t;
#endif
    typedef std::function<void (pthread_t)> crash_handler;

    typedef std::function<int (void)>       thr_fn;

    union thread_err_code {
        void *ptr;

        int   code;

        thread_err_code () KNGIN_NOEXCP
        { ptr = nullptr; code = 0; }

        explicit
        thread_err_code (int _code) KNGIN_NOEXCP
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
    thread         ();

    explicit
    thread         (const char *_name);

    virtual
    ~thread        () KNGIN_NOEXCP;

public:
    void
    run           (thr_fn &&_fn, crash_handler &&_crash_handler = nullptr);

    int
    join          ();

    void
    cancel        ();

    bool
    joined        () const KNGIN_NOEXCP
    { return m_joined; }

    pthread_t
    get_interface () const KNGIN_NOEXCP
    { return m_thr; }

    const char *
    name          () const KNGIN_NOEXCP
    { return m_name.c_str(); }

public:
    static uint64_t
    tid           () KNGIN_NOEXCP;

    static pthread_t
    ptid          () KNGIN_NOEXCP;

    static void
    sleep         (timestamp _ms) KNGIN_NOEXCP;

    static bool
    equal         (pthread_t _thr1, pthread_t _thr2) KNGIN_NOEXCP;

    bool
    equal_to      (pthread_t _t) KNGIN_NOEXCP;

    static void
    exit          (int _err_code) KNGIN_NOEXCP;

protected:
    static void *
    start         (void *_args) KNGIN_NOEXCP;

    static void
    cleanup       (void *_args) KNGIN_NOEXCP;

protected:
    const std::string m_name;

    pthread_t         m_thr;

    std::atomic_bool  m_joined;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_THREAD_H */