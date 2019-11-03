#include "mylogfile.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin_test/log_test.cpp"

using namespace k;

extern void
log_test ()
{
    log_fatal("server_fatal: %d", 10);
    log_error("server_errror: %d", 10);
    log_warning("server_warning: %d", 10);
    log_info("server_info: %d", 10);
    log_debug("server_debug: %d", 10);
    if_not(!"test") {}
}
