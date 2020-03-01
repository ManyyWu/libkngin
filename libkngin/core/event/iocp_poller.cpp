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
    : events_(),
      mutex_(),
      iocp_handle_(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1))
{
    if (INVALID_HANDLE_VALUE == iocp_handle_)
        throw k::system_error("::CreateIoCompletionPort() error");
} catch (...) {
    log_fatal("iocp_poller::iocp_poller() error");
    throw;
}

iocp_poller::~iocp_poller () noexcept
{
    if (INVALID_HANDLE_VALUE != iocp_handle_)
        ignore_excp(this->close());
}

size_t
iocp_poller::wait (iocp_event_set &events, timestamp ms)
{
    return (g_have_get_iocp_status_ex ? poll(events, ms) : poll_wine(events, ms));
}

void
iocp_poller::wakeup ()
{
    if (iocp_handle_) {
        cond_sys_err(
            FALSE == ::PostQueuedCompletionStatus(iocp_handle_, -1, 0, nullptr),
            "::PostQueuedCompletionStatus() error"
        );
    }
}

size_t
iocp_poller::poll (iocp_event_set &events, timestamp ms)
{
    ULONG            size = 0;
    ULONG            count = 0;
    ULONG_PTR        key = 0;
    OVERLAPPED_ENTRY overlappeds[KNGIN_RESERVED_POLLELR_EVENT];

    ::GetQueuedCompletionStatusEx(iocp_handle_,
                                  overlappeds,
                                  KNGIN_RESERVED_POLLELR_EVENT,
                                  &size,
                                  ms.value_uint(),
                                  FALSE);
    if (size > 0) {
        count = size;
        for (size_t i = 0; i < size; ++i) {
            auto overlapped = overlappeds[i].lpOverlapped;
            if (overlapped) {
                assert(events[i] = (per_io_data *)CONTAINING_RECORD(
                                         &overlappeds[i], per_io_data, overlapped));
            } else {
                --count; // wakeup;
            }
        }
    } else if (WAIT_TIMEOUT == last_error()) {
        return 0;
    } else {
        throw k::system_error("::GetQueuedCompletionStatusEx() error");
    }
    return count;
}

size_t
iocp_poller::poll_wine (iocp_event_set &events, timestamp ms)
{
    auto   size = events.size();
    size_t count = 0;
    while (count < size) {
        DWORD       bytes = 0;
        ULONG_PTR   key = 0;
        OVERLAPPED *overlapped = nullptr;

        bool ok = ::GetQueuedCompletionStatus(iocp_handle_,
                                    &bytes,
                                    &key,
                                    &overlapped,
                                    ms.value_uint());
        auto err = last_error();
        if (ok) {
            if (overlapped)
                assert(events[count++] = (per_io_data *)CONTAINING_RECORD(
                                               overlapped, per_io_data, overlapped));
            else
                break; // wakeup
        } else if (WAIT_TIMEOUT == err) {
            return 0;
        } else {
            throw k::system_error("::GetQueuedCompletionStatus() error");
        }
    }
    return count;
}

void
iocp_poller::close ()
{
    assert(INVALID_HANDLE_VALUE != iocp_handle_);
    cond_sys_err(
        FALSE == ::CloseHandle(iocp_handle_),
        "::CloseHandle() error"
    );
    events_.clear();
    if (events_.size())
        log_warning("there are still have %" PRIu64
                    " undeleted iocp event in epoller", events_.size());
}

void
iocp_poller::register_event (iocp_event_ptr e)
{
    assert(e);
    assert(INVALID_HANDLE_VALUE == iocp_handle_);
    {
        local_lock lock(mutex_);
        assert(!e->registed());
        events_.push_back(e);
        e->set_registed(true);
        e->set_index(events_.back());
    }
}

void
iocp_poller::remove_event (iocp_event &e)
{
    assert(INVALID_HANDLE_VALUE == iocp_handle_);
    {
        local_lock lock(mutex_);
        assert(e.registed());
        e.set_registed(false);
        if (auto index = e.index().lock())
            events_.remove(index);
    }
}

void
iocp_poller::modify_event (iocp_event &e)
{
    assert(INVALID_HANDLE_VALUE == iocp_handle_);
}

bool
iocp_poller::registed (iocp_event &e) noexcept
{
    assert(INVALID_HANDLE_VALUE == iocp_handle_);
    return (!e.registed());
}

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_FLAG_HAVE_EPOLLER */
