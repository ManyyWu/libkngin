#ifndef KNGIN_THREAD_H
#define KNGIN_THREAD_H

#include <cstring>
#include <atomic>
#include <memory>
#include <functional>
#include "core/define.h"
#include "core/noncopyable.h"
#include "core/timestamp.h"

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
public:
    typedef std::function<void (pthread_t)> crash_handler;

    typedef std::function<int (void)>       thr_fn;

public:
    class pimpl
        : public noncopyable,
          public std::enable_shared_from_this<pimpl> {
    public:
#ifdef _WIN32
        struct thread_t;
#endif

        union thread_err_code {
            void *ptr;

            int   code;

            thread_err_code () KNGIN_NOEXCP
            { ptr = nullptr; code = 0; }

            explicit
            thread_err_code (int _code) KNGIN_NOEXCP
            { ptr = nullptr; code = _code; }
        };

        struct thread_data {
            std::string   name;

            thr_fn        fn;

            crash_handler handler;

            thread_data (const std::string &_name,
                         thr_fn &&_fn,
                         crash_handler &&_handler)
                : name(_name), fn(std::move(_fn)), handler(std::move(_handler)) {}
        };

        typedef std::shared_ptr<pimpl> pimpl_ptr;

        typedef std::weak_ptr<pimpl>   pimpl_weak_ptr;

    public:
        pimpl         () = delete;

        explicit
        pimpl         (const char *_name);

        virtual
        ~pimpl        () KNGIN_NOEXCP;

    public:
        void
        run           (thr_fn &&_fn, crash_handler &&_crash_handler);

        int
        join          ();

        void
        cancel        ();

        bool
        joined        () const KNGIN_NOEXCP;

        pthread_t
        get_interface () const KNGIN_NOEXCP;

        const char *
        name          () const KNGIN_NOEXCP;

    public:
        bool
        equal_to      (pthread_t _t) KNGIN_NOEXCP;

    public:
        pimpl_ptr
        self          ()
        { return shared_from_this(); }

        pimpl_weak_ptr
        weak_self     ()
        { return shared_from_this(); }

    protected:
        static void *
        start         (void *_args) KNGIN_NOEXCP;

    protected:
        static void
        cleanup       (void *_args) KNGIN_NOEXCP;

    protected:
        const std::string m_name;

        pthread_t         m_thr;

        std::atomic_bool  m_joined;
    };

public:
    typedef pimpl::pimpl_ptr      pimpl_ptr;

    typedef pimpl::pimpl_weak_ptr pimpl_weak_ptr;

public:
    thread        ()
        : m_pimpl(std::make_shared<pimpl>(nullptr))  {}

    explicit
    thread        (const char *_name)
        : m_pimpl(std::make_shared<pimpl>(_name)) {}

    virtual
    ~thread       () = default;

public:
    void
    run           (thr_fn &&_fn, crash_handler &&_crash_handler = nullptr)
    { m_pimpl->run(std::move(_fn), std::move(_crash_handler)); }

    int
    join          ()
    { return m_pimpl->join(); }

    void
    cancel        ()
    { m_pimpl->cancel(); }

    bool
    joined        () const KNGIN_NOEXCP
    { return m_pimpl->joined(); }

    pthread_t
    get_interface () const KNGIN_NOEXCP
    { return m_pimpl->get_interface(); }

    const char *
    name          () const KNGIN_NOEXCP
    { return m_pimpl->name(); }

public:
    static uint64_t
    tid           () KNGIN_NOEXCP;

    static pthread_t
    ptid          () KNGIN_NOEXCP;

    static void
    sleep         (timestamp _ms) KNGIN_NOEXCP;

    static bool
    equal         (pthread_t _thr1, pthread_t _thr2) KNGIN_NOEXCP;

    static void
    exit          (int _err_code) KNGIN_NOEXCP
    { ::pthread_exit(pimpl::thread_err_code(_err_code).ptr); }

    bool
    equal_to      (pthread_t _t) KNGIN_NOEXCP
    { return m_pimpl->equal_to(_t); }

public:
    pimpl_weak_ptr
    weak_self     ()
    { return m_pimpl->weak_self(); }

private:
    pimpl::pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_THREAD_H */