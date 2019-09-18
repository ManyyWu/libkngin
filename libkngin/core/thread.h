#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <atomic>
#include "define.h"
#include "common.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

#define INFINITE     0xFFFFFFFF
typedef int (*pthr_fn) (void *);

class thread : public noncopyable {
public:
    thread        (pthr_fn _pfn, void *_args);

    virtual
    ~thread       ();

public:
    bool
    run           ();

    bool
    join          (int *_err_code, int _ms = INFINITE);

    bool
    cancel        ();

    bool
    is_running    () const;

    pthread_t
    get_interface () const;

    int
    get_err_code  () const;

public:
    static void
    sleep         (int ms);

    static void
    exit          (int _err_code);

    static pthread_t
    self          ();

    static void
    testcancel    ();

    void
    set_err_code  (int _err_code);

protected:
    static void *
    start         (void *_args);

public:
    static int
    process       (void *_args);

protected:
    pthread_t         m_tid;

    pthr_fn           m_pfn;

    void *            m_args;

    void *            m_retptr;

    std::atomic<bool> m_running;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */