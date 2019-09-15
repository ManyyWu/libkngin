#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif
#include "thread.h"
#include "logfile.h"
#include "error.h"

__NAMESPACE_BEGIN

thread::thread (pthr_fn _pfn, void *_args)
    : m_pfn(_pfn), m_args(_args), m_err_code(0), m_tid(0)
{
    assert(_pfn);
}

thread::~thread ()
{
    int _ret = 0;
#ifdef _WIN32
#else
    if (m_tid)
        _ret = pthread_detach(m_tid);
    assert(!_ret);
#endif
}

bool
thread::run ()
{
    assert(!m_tid);

    int _ret = 0;
#ifdef _WIN32
    m_tid = (thread_interface)_beginthreadex(NULL, 0, thread::start, this,
                                             0, NULL);
    if (!m_tid)
        return false
#else
    _ret = pthread_create(&m_tid, NULL, thread::start, this);
    assert(!_ret);
    return !_ret;
#endif
}

bool
thread::join (unsigned int *_err_code, int _ms /* = INFINITE */)
{
    assert(m_tid);

    int _ret = 0;
#ifdef _WIN32
    assert(0);
    WaitForSingleObject(m_tid, _ms);
    CloseHandle(m_tid);
    m_tid = NULL;
#else
    assert(!pthread_equal(m_tid, pthread_self()));
    void *_perr_code= &m_err_code;
    _ret = pthread_join(m_tid, &_perr_code);
    assert(!_ret);
    if (_ret)
        return false;
    if (_err_code)
        *_err_code = m_err_code;
    m_tid = 0;
#endif
    return true;
}

bool
thread::cancel ()
{
    assert(m_tid);

    int _ret = 0;
#ifdef _WIN32
#else
    assert(!pthread_equal(m_tid, pthread_self()));
    _ret = pthread_cancel(m_tid);
    assert(!_ret);
#endif
    return !_ret;
}

bool
thread::is_exited () const
{
    return !m_tid;
}

int
thread::get_err_code  () const
{
    return m_err_code;
}

void
thread::sleep (int _ms)
{
#ifdef _WIN32
#else
    usleep(_ms * 1000);
#endif
}

void
thread::exit ()
{
#ifdef _WIN32
#else
    pthread_t _tid = pthread_self();
    pthread_exit(&_tid);
#endif
}

void
thread::set_err_code (unsigned int _err_code)
{
    m_err_code = _err_code;
}

unsigned int
thread::process (void *_args)
{
    assert(_args);

    thread *_p = (thread *)_args;

    // pthread_cleanup_push()
    // pthread_setcancelstate()
    // pthread_setcanceltype()
    // pthread testcancel()

    fprintf(stderr, "thread::process()\n", _p->m_tid);
    fflush(stderr);

    // pthread_cleanup_pop()
    return 0;
}

#ifdef _WIN32
signed int
process (void *_args)
#else
void *
thread::start (void *_args)
#endif
{
    assert(_args);

    thread *_p = (thread *)_args;

    _p->set_err_code((*_p->m_pfn)(_p->m_args));
    return &_p->m_err_code;
}

thread_interface
thread::get_interface () const
{
    return m_tid;
}

__NAMESPACE_END
