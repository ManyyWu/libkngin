#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include <atomic>
#include <memory>
#include <functional>
#include "define.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

class thread {
public:
    union thread_err_code {
        void *ptr;
        int   code;

        thread_err_code ()          { ptr = nullptr; code = 0; }

        explicit
        thread_err_code (int _code) { ptr = nullptr; code = _code; }
    };

    typedef std::function<int (void)> thr_fn;

    struct thread_data {
        std::string       name;

        thr_fn            fn;

        thread_data (std::string &_name, thr_fn &&_fn) : name(_name), fn(std::move(_fn)) {}
    };

public:
    thread        () = delete;

    explicit
    thread        (const char *_name);

    virtual
    ~thread       ();

public:
    bool
    run           (thr_fn &&_fn);

    bool
    join          (int *_err_code);

    bool
    cancel        ();

    bool
    joined        () const        { return m_joined; }

    pthread_t
    get_interface () const        { return m_thr; }

    uint64_t
    get_tid       () const        { return m_tid; }

    const char *
    name          () const        { return m_name.c_str(); }

public:
    static uint64_t
#ifdef _WIN32
    tid           ()              { return ::GetCurrentThreadId(); }
#else
    tid           ()              { return ::getpid(); }
#endif

    static pthread_t
    self          ()              { return ::pthread_self(); }

    static void
#ifdef _WIN32
    sleep         (timestamp _ms) { ::Sleep((DWORD)_ms.value_uint()); }
#else
    sleep         (timestamp _ms) { ::usleep(_ms.value_uint() * 1000); }
#endif

    static void
    exit          (int _err_code) { ::pthread_exit(thread_err_code(_err_code).ptr); }

    bool
    equal_to      (pthread_t _t)  { return ::pthread_equal(_t, m_thr); }

protected:
    static void *
    start         (void *_args);

protected:
    static void
    cleanup       (void *_args);

protected:
    std::string       m_name;

    pthread_t         m_thr;

    uint64_t          m_tid;

    std::atomic<bool> m_joined;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */