#ifndef KNGIN_LISTENER_H
#define KNGIN_LISTENER_H

#include <functional>
#include <cassert>
#include <memory>
#include <system_error>
#include "core/noncopyable.h"
#include "net/event_loop.h"
#include "net/socket.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

class listener
    : public epoller_event,
      public std::enable_shared_from_this<listener> {
public:
    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (std::error_code)> error_handler;

    typedef std::shared_ptr<listener>             listener_ptr;

public:
    listener       () = delete;

    listener       (event_loop &_loop, k::socket &&_socket,
                    const std::string &_name, uint16_t _port,
                    int _backlog,
                    accept_handler &&_new_ssesion_handler,
                    error_handler &&_error_handler);

    virtual
    ~listener      () KNGIN_NOEXCP;

public:
    virtual void
    close          (bool _sync = true);

public:
    event_loop *
    loop           () KNGIN_NOEXCP
    { return m_loop; }

    const event_loop *
    loop           () const KNGIN_NOEXCP
    { return m_loop; }

    listener_ptr
    self           ()
    { return shared_from_this(); }

    const address &
    listen_addr    () const
    { return m_listen_addr; }

private:
    void
    parse_addr     (const std::string &_name, uint16_t _port);

private:
    virtual void
    on_events      (event_loop &_loop, uint32_t _flags);

    void
    on_read        ();

    void
    on_close       ();

private:
    event_loop *     m_loop;

    k::socket        m_socket;

    std::atomic_bool m_closed;

    address          m_listen_addr;

    accept_handler   m_accept_handler;

    error_handler    m_error_handler;

    filefd           m_idle_file;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LISTENER_H */

