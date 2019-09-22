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
    : m_pfn(_pfn), m_args(_args), m_retptr(NULL), 
      m_tid(pthread_t{NULL, 0}), m_running(false)
#else
thread::thread (pthr_fn _pfn, void *_args, const char *_name /* = "" */)
    : m_pfn(_pfn), m_args(_args), m_retptr(NULL),
      m_tid(0), m_running(false), m_name(_name ? _name : "")
#endif
{
    kassert(_pfn);
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
    if_not (!_ret)
        server_fatal("pthread_detach() return %d", _ret);
}

bool
thread::run ()
{
#ifdef _WIN32
    kassert_r0(!m_tid.p);
#else
    kassert_r0(!m_tid);
#endif

    int _ret = 0;
    _ret = pthread_create(&m_tid, NULL, thread::start, this);
    if_not (!_ret) {
        server_fatal("pthread_create(), name = \"%s\", return %d", m_name, _ret);
    }
#ifdef _WIN32
    if (m_tid.p)
#else
    if (m_tid) 
#endif
        m_running.store(true);
    return !_ret;
}

bool
thread::join (int *_err_code)
{
#ifdef _WIN32
    kassert_r0(m_tid.p);
#else
    kassert_r0(m_tid);
#endif
    kassert_r0(!pthread_equal(m_tid, pthread_self()));

    int _ret = 0;
    _ret = pthread_join(m_tid, &m_retptr);
    if_not (!_ret) {
        server_fatal("pthread_join(), name = \"%s\"return %d", m_name, _ret);
        return false;
    }
    if (_err_code)
        *_err_code = (int)(long long)m_retptr;
#ifdef _WIN32
    m_tid = pthread_t{NULL, 0};
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
    kassert_r0(m_tid.p);
#else
    kassert_r0(m_tid);
#endif
    kassert_r0(!pthread_equal(m_tid, pthread_self()));

    int _ret = 0;
    _ret = pthread_cancel(m_tid);
    if_not (!_ret) {
        server_fatal("pthread_cancel(), name = \"%s\"return %d", m_name, _ret);
        return false;
    }
    return true;
}

bool
thread::running () const
{
    return m_running.load();
}

pthread_t
thread::get_interface () const
{
    kassert_r0(m_running.load());

    return m_tid;
}

int
thread::get_err_code  () const
{
    kassert_r0(m_running.load());

    return (int)(long long)m_retptr;
}

const char *
thread::name () const
{
    return m_name;
}

void
thread::sleep (time_t _ms)
{
    kassert_r(__time_valid(_ms));
#ifdef _WIN32
    Sleep(_ms);
#else
    usleep(std::abs(_ms) * 1000);
#endif
}

void
thread::exit (int _err_code)
{
    pthread_t _tid = pthread_self();
    pthread_exit((void *)(long long)_err_code);
}

pthread_t
thread::self ()
{
    return pthread_self();
}

void
thread::set_err_code (int _err_code)
{
    m_retptr = (void *)(long long)_err_code;
}

void *
thread::start (void *_args)
{
    kassert_r0(_args);

    thread *_p = (thread *)_args;
    pthread_cleanup_push(thread::cleanup, _args);

    _p->set_err_code((*_p->m_pfn)(_p->m_args));

    pthread_cleanup_pop(1);
    return _p->m_retptr;
}

void
thread::cleanup (void *_args)
{
    kassert_r(_args);

    thread *_p = (thread *)_args;
    _p->m_running.store(false);
}

int
thread::process (void *_args)
{
    // thread *_p = (thread *)_args;
    // pthread_cleanup_push()
    // pthread_setcancelstate()
    // pthread_setcanceltype()
    // pthread testcancel()

    fprintf(stderr, "thread::process()\n");
    fflush(stderr);

    // pthread_cleanup_pop()
    return 0 ;
}

__NAMESPACE_END
