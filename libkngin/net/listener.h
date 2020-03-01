#ifndef KNGIN_LISTENER_H
#define KNGIN_LISTENER_H

#include <functional>
#include <cassert>
#include <memory>
#include "core/event/event_loop.h"
#include "net/socket.h"
#include "net/address.h"

KNGIN_NAMESPACE_K_BEGIN

class listener
    : public epoller_event,
      public std::enable_shared_from_this<listener> {
public:
    typedef std::function<void (k::socket &&)>    accept_handler;

    typedef std::function<void (error_code)>      error_handler;

    typedef std::shared_ptr<listener>             listener_ptr;

    typedef std::weak_ptr<listener>               listener_weak_ptr;

public:
    listener       () = delete;

    listener       (event_loop &loop, k::socket &&socket,
                    const std::string &name, uint16_t port,
                    int backlog,
                    accept_handler &&new_ssesion_handler,
                    error_handler &&error_handler);

    virtual
    ~listener      () noexcept;

public:
    virtual void
    close          (bool sync = true);

public:
    event_loop *
    loop           () noexcept
    { return loop_; }

    const event_loop *
    loop           () const noexcept
    { return loop_; }

    listener_ptr
    self           ()
    { return shared_from_this(); }

    listener_weak_ptr
    weak_ptr            ()
    { return weak_from_this(); }

    const address &
    listen_addr    () const
    { return listen_addr_; }

private:
    void
    parse_addr     (const std::string &name, uint16_t port);

private:
    virtual void
    on_events      (event_loop &loop, uint32_t flags);

    void
    on_read        ();

    void
    on_close       ();

private:
    event_loop *     loop_;

    k::socket        socket_;

    std::atomic_bool closed_;

    address          listen_addr_;

    accept_handler   accept_handler_;

    error_handler    error_handler_;

    filefd           idle_file_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LISTENER_H */

