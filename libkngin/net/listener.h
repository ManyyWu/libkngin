#ifndef _LISTENER_H_
#define _LISTENER_H_

#include <functional>
#include <cassert>
#include <memory>
#include <system_error>
#include "core/noncopyable.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

class listener : public noncopyable {
public:
    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (std::error_code)> close_handler;

private:
    class pimpl
        : public epoller_event,
          public std::enable_shared_from_this<listener::pimpl> {
    public:
        pimpl         () = delete;

        pimpl         (event_loop &_loop, k::socket &&_socket);

        ~pimpl        () KNGIN_NOEXP;

    public:
        void
        bind          (const address &_listen_addr);

        void
        bind          (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP;

        void
        listen        (int _backlog, accept_handler &&_new_ssesion_handler,
                       close_handler &&_close_handler);

        void
        listen        (int _backlog, std::error_code &_ec,
                       accept_handler &&_handler,
                       close_handler &&close_handler) KNGIN_NOEXP;

        void
        close         (bool _blocking = true);

    public:
        event_loop_pimpl_ptr &
        loop          () KNGIN_NOEXP
        { return m_loop; }

        std::shared_ptr<listener::pimpl>
        self     ()
        { return shared_from_this(); }

    private:
        virtual void
        on_read       ();

        void
        on_close      (std::error_code _ec);

        virtual void
        on_error      ();

    private:
        event_loop_pimpl_ptr m_loop;

        k::socket            m_socket;

        std::atomic<bool>    m_closed;

        address              m_listen_addr;

        accept_handler       m_accept_handler;

        close_handler        m_close_handler;

        filefd               m_idle_file;
    };

public:
    typedef listener::pimpl                 listener_pimpl;

    typedef std::shared_ptr<listener_pimpl> listener_pimpl_ptr;

public:
    listener  () = delete;

    listener  (event_loop &_loop, k::socket &&_socket)
        : m_pimpl(std::make_shared<listener_pimpl>(
                      _loop, std::move(_socket))) {}

    ~listener () KNGIN_NOEXP {}

public:
    void
    bind      (const address &_listen_addr)
    { m_pimpl->bind(_listen_addr); }

    void
    bind      (const address &_listen_addr, std::error_code &_ec) KNGIN_NOEXP
    { m_pimpl->bind(_listen_addr, _ec); }

    void
    listen    (int _backlog,
               accept_handler &&_new_ssesion_handler,
               close_handler &&_close_handler)
    { m_pimpl->listen(_backlog, std::move(_new_ssesion_handler), std::move(_close_handler)); }

    void
    listen    (int _backlog, std::error_code &_ec,
               accept_handler &&_new_ssesion_handler,
               close_handler &&_close_handler) KNGIN_NOEXP
    { m_pimpl->listen(_backlog, _ec, std::move(_new_ssesion_handler), std::move(_close_handler)); }

    void
    close     (bool _blocking = true)
    { m_pimpl->close(_blocking); }

public:
    void
    check_thread  () const KNGIN_NOEXP
    { m_pimpl->loop()->check_thread(); }

public:
    listener_pimpl_ptr
    pimpl         ()
    { return m_pimpl; }

private:
    listener_pimpl_ptr m_pimpl;
};

KNGIN_NAMESPACE_K_END

#endif /* _LISTENER_H_ */

