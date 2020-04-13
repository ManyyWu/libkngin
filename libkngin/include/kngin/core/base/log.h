#ifndef KNGIN_LOG_H
#define KNGIN_LOG_H

#include "kngin/core/base/logger.h"

namespace k {

// default logfile
#define KNGIN_DEFAULT_LOGFILE_NAME "kngin"
#define KNGIN_DEFAULT_LOGFILE_MODE log_mode_stderr
#define KNGIN_DEFAULT_LOGFILE_CALLBACK nullptr
extern logfile &log ();

#define log_fatal(fmt, ...) \
    k::log().write_fatal(KNGIN_LOG_FORMAT("FATAL  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define log_error(fmt, ...) \
    k::log().write_error(KNGIN_LOG_FORMAT("ERROR  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define log_warn(fmt, ...) \
    k::log().write_warning(KNGIN_LOG_FORMAT("WARNING", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define log_info(fmt, ...) \
    k::log().write_info(KNGIN_LOG_FORMAT_NOLINE("INFO   ", fmt), ##__VA_ARGS__)
#define log_debug(fmt, ...) \
    k::log().write_debug(KNGIN_LOG_FORMAT("DEBUG  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)

} /* namespace k */

#endif /* KNGIN_LOG_H */