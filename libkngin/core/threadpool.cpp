//#ifdef _WIN32
//#include <process.h>
//#else
//#include <unistd.h>
//#endif
//#include <algorithm>
//#include "define.h"
//#include "error.h"
//#include "threadpool.h"
//
#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/core/threadpoll.cpp"

//__NAMESPACE_BEGIN
//
//thread_pool::thread_pool (size_t _qsize, int _thr_max, time_t _alive)
//    : m_running(false), m_stop(true),
//      m_queue_size(_qsize), m_maxthread(_thr_max), m_alive(_alive),
//      m_task_queue(nullptr), m_msg_queue(nullptr),
//      m_pool_thread(nullptr), m_serial(0)
//{
//    check(_thr_max > 0 && _thr_max <= THREAD_NUM_MAX);
//    // log, throw
//}
//
//thread_pool::~thread_pool()
//{
//    this->stop();
//    this->clear();
//}
//
//bool
//thread_pool::run (int _num)
//{
//    check_r0(m_stop);
//    check_r0(!m_running);
//    char _name[50];
//
//    m_stop = false;
//    m_running = false;
//
//    // create manager thread
//    ::snprintf(_name, sizeof(_name),
//             "thread_pool_manager[%10d]", // format: thread_pool_manager[pid]
//             getpid());
//    knew(m_pool_thread, thread, (thread_pool::process, this, _name));
//    if_not (m_pool_thread)
//        return false;
//    bool _ret = m_pool_thread->run();
//    if_not (_ret)
//        goto fail;
//
//    // create thread pool
//    for (int i = 0; i < _num; i++) {
//        ::snprintf(_name, sizeof(_name),
//                 "work_thread[%10d-%19lld]", // format: work_thread[pid: serial]
//                 getpid(), (std::min)(m_serial++, SIZE_MAX));
//        work_thread *_thr = nullptr;
//        knew(_thr, work_thread, (_name));
//        if_not (_thr)
//            goto fail;
//        _ret = _thr->run();
//        if_not (_ret)
//            goto fail;
//    }
//
//    // init task queue
//    m_task_queue = task_queue::create(m_queue_size, true);
//    if_not (m_task_queue)
//        goto fail;
//
//    // init msg queue
//    m_msg_queue = msg_queue::create(m_queue_size, true);
//    if_not (m_msg_queue)
//        goto fail;
//
//    log_info("thread pool was created successfully");
//    return true;
//fail:
//    // log error
//    if (m_pool_thread && m_pool_thread->running()) {
//        m_pool_thread->cancel();
//        m_pool_thread->join(nullptr);
//        // log error
//    }
//    kdelete(m_pool_thread);
//
//    if (m_pool.size()) {
//        for (auto iter : m_pool) {
//            if (iter && iter->running()) {
//                iter->cancel();
//                iter->join(nullptr);
//            }
//            kdelete(iter);
//        }
//        pool_vector _v;
//        m_pool.swap(_v);
//    }
//
//    if (m_task_queue) {
//        m_task_queue->release();
//        m_task_queue = nullptr;
//    }
//    if (m_msg_queue) {
//        m_msg_queue->release();
//        m_msg_queue = nullptr;
//    }
//
//    log_fatal("thread pool create failed");
//    return _ret;
//}
//
//bool
//thread_pool::stop ()
//{
//    check_r0(m_pool_thread);
//
//    // stop manager thread
//    bool _ret = true;
//    if (m_pool_thread && m_pool_thread->running()) {
//        if_not (m_pool_thread->cancel()) {
//            log_fatal("pool manager thread cancel failed");
//            _ret = false;
//        }
//        if_not (m_pool_thread->join(nullptr)) {
//            log_fatal("pool manager thread join failed");
//            _ret = false;
//        }
//    }
//    kdelete(m_pool_thread);
//
//    // stop thread pool
//    for (auto iter : m_pool) {
//        if (iter && iter->running()) {
//            if_not (iter->cancel()) {
//                log_fatal("pool thread cancel failed");
//                _ret = false;
//            }
//            if_not (iter->join(nullptr)) {
//                log_fatal("pool thread join failed");
//                _ret = false;
//            }
//            kdelete(iter);
//        }
//    }
//    pool_vector _v;
//    m_pool.swap(_v);
//
//    log_info("thread pool stopped");
//    return _ret;
//}
//
//void
//thread_pool::clear ()
//{
//    check_r(m_msg_queue && m_task_queue);
//    if (m_task_queue) {
//        m_task_queue->lock();
//        m_task_queue->clear();
//        m_task_queue->unlock();
//    }
//    log_info("task queue are cleared up");
//    if (m_msg_queue) {
//        m_msg_queue->lock();
//        m_msg_queue->clear();
//        m_msg_queue->unlock();
//    }
//    log_info("message qeueu are cleared up");
//}
//
//bool
//thread_pool::commit (task_base **_task, time_t _ms /* = TIME_INFINITE */)
//{
//    if_not (!_task || !*_task)
//        return E_INVAL;
//    if_not (__time_valid(_ms))
//        return E_INVAL;
//    if (m_task_queue->lock(_ms))
//        return E_TIMEDOUT;
//
//    // check size
//    if (m_task_queue->size() > m_maxthread) {
//        m_task_queue->unlock();
//        return E_QUEUE_FULL;
//    }
//
//    // push
//    m_task_queue->push_front(_task);
//
//    m_task_queue->unlock();
//
//    return 0;
//}
//
//msg *
//thread_pool::get_msg (time_t _ms /* = TIME_INFINITE */)
//{
//    if_not (__time_valid(_ms))
//        return nullptr;
//}
//
//bool
//thread_pool::running () const
//{
//    return m_running;
//}
//
//int
//(thread_pool::max) () const
//{
//    return m_maxthread;
//}
//
//void
//thread_pool::set_max(int _max)
//{
//    // log info
//    m_maxthread = (std::min)((std::max)(_max, 1), THREAD_NUM_MAX);
//}
//
//size_t
//thread_pool::queue_max () const
//{
//    return m_queue_size;
//}
//
//void
//thread_pool::set_queue_max(size_t _max)
//{
//    // log info
//    m_queue_size = (std::min)((std::max)(_max, QUEUE_MIN), QUEUE_MAX);
//}
//
//int
//thread_pool::process (void *_args)
//{
//    check_r0(_args);
//    thread_pool *_pool = (thread_pool *)_args;
//    _pool->m_running = true;
//
//    // alive time
//    if (!_pool->m_alive){}
//    // max thread
//    // testcancel
//
//    _pool->m_running = false;
//    return 0;
//}
//
//__NAMESPACE_END
