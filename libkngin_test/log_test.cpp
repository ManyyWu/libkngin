#include <iostream>
#include "../libkngin/core/base/common.h"

#define KNGIN_FILENAME "libkngin_test/log_test.cpp"

using namespace k;

extern void
log_test ()
{
    logger().set_log_callback([] (const char *file, KNGIN_LOG_LEVEL level,
                                  const char *str, size_t len) {
         // can't call log_xxx
         std::cout << str << std::endl;
    });
    log_fatal("server_fatal: %d", 10);
    log_error("server_errror: %d", 10);
    log_warning("server_warning: %d", 10);
    log_info("server_info: %d", 10);
    log_debug("server_debug: %d", 10);
    if_not(!"test") {}
}
