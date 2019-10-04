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
#include "common.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

typedef std::function<int (void *)> thr_fn;

class thread : public noncopyable {
public:
    thread        (thr_fn _fn, const char *_name = "");

    virtual
    ~thread       ();

public:
    virtual bool
    run           ();

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
    static void
    sleep         (time_t _ms);

    static void
    exit          (int _err_code);

    static pthread_t
    self          ();

    void
    set_err_code  (int _err_code);

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

    pthread_t         m_tid;

    thr_fn            m_fn;

    void *            m_args;

    void *            m_retptr;

    std::atomic<bool> m_running;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */