#ifndef KNGIN_LOG_H
#define KNGIN_LOG_H

#include "kngin/core/base/logger.h"

KNGIN_NAMESPACE_K_BEGIN

// default logfile
#define KNGIN_DEFAULT_LOGFILE_NAME "kngin"
#define KNGIN_DEFAULT_LOGFILE_MODE KNGIN_LOG_MODE_STDERR
#define KNGIN_DEFAULT_LOGFILE_CALLBACK nullptr
extern logfile &log ();

#define fatal(fmt, ...) \
    k::log().write_fatal(KNGIN_LOG_FORMAT("FATAL  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define error(fmt, ...) \
    k::log().write_error(KNGIN_LOG_FORMAT("ERROR  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define warning(fmt, ...) \
    k::log().write_warning(KNGIN_LOG_FORMAT("WARNING", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)
#define info(fmt, ...) \
    k::log().write_info(KNGIN_LOG_FORMAT_NOLINE("INFO   ", fmt), ##__VA_ARGS__)
#define debug(fmt, ...) \
    k::log().write_debug(KNGIN_LOG_FORMAT("DEBUG  ", fmt), FUNCTION, FILENAME, LINE, ##__VA_ARGS__)

KNGIN_NAMESPACE_K_END

#endif /* KNGIN_LOG_H */