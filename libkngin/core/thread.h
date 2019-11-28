#ifndef _THREAD_H_
#define _THREAD_H_

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
    typedef std::function<int (void)> thr_fn;

public:
    class pimpl
        : public noncopyable,
          public std::enable_shared_from_this<thread::pimpl> {
    public:
#ifdef _WIN32
        struct thread_t;
#endif

        union thread_err_code {
            void *ptr;

            int   code;

            thread_err_code () KNGIN_NOEXP
            { ptr = nullptr; code = 0; }

            explicit
            thread_err_code (int _code) KNGIN_NOEXP
            { ptr = nullptr; code = _code; }
        };

        struct thread_data {
            const std::string name;

            thr_fn            fn;

            thread_data (const std::string &_name, thr_fn &&_fn) KNGIN_EXP
                    : name(_name), fn(std::move(_fn)) {}
        };

    public:
        pimpl         () KNGIN_EXP;

        explicit
        pimpl         (const char *_name) KNGIN_EXP;

        virtual
        ~pimpl        () KNGIN_NOEXP;

    public:
        void
        run           (thr_fn &&_fn) KNGIN_EXP;

        int
        join          () KNGIN_EXP;

        void
        cancel        () KNGIN_EXP;

        bool
        joined        () const KNGIN_NOEXP;

        pthread_t
        get_interface () const KNGIN_NOEXP;

        const char *
        name          () const KNGIN_NOEXP;

    public:
        bool
        equal_to      (pthread_t _t) KNGIN_NOEXP;

    public:
        std::shared_ptr<thread::pimpl>
        self          () KNGIN_EXP
        { return shared_from_this(); }

    protected:
        static void *
        start         (void *_args) KNGIN_NOEXP;

    protected:
        static void
        cleanup       (void *_args) KNGIN_NOEXP;

    protected:
        const std::string m_name;

        pthread_t         m_thr;

        std::atomic<bool> m_joined;
    };

    typedef std::shared_ptr<thread::pimpl> thread_pimpl_ptr;

public:
    thread        () KNGIN_EXP
        : m_pimpl(std::make_shared<thread::pimpl>())  {}

    explicit
    thread        (const char *_name) KNGIN_EXP
        : m_pimpl(std::make_shared<thread::pimpl>(_name)) {}

    virtual
    ~thread       () KNGIN_NOEXP = default;

public:
    void
    run           (thr_fn &&_fn) KNGIN_EXP
    { m_pimpl->run(std::move(_fn)); }

    int
    join          () KNGIN_EXP
    { return m_pimpl->join(); }

    void
    cancel        () KNGIN_EXP
    { m_pimpl->cancel(); }

    bool
    joined        () const KNGIN_NOEXP
    { return m_pimpl->joined(); }

    pthread_t
    get_interface () const KNGIN_NOEXP
    { return m_pimpl->get_interface(); }

    const char *
    name          () const KNGIN_NOEXP
    { return m_pimpl->name(); }

public:
    static uint64_t
    tid           () KNGIN_NOEXP;

    static pthread_t
    ptid          () KNGIN_NOEXP;

    static void
    sleep         (timestamp _ms) KNGIN_NOEXP;

    static void
    exit          (int _err_code) KNGIN_NOEXP
    { ::pthread_exit(thread::pimpl::thread_err_code(_err_code).ptr); }

    bool
    equal_to      (pthread_t _t) KNGIN_NOEXP
    { return m_pimpl->equal_to(_t); }

public:
    thread_pimpl_ptr
    pimpl         () KNGIN_EXP
    { return m_pimpl; }

protected:
    thread_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _THREAD_H_ */