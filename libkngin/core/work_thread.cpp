#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#endif
#include <cstdio>
#include "define.h"
#include "work_thread.h"
#include "work_task.h"
#include "thread.h"
#include "msg.h"

__NAMESPACE_BEGIN

work_thread::work_thread (const char *_name)
    : thread(work_thread::process, this, _name ? _name : "work_thread"),
      m_done(true), m_new_task(false),
      m_stop_thread(false), m_task(NULL),
      m_mutex(NULL), m_cond(NULL)
{
}

work_thread::~work_thread ()
{
    if (m_task)
        m_task->release();
    if (m_mutex)
        m_mutex->release();
    if (m_cond)
        m_cond->release();
    server_info("work_thread \"%s\" closed", m_name.c_str());
}

bool
work_thread::run ()
{
    kassert_r0(!m_running.load());
    kassert_r0(!m_mutex && !m_cond);

    bool _ret = true;
    m_mutex = mutex::create();
    if_not (m_mutex)
        goto fail;
    m_cond = cond::create(m_mutex);
    kassert_r0(m_cond);
    if (!m_cond)
        goto fail;
    _ret = this->thread::run();
    if_not (_ret) {
fail:
        if (m_cond)
            m_cond->release();
        if (m_mutex)
            m_mutex->release();
        if (this->thread::running()) {
            _ret = this->cancel();
            _ret = this->join(NULL);
        }
        server_fatal("thread::run() error, name = \"%s\"", m_name.c_str());
        return false;
    }
    server_info("work thread \"%s\" running", m_name.c_str());
    return true;
}

bool
work_thread::cancel ()
{
    kassert_r0(m_mutex && m_cond);

    m_stop_thread.store(true);
    m_new_task.store(false);
    m_done.store(true);

    bool _ret = this->thread::cancel();
    if_not (_ret)
        server_fatal("thread::cancel() error, name = \"%s\"", m_name.c_str());
    m_cond->broadcast();

    server_info("work thread \"%s\" canceled", m_name.c_str());
    return _ret;
}

bool
work_thread::task_done () const
{
    kassert_r0(m_running.load());

    return m_done.load();
}

bool
work_thread::recv_task (work_task **_task)
{
    kassert_r0(_task);
    kassert_r0(*_task);
    kassert_r0(!m_task);
    kassert_r0(m_mutex && m_cond);
    kassert_r0(m_running.load() && m_done.load());

    m_mutex->lock();
    m_task = *_task;
    *_task = NULL;
    m_done.store(false);
    m_new_task.store(true);
    m_mutex->unlock();
    m_cond->broadcast();

    return true;
}

msg *
work_thread::send_msg ()
{
    kassert_r0(m_task);
    kassert_r0(m_mutex && m_cond);
    kassert_r0(m_running.load() && m_done.load())

    m_mutex->lock();
    msg *_ret = m_task->send_reply_msg();
    m_task->release();
    m_task = NULL;
    m_mutex->unlock();
    m_cond->broadcast();

    return _ret;
}

void
work_thread::cleanup_lock (void *_args)
{
    kassert_r(_args);
    work_thread *_p = (work_thread *)_args;
    kassert_r(_p->m_mutex && _p->m_cond);

    _p->m_done.store(true);
    _p->m_mutex->unlock();
    _p->m_cond->broadcast();
}

int
work_thread::process (void *_args)
{
    kassert_r0(_args);

    work_thread *_p = (work_thread *)_args;
    pthread_cleanup_push(cleanup_lock, _args);
    int _ret = 0;

    while (!_p->m_stop_thread.load()) {
        // wait for new task
        pthread_testcancel();
        _p->m_mutex->lock();
        while (!_p->m_new_task.load()) {
            pthread_testcancel();
            if (!_p->m_new_task.load())
                _p->m_cond->wait();
            pthread_testcancel();
        }
        _p->m_new_task.store(false);
        _p->m_new_task.store(false);

        // process task
        work_task *_task = _p->m_task;
        if_not (_task)
            goto unlock;
        _ret = [_p] (work_task *_task) mutable -> bool {
            bool _ret = false;
            pthread_testcancel();
            _ret = _task->process();
            if (!_ret) {
                server_error("task process failed");
            }
            pthread_testcancel();
            return true;
        }(_p->m_task);

unlock:
        _p->m_mutex->unlock();
        _p->m_done.store(true);
        _p->m_cond->broadcast();
    }

    pthread_cleanup_pop(0);
    return 0;
}

__NAMESPACE_END
