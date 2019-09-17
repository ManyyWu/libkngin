#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <atomic>
#include "define.h"
#include "common.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

typedef pthread_t    thread_interface;
#define INFINITE     0xFFFFFFFF
typedef unsigned int (*pthr_fn) (void *);

class thread : public noncopyable {
public:
    thread        (pthr_fn _pfn, void *_args);

    virtual
    ~thread       ();

public:
    bool
    run           ();

    bool
    join          (unsigned int *_err_code, int _ms = INFINITE);

    bool
    cancel        ();

    bool
    is_running    () const;

    thread_interface
    get_interface () const;

    unsigned int
    get_err_code  () const;

public:
    static void
    sleep         (int ms);

    static void
    exit          (unsigned int _err_code);

    static thread_interface
    self ();

    static void
    testcancel    ();

    void
    set_err_code  (unsigned int _err_code);

protected:
    static void *
    start         (void *_args);

public:
    static unsigned int
    process       (void *_args);

protected:
    thread_interface  m_tid;

    pthr_fn           m_pfn;

    void *            m_args;

    unsigned int      m_err_code;

#ifdef _WIN32
    std::atomic<bool> m_cancel;
#endif

    std::atomic<bool> m_running;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */