#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <atomic>
#include <functional>
#include "define.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

class thread {
public:
    typedef std::function<int (void *)> thr_fn;

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
    running       () const;

    pthread_t
    get_interface () const;

    int
    get_err_code  () const;

    const char *
    name          () const;

public:
    static uint64_t
    get_tid       ();

    static void
    sleep         (timestamp _ms);

    static void
    exit          (int _err_code);

    static pthread_t
    self          ();

    void
    set_err_code  (int _err_code);

    bool
    equal_to      (pthread_t _t);

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
    const std::string m_name;

    pthread_t         m_thr;

    uint64_t          m_tid;

    void *            m_args;

    void *            m_retptr;

    std::atomic<bool> m_running;

    thr_fn            m_fn;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */