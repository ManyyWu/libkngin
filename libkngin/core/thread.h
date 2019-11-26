#ifndef _THREAD_H_
#define _THREAD_H_

#include <cstring>
#include <atomic>
#include <memory>
#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "timestamp.h"
#include "system_error.h"

KNGIN_NAMESPACE_K_BEGIN

class thread : public noncopyable {
private:
#ifdef _WIN32
    struct thread_t;
#else
#endif

public:
    union thread_err_code{
        void *ptr;
        int   code;

        thread_err_code ()
        { ptr = nullptr; code = 0; }

        explicit
        thread_err_code (int _code)
        { ptr = nullptr; code = _code; }
    };

    typedef std::function<int (void)> thr_fn;

    struct thread_data {
        std::string       name;

        thr_fn            fn;

        thread_data (std::string &_name, thr_fn &&_fn)
            : name(_name), fn(std::move(_fn)) {}
    };

public:
    class pimpl
        : public noncopyable,
          public std::enable_shared_from_this<thread::pimpl> {
    public:
        pimpl         ();

        explicit
        pimpl         (const char *_name);

        virtual
        ~pimpl        ();

    public:
        void
        run           (thr_fn &&_fn);

        int
        join          ();

        bool
        cancel        ();

        bool
        joined        () const
        { return m_joined; }

        pthread_t
        get_interface () const
        { return m_thr; }

        const char *
        name          () const
        { return m_name.c_str(); }

    public:
        static uint64_t
        tid           ();

        static pthread_t
        ptid          ();

        static void
        sleep         (timestamp _ms);

        static void
        exit          (int _err_code);

        bool
        equal_to      (pthread_t _t);

    protected:
        static void *
        start         (void *_args);

    protected:
        static void
        cleanup       (void *_args);

    protected:
        const std::string m_name;

        pthread_t         m_thr;

        std::atomic<bool> m_joined;
    };

public:
    typedef std::shared_ptr<thread::pimpl> thread_pimpl_ptr;

public:
    thread        ()
        : m_pimpl(std::make_shared<thread::pimpl>())  {}

    explicit
    thread        (const char *_name)
        : m_pimpl(std::make_shared<thread::pimpl>(_name)) {}

    virtual
    ~thread       () = default;

public:
    void
    run           (thr_fn &&_fn)
    { m_pimpl->run(std::move(_fn)); }

    int
    join          ()
    { return m_pimpl->join(); }

    bool
    cancel        ();

    bool
    joined        () const;

    pthread_t
    get_interface () const;

    const char *
    name          () const;

public:
    static uint64_t
    tid           ();

    static pthread_t
    ptid          ();

    static void
    sleep         (timestamp _ms);

    static void
    exit          (int _err_code);

    bool
    equal_to      (pthread_t _t);

protected:
    static void *
    start         (void *_args);

protected:
    static void
    cleanup       (void *_args);

protected:
    thread_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _THREAD_H_ */