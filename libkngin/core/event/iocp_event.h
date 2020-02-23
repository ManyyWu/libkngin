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
    iocp_event     (HANDLE _handle, EVENT_TYPE _type, 
                    uint8_t _priority = UINT8_MAX) KNGIN_NOEXCP;

    iocp_event     (iocp_event &&_e) KNGIN_NOEXCP;

    virtual
    ~iocp_event    () KNGIN_NOEXCP {};

public:
    HANDLE
    handle         () const KNGIN_NOEXCP
    { return m_handle; }

    bool
    registed       () const KNGIN_NOEXCP
    { return m_registed; }

    EVENT_TYPE
    type           () const KNGIN_NOEXCP
    { return m_type; }

    uint8_t
    priority       () const KNGIN_NOEXCP
    { return m_priority; }

protected:
    void
    set_registed   (bool _on) KNGIN_NOEXCP
    { m_registed = _on; }

    void
    set_index      (iocp_event_ptr &_e)
    { m_index = _e; }

    iocp_event_weak_ptr &
    index          () KNGIN_NOEXCP
    { return m_index; }

protected:
    void
    set_flags      (uint32_t _flags) KNGIN_NOEXCP { m_flags = _flags; }
    uint32_t
    flags          () const          KNGIN_NOEXCP { return m_flags; }
    void
    enable_read    ()                KNGIN_NOEXCP { m_flags |= IOCP_EVENT_IN; }
    void
    enable_write   ()                KNGIN_NOEXCP { m_flags |= IOCP_EVENT_OUT; }
    void
    enable_oob     ()                KNGIN_NOEXCP { m_flags |= IOCP_EVENT_PRI; }
    void
    disable_read   ()                KNGIN_NOEXCP { m_flags &= ~IOCP_EVENT_IN; }
    void
    disable_write  ()                KNGIN_NOEXCP { m_flags &= ~IOCP_EVENT_OUT; }
    void
    disable_oob    ()                KNGIN_NOEXCP { m_flags &= ~IOCP_EVENT_PRI; }
    void
    disable_all    ()                KNGIN_NOEXCP { m_flags = IOCP_EVENT_HUP | IOCP_EVENT_ERR; }
    bool
    pollin         () const          KNGIN_NOEXCP { return (m_flags & IOCP_EVENT_IN); }
    bool
    pollout        () const          KNGIN_NOEXCP { return (m_flags & IOCP_EVENT_OUT); }
    bool
    pollpri        () const          KNGIN_NOEXCP { return (m_flags & IOCP_EVENT_PRI); }

protected:
    virtual void
    on_events      (event_loop &_loop, uint32_t _flags) = 0;

private:
    uint32_t            m_flags;

    HANDLE              m_handle;

    bool                m_registed;

    EVENT_TYPE          m_type;

    uint8_t             m_priority;
        
    iocp_event_weak_ptr m_index;
};

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_EPOLLER_EVENT_H */

#endif /* KNGIN_FLAG_HAVE_EPOLLER_EVENT */
