#ifndef _THREAD_H_
#define _THREAD_H_

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif
#include "define.h"
#include "common.h"
#include "noncopyable.h"

__NAMESPACE_BEGIN

#ifdef _WIN32
typedef HANDLE       thread_interface;
#else
typedef pthread_t    thread_interface;
#define INFINITE     0xffffffff
#endif
typedef unsigned int (*pthr_fn) (void *);

class thread /* : public noncopyable */ {
public:
    thread        () = delete;

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
    is_exited     () const;

    int
    get_err_code  () const;

public:
    static void
    sleep         (int ms);

    static void
    exit          ();

    void
    set_err_code  (unsigned int _err_code);

public:
#ifdef _WIN32
    static signed int
    process   (void *);
#else
    static void *
    start         (void *_args);
#endif

public:
    static unsigned int
    process       (void *_args);

public:
    thread_interface
    get_interface () const;


protected:
    thread_interface m_tid;

    pthr_fn          m_pfn;

    void *           m_args;

    unsigned int     m_err_code;
};

__NAMESPACE_END

#endif /* _THREAD_H_ */