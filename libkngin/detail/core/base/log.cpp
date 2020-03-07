#include "kngin/core/base/log.h"

KNGIN_NAMESPACE_K_BEGIN

// default logfile
extern logger &query_logger ();

logfile &
log () {
  static logfile &default_log =
      query_logger().add_logfile(KNGIN_DEFAULT_LOGFILE_NAME,
                                 KNGIN_DEFAULT_LOGFILE_MODE,
                                 KNGIN_DEFAULT_LOGFILE_CALLBACK);
  return default_log;
}

KNGIN_NAMESPACE_K_END
