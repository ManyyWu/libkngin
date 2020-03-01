#ifdef _WIN32
#include "pthread.h"
#else
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif
#include "core/base/common.h"
#include "core/base/system_error.h"
#include "core/base/exception.h"
#include "core/base/thread.h"

#define KNGIN_FILENAME "libkngin/core/base/thread.cpp"

KNGIN_NAMESPACE_K_BEGIN

thread::thread ()
try
    : m_name(""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
} catch (...) {
    log_fatal("thread() error");
    throw;
}

thread::thread (const char *name)
    try
    : m_name(name ? name : ""),
#ifdef _WIN32
      m_thr({nullptr, 0}),
#else
      m_thr(0),
#endif
      m_joined(false)
{
} catch (...) {
    log_fatal("thread() error");
    throw;
}

thread::~thread () noexcept
{
#ifdef _WIN32
    if (m_joined or !m_thr.p or !m_thr.x)
#else
    if (m_joined or !m_thr)
#endif
        return;
    auto ec = ::pthread_detach(m_thr);
    if (ec)
        log_fatal("::pthread_detach() error, name = \"%s\", %s",
                   m_name.c_str(), system_error_str(CERR(ec)).c_str());
    else
        log_info("thread \"%s\" has detached", m_name.c_str());
}

void
thread::run (thr_fn &&fn, crash_handler &&crash_handler /* = nullptr */)
{
    assert(fn);

    auto ec = ::pthread_create(
                   &m_thr, nullptr,
                   thread::start,
                   new thread::thread_data(
                       m_name,
                       std::move(fn),
                       std::move(crash_handler)
                   )
               );
    if (ec) {
        log_fatal("::pthread_create() error, name = \"%s\", %s",
                  m_name.c_str(), system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_create() error");
    }
}

int
thread::join ()
{
    assert(!equal_to(ptid()));

    thread_err_code code;
    auto ec = ::pthread_join(m_thr, &code.ptr);
    m_joined = true;
    if (ec) {
        log_fatal("::pthread_join(), name = \"%s\", %s",
                  m_name.c_str(), system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_join() error");
    }
    log_info("thread \"%s\" has joined with code: %d", m_name.c_str(), code.code);
    return code.code;
}

void
thread::cancel ()
{
    assert(!equal_to(ptid()));

    auto ec = ::pthread_cancel(m_thr);
    if (ec) {
        log_fatal("::pthread_cancel(), name = \"%s\", %s",
                  m_name.c_str(), system_error_str(CERR(ec)).c_str());
        throw k::exception("::pthread_cancel() error");
    }
    log_info("thread \"%s\" cancelled", m_name.c_str());
}

void *
thread::start (void *args) noexcept
{
    assert(args);
    thread_err_code code;
    bool crash = true;

    auto data = static_cast<thread_data *>(args);
    assert(data);
    pthread_cleanup_push(thread::cleanup, args);
    try {
        log_info("thread \"%s\" is running, tid = %" PRIu64, data->name.c_str(), thread::tid());
        if (data->fn)
            code.code = data->fn();
        crash = false;
    } catch (const k::exception &e) {
        log_fatal("caught an exception in thread \"%s\", %s",
                  data->name.c_str(), e.what());
        log_dump(e.dump().c_str());
    } catch (const std::exception &e) {
        log_fatal("caught an exception in thread \"%s\", %s",
                  data->name.c_str(), e.what());
    } catch (...) {
        log_fatal("caught an undefined exception in thread \"%s\"",
                  data->name.c_str());
    }
    if (crash) {
        //assert(0);
        if (data->handler)
            data->handler(thread::ptid());
    }
    pthread_cleanup_pop(1);
    return code.ptr;
}

void
thread::cleanup (void *args) noexcept
{
    assert(args);
    auto data = static_cast<thread_data *>(args);
    safe_release(data);
}

uint64_t
thread::tid () noexcept
{
#ifdef _WIN32
    return ::GetCurrentThreadId();
#else
    return ::syscall(SYS_gettid);
#endif
}

pthread_t
thread::ptid () noexcept
{
    return ::pthread_self();
}

void
thread::sleep (timestamp ms) noexcept
{
#ifdef _WIN32
    ::Sleep(static_cast<DWORD>(ms.value_uint()));
#else
    ::usleep(ms.value_uint() * 1000);
#endif
}

bool
thread::equal_to (pthread_t t) noexcept
{
    return ::pthread_equal(t, m_thr);
}

bool
thread::equal (pthread_t thr1, pthread_t thr2) noexcept
{
    return ::pthread_equal(thr1, thr2);
}

void
thread::exit (int err_code) noexcept
{
    ::pthread_exit(thread::thread_err_code(err_code).ptr);
}

KNGIN_NAMESPACE_K_END
