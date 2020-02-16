#include "core/base/define.h"
#ifndef KNGIN_FLAG_HAVE_EPOLLER

#ifdef _WIN32
#else
#include <sys/epoll.h>
#endif
#include "core/base/common.h"
#include "core/event/win_utils.h"
#include "core/base/exception.h"
#include "core/base/system_error.h"
#include "core/event/iocp_poller.h"

#define KNGIN_FILENAME "libkngin/core/event/iocp_poller.cpp"

KNGIN_NAMESPACE_K_BEGIN

iocp_poller::iocp_poller ()
    try
    : m_events(),
      m_mutex(),
      m_iocp_handle(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1))
{
    if (INVALID_HANDLE_VALUE == m_iocp_handle)
        throw k::system_error("::CreateIoCompletionPort() error");
} catch (...) {
    log_fatal("iocp_poller::iocp_poller() error");
    throw;
}

iocp_poller::~iocp_poller () KNGIN_NOEXCP
{
    if (INVALID_HANDLE_VALUE != m_iocp_handle)
        assert(::CloseHandle(m_iocp_handle));
}

size_t
iocp_poller::wait (iocp_event_set &_events, timestamp _ms)
{
    if (g_have_get_iocp_status_ex)
        poll(_events, _ms);
    else
        poll_wine(_events, _ms);

    return 0;
}

void
iocp_poller::wakeup ()
{
    if (m_iocp_handle)
        ::PostQueuedCompletionStatus(m_iocp_handle, 0, 0, nullptr);
}

size_t
iocp_poller::poll (iocp_event_set &_events, timestamp _ms)
{
    ULONG            _size = 0;
    ULONG            _count = 0;
    ULONG_PTR        _key = 0;
    OVERLAPPED_ENTRY _overlappeds[KNGIN_RESERVED_POLLELR_EVENT];

    ::GetQueuedCompletionStatusEx(m_iocp_handle,
                                  _overlappeds,
                                  KNGIN_RESERVED_POLLELR_EVENT,
                                  &_size,
                                  _ms.value_uint(),
                                  FALSE);
    if (_size > 0) {
        _count = _size;
        for (size_t _i = 0; _i < _size; ++_i) {
            auto _overlapped = _overlappeds[_i].lpOverlapped;
            if (_overlapped) {
                assert(_events[_i] = (per_io_data *)CONTAINING_RECORD(
                                        &_overlappeds[_i], per_io_data, overlapped));
            } else {
                --_count; // wakeup;
            }
        }
    } else if (WAIT_TIMEOUT == last_errno()) {
        return 0;
    } else {
        throw k::system_error("::GetQueuedCompletionStatusEx() error");
    }
    return _count;
}

size_t
iocp_poller::poll_wine (iocp_event_set &_events, timestamp _ms)
{
    auto   _size = _events.size();
    size_t _count = 0;
    while (_count < _size) {
        DWORD       _bytes = 0;
        ULONG_PTR   _key = 0;
        OVERLAPPED *_overlapped = nullptr;

        bool _ok = ::GetQueuedCompletionStatus(m_iocp_handle,
                                    &_bytes,
                                    &_key,
                                    &_overlapped,
                                    _ms.value_uint());
        auto _err = last_errno();
        if (_ok) {
            if (_overlapped)
                assert(_events[_count++] = (per_io_data  *)CONTAINING_RECORD(
                                               _overlapped, per_io_data, overlapped));
            else
                break; // wakeup
        } else if (WAIT_TIMEOUT == _err) {
            return 0;
        } else {
            throw k::system_error("::GetQueuedCompletionStatus() error");
        }
    }
    return _count;
}

void
iocp_poller::close ()
{
    assert(INVALID_HANDLE_VALUE == m_iocp_handle);
    assert(::CloseHandle(m_iocp_handle));
    m_events.clear();
    if (m_events.size())
        log_warning("there are still have %" PRIu64
                    " undeleted iocp event in epoller", m_events.size());
}

void
iocp_poller::register_event (iocp_event_ptr _e)
{
    assert(_e);
    assert(INVALID_HANDLE_VALUE == m_iocp_handle);
    {
        local_lock _lock(m_mutex);
        assert(!_e->registed());
        m_events.push_back(_e);
        _e->set_registed(true);
        _e->set_index(m_events.back());
    }
}

void
iocp_poller::remove_event (iocp_event &_e)
{
    assert(INVALID_HANDLE_VALUE == m_iocp_handle);
    {
        local_lock _lock(m_mutex);
        assert(_e.registed());
        _e.set_registed(false);
        if (auto _index = _e.index().lock())
            m_events.remove(_index);
    }
}

void
iocp_poller::modify_event (iocp_event &_e)
{
    assert(INVALID_HANDLE_VALUE == m_iocp_handle);
}

bool
iocp_poller::registed (iocp_event &_e) KNGIN_NOEXCP
{
    assert(INVALID_HANDLE_VALUE == m_iocp_handle);
    return (!_e.registed());
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER */
