#ifdef _WIN32
#include "pthread.h"
#include "windows.h"
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include "thread.h"
#include "logfile.h"
#include "error.h"

__NAMESPACE_BEGIN

#ifdef _WIN32
thread::thread (pthr_fn _pfn, void *_args)
    : m_pfn(_pfn), m_args(_args), m_err_code(0), 
      m_tid(pthread_t{NULL, 0}), m_cancel(false), m_running(false)
#else
thread::thread (pthr_fn _pfn, void *_args)
    : m_pfn(_pfn), m_args(_args), m_retptr(NULL),
      m_tid(0), m_running(false)
#endif
{
    assert(_pfn);
}

thread::~thread ()
{
    int _ret = 0;
#ifdef _WIN32
    if (m_tid.p)
#else
    if (m_tid)
#endif
        _ret = pthread_detach(m_tid);
    assert(!_ret);
}

bool
thread::run ()
{
#ifdef _WIN32
    assert(!m_tid.p);
#else
    assert(!m_tid);
#endif

    int _ret = 0;
    _ret = pthread_create(&m_tid, NULL, thread::start, this);
#ifdef _WIN32
    if (m_tid.p)
#else
    if (m_tid) 
#endif
    m_running.store(true);
    return !_ret;
}

bool
thread::join (int *_err_code, int _ms /* = INFINITE */)
{
#ifdef _WIN32
    assert(m_tid.p);
#else
    assert(m_tid);
#endif

    int _ret = 0;
    assert(_ms);
    assert(!pthread_equal(m_tid, pthread_self()));
    _ret = pthread_join(m_tid, &m_retptr);
    assert(!_ret);
    if (_ret)
        return false;
    if (_err_code)
        *_err_code = (int)(long)m_retptr;
#ifdef _WIN32
    m_tid.p = pthread_t{NULL, 0};
#else
    m_tid = 0;
#endif
    m_running.store(false);
    return true;
}

bool
thread::cancel ()
{
#ifdef _WIN32
    assert(m_tid.p);
#else
    assert(m_tid);
#endif

    int _ret = 0;
    assert(!pthread_equal(m_tid, pthread_self()));
    _ret = pthread_cancel(m_tid);
    assert(!_ret);
    if (_ret)
        return false;
    return join(NULL, INFINITE);
}

bool
thread::is_running () const
{
    return m_running.load();
}

pthread_t
thread::get_interface () const
{
    assert(m_running.load());
    return m_tid;
}

int
thread::get_err_code  () const
{
    assert(m_running.load());
    return (int)(long)m_retptr;
}

void
thread::sleep (int _ms)
{
#ifdef _WIN32
    Sleep(_ms);
#else
    usleep(_ms * 1000);
#endif
}

void
thread::exit (int _err_code)
{
    pthread_t _tid = pthread_self();
    pthread_exit((void *)(long)_err_code);
}

pthread_t
thread::self ()
{
    return pthread_self();
}

void
thread::testcancel ()
{
    pthread_testcancel();
}

void
thread::set_err_code (int _err_code)
{
    m_retptr = (void *)(long)_err_code;
}

void *
thread::start (void *_args)
{
    assert(_args);

    thread *_p = (thread *)_args;

    _p->set_err_code((*_p->m_pfn)(_p->m_args));
    _p->m_running.store(false);
    return _p->m_retptr;
}

int
thread::process (void *_args)
{
    // thread *_p = (thread *)_args;
    // pthread_cleanup_push()
    // pthread_setcancelstate()
    // pthread_setcanceltype()
    // pthread testcancel()

    fprintf(stderr, "thread::process()\n", self());
    fflush(stderr);

    // pthread_cleanup_pop()
    thread::exit(0);
}

__NAMESPACE_END
