#include <pthread.h>
#include <unistd.h>
#include "thread.h"
#include "logfile.h"
#include "error.h"

__NAMESPACE_BEGIN

#ifdef _WIN32
thread::thread (pthr_fn _pfn, void *_args)
    : m_pfn(_pfn), m_args(_args), m_err_code(0), 
      m_tid(0), m_cancel(false), m_running(false)
#else
thread::thread (pthr_fn _pfn, void *_args)
    : m_pfn(_pfn), m_args(_args), m_err_code(0), 
      m_tid(0), m_running(false)
#endif
{
    assert(_pfn);
}

thread::~thread ()
{
    int _ret = 0;
    if (m_tid)
        _ret = pthread_detach(m_tid);
    assert(!_ret);
    m_tid = 0; 
    m_running.store(false);
}

bool
thread::run ()
{
    assert(!m_tid);

    int _ret = 0;
    _ret = pthread_create(&m_tid, NULL, thread::start, this);
    if (m_tid) 
        m_running.store(true);
    return !_ret;
}

bool
thread::join (unsigned int *_err_code, int _ms /* = INFINITE */)
{
    assert(m_tid);

    int _ret = 0;
    assert(_ms);
    assert(!pthread_equal(m_tid, pthread_self()));
    void *_perr_code= &m_err_code;
    _ret = pthread_join(m_tid, &_perr_code);
    assert(!_ret);
    if (_ret)
        return false;
    if (_err_code)
        *_err_code = m_err_code;
    m_tid = 0;
    m_running.store(false);
    return true;
}

bool
thread::cancel ()
{
    assert(m_tid);

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

thread_interface
thread::get_interface () const
{
    return m_tid;
}

unsigned int
thread::get_err_code  () const
{
    assert(m_running.load());
    return m_err_code;
}

void
thread::sleep (int _ms)
{
    usleep(_ms * 1000);
}

void
thread::exit (unsigned int _err_code)
{
    m_tid = _err_code;
    pthread_t _tid = pthread_self();
    pthread_exit(&_tid);
}

thread_interface
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
thread::set_err_code (unsigned int _err_code)
{
    m_err_code = _err_code;
}

void *
thread::start (void *_args)
{
    assert(_args);

    thread *_p = (thread *)_args;

    _p->set_err_code((*_p->m_pfn)(_p->m_args));
    _p->m_running.store(false);
    return &_p->m_err_code;
}

unsigned int
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
    return 0;
}

__NAMESPACE_END
