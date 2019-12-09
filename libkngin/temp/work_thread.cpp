//#ifdef _WIN32
//#include "pthreads_win32/include/pthread.h"
//#else
//#include <pthread.h>
//#endif
//#include <cstdio>
//#include "define.h"
//#include "work_thread.h"
//#include "work_task.h"
//#include "thread.h"
//#include "msg.h"
//
//KNGIN_NAMESPACE_K_BEGIN
//
//work_thread::work_thread (const char *_name)
//    : thread(work_thread::process, this, _name ? _name : "work_thread"),
//      m_done(true), m_new_task(false),
//      m_stop_thread(false), m_task(nullptr)
//{
//}
//
//work_thread::~work_thread ()
//{
//    if (m_task)
//        m_task->release();
//    log_info("work_thread \"%s\" closed", m_name.c_str());
//}
//
//bool
//work_thread::run ()
//{
//    assert(!m_running);
//
//    bool _ret = true;
//    _ret = this->thread::run();
//    if_not (_ret) {
//fail:
//        if (this->thread::running()) {
//            _ret = this->cancel();
//            _ret = this->join();
//        }
//        log_fatal("::thread::run() error, name = \"%s\"", m_name.c_str());
//        return false;
//    }
//    log_info("work thread \"%s\" is running", m_name.c_str());
//    return true;
//}
//
//bool
//work_thread::cancel ()
//{
//    assert(m_mutex && m_cond);
//
//    m_stop_thread = true;
//    m_new_task = false;
//    m_done = true;
//    bool _ret = this->thread::cancel();
//    if_not (_ret)
//        log_fatal("::thread::cancel() error, name = \"%s\"", m_name.c_str());
//    m_cond.broadcast();
//
//    log_info("work thread \"%s\" cancelled", m_name.c_str());
//    return _ret;
//}
//
//bool
//work_thread::task_done () const
//{
//    assert(m_running);
//
//    return m_done;
//}
//
//bool
//work_thread::recv_task (work_task **_task)
//{
//    assert(_task);
//    assert(*_task);
//    assert(!m_task);
//    assert(m_mutex && m_cond);
//    assert(m_running && m_done);
//
//    if (!m_mutex.trylock())
//        return false;
//    m_task = *_task;
//    *_task = nullptr;
//    m_done = false;
//    m_new_task = true;
//    m_mutex.unlock();
//    m_cond.broadcast();
//
//    return true;
//}
//
//msg *
//work_thread::send_msg ()
//{
//    assert(m_task);
//    assert(m_mutex && m_cond);
//    assert(m_running && m_done)
//
//    m_mutex.lock();
//    msg *_ret = m_task->send_reply_msg();
//    m_task->release();
//    m_task = nullptr;
//    m_mutex.unlock();
//    m_cond.broadcast();
//
//    return _ret;
//}
//
//void
//work_thread::cleanup_lock (void *_args)
//{
//    assert(_args);
//    work_thread *_p = (work_thread *)_args;
//    assert(_p->m_mutex && _p->m_cond);
//
//    _p->m_done = true;
//    _p->m_mutex.unlock();
//    _p->m_cond.broadcast();
//}
//
//int
//work_thread::process (void *_args)
//{
//    assert(_args);
//
//    work_thread *_p = (work_thread *)_args;
//    pthread_cleanup_push(cleanup_lock, _args);
//    int _ret = 0;
//
//    while (!_p->m_stop_thread) {
//        // wait for new task
//        pthread_testcancel();
//        _p->m_mutex.lock();
//        while (!_p->m_new_task) {
//            pthread_testcancel();
//            if (!_p->m_new_task)
//                _p->m_cond.wait();
//            pthread_testcancel();
//        }
//        _p->m_new_task = false;
//
//        // process task
//        work_task *_task = _p->m_task;
//        if_not (_task)
//            goto unlock;
//        _ret = [_p] (work_task *_task) mutable -> bool {
//            bool _ret = false;
//            pthread_testcancel();
//            _ret = _task->process();
//            if (!_ret) {
//                log_error("task process failed");
//            }
//            pthread_testcancel();
//            return true;
//        }(_p->m_task);
//
//unlock:
//        _p->m_mutex.unlock();
//        _p->m_done = true;
//        _p->m_cond.broadcast();
//    }
//
//    pthread_cleanup_pop(0);
//    return 0;
//}
//
//KNGIN_NAMESPACE_K_END
