#ifndef KNGIN_CALLBACK_H
#define KNGIN_CALLBACK_H

#include "kngin/core/define.h"
#include <functional>

namespace k {

enum class log_level;
typedef std::function<void (log_level, const char *, size_t)> log_callback;

class timer_id;
class timestamp;
typedef std::function<void (const timer_id &, const timestamp &)> timeout_handler;

class in_buffer;
class out_buffer;
class error_code;
typedef std::function<void (const out_buffer &, size_t, const error_code &)> recv_handler;
typedef std::function<void (const in_buffer &, size_t, const error_code &)> sned_handler;

} /* namespace k */

#endif /* KNGIN_CALLBACK_H */