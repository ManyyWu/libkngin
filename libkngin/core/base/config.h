#ifndef KNGIN_CONFIG_H
#define KNGIN_CONFIG_H

#include <cstdint>
#include <climits>

#define ON  true
#define OFF false

// for MSVC
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

// for log
#define KNGIN_LOG_RELATIVE_PATH         ON
#define KNGIN_ASYNC_LOGGER              OFF
#define KNGIN_ASYNC_LOGGER_TIMEOUT      3000ULL

// for queues
#define KNGIN_QUEUE_MAX                 size_t(SIZE_MAX)
#define KNGIN_QUEUE_MIN                 size_t(1)

// for timer
#define KNGIN_USE_TIMERFD               OFF
#define KNGIN_TIMER_REMAINING_PRESISION 1ULL

// for session
#define KNGIN_ONE_LOOP_PER_SESSION      ON
#define KNGIN_SESSION_TEMP_CALLBACK     ON
#define KNGIN_SESSION_ET_MODE           ON

// for epoller
#define KNGIN_RESERVED_POLLELR_EVENT    64
#define KNGIN_DEFAULT_POLLER_TIMEOUT    3000ULL

#endif /* KNGIN_CONFIG_H */
