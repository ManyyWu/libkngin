#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <atomic>
#include <functional>
#include <unistd.h> ///////////
#include "define.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

class thread {
public:
    typedef std::function<int (void)> thr_fn;

    union thread_err_code {
        void *ptr;
        int   code;

        thread_err_code ()          { ptr = nullptr; code = 0; }

        explicit
        thread_err_code (int _code) { ptr = nullptr; code = _code; }
    };

public:
    thread        () = delete;

    explicit
    thread        (const char *_name);

    virtual
    ~thread       ();

public:
    virtual bool
    run           (thr_fn &&_fn);

    virtual bool
    join          (int *_err_code);

    virtual bool
    cancel        ();

    bool
    running       () const        { return m_running; }

    bool
    joined        () const        { return m_joined; }

    pthread_t
    get_interface () const        { return m_thr; }

    uint64_t
    get_tid       () const        { return m_tid; }

    int
    get_err_code  () const        { return m_err_code.code; }

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

    void
    set_err_code  (int _err_code) { m_err_code.code = _err_code; }

    bool
    equal_to      (pthread_t _t)  { return ::pthread_equal(_t, m_thr); }

protected:
    static void *
    start         (void *_args);

protected:
    static void
    cleanup       (void *_args);

public:
    static int
    process       (void *_args);

protected:
    std::string       m_name;

    pthread_t         m_thr;

    uint64_t          m_tid;

    thread_err_code   m_err_code;

    std::atomic<bool> m_running;

    std::atomic<bool> m_joined;

    thr_fn            m_fn;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */