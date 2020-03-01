#ifndef KNGIN_EPOLLER_EVENT_H
#define KNGIN_EPOLLER_EVENT_H

#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER_EVENT

#include <memory>
#include <functional>

KNGIN_NAMESPACE_K_BEGIN

class iocp_poller;
class event_loop;
class iocp_event {
    friend class iocp_poller;
    friend class event_loop;

public:
    typedef std::shared_ptr<iocp_event> iocp_event_ptr;

    typedef std::weak_ptr<iocp_event>   iocp_event_weak_ptr;

    enum EVENT_TYPE {
        EVENT_TYPE_SOCKET = 1,
        EVENT_TYPE_FILE   = 2
    };

    enum IOCP_EVENT {
        IOCP_EVENT_IN  = 0x1,
        IOCP_EVENT_OUT = 0x2,
        IOCP_EVENT_PRI = 0x4,
        IOCP_EVENT_HUP = 0x8,
        IOCP_EVENT_ERR = 0x16,
    };

    struct per_io_data { 
        OVERLAPPED overlapped; 
        EVENT_TYPE type;
        size_t     bytes;
        void *     data;
    };

public:
    iocp_event     () = delete;

    explicit
    iocp_event     (HANDLE handle, EVENT_TYPE type,
                    uint8_t priority = UINT8_MAX) noexcept;

    iocp_event     (iocp_event &&e) noexcept;

    virtual
    ~iocp_event    () noexcept {};

public:
    HANDLE
    handle         () const noexcept
    { return handle_; }

    bool
    registed       () const noexcept
    { return registed_; }

    EVENT_TYPE
    type           () const noexcept
    { return type_; }

    uint8_t
    priority       () const noexcept
    { return priority_; }

protected:
    void
    set_registed   (bool on) noexcept
    { registed_ = on; }

    void
    set_index      (iocp_event_ptr &_e)
    { index_ = _e; }

    iocp_event_weak_ptr &
    index          () noexcept
    { return index_; }

protected:
    void
    set_flags      (uint32_t _flags) noexcept { flags_ = _flags; }
    uint32_t
    flags          () const          noexcept { return flags_; }
    void
    enable_read    ()                noexcept { flags_ |= IOCP_EVENT_IN; }
    void
    enable_write   ()                noexcept { flags_ |= IOCP_EVENT_OUT; }
    void
    enable_oob     ()                noexcept { flags_ |= IOCP_EVENT_PRI; }
    void
    disable_read   ()                noexcept { flags_ &= ~IOCP_EVENT_IN; }
    void
    disable_write  ()                noexcept { flags_ &= ~IOCP_EVENT_OUT; }
    void
    disable_oob    ()                noexcept { flags_ &= ~IOCP_EVENT_PRI; }
    void
    disable_all    ()                noexcept { flags_ = IOCP_EVENT_HUP | IOCP_EVENT_ERR; }
    bool
    pollin         () const          noexcept { return (flags_ & IOCP_EVENT_IN); }
    bool
    pollout        () const          noexcept { return (flags_ & IOCP_EVENT_OUT); }
    bool
    pollpri        () const          noexcept { return (flags_ & IOCP_EVENT_PRI); }

protected:
    virtual void
    on_events      (event_loop &_loop, uint32_t _flags) = 0;

private:
    uint32_t            flags_;

    HANDLE              handle_;

    bool                registed_;

    EVENT_TYPE          type_;

    uint8_t             priority_;
        
    iocp_event_weak_ptr index_;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_EVENT_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
