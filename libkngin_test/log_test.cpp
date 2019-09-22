#include "mylogfile.h"

using namespace k;

extern void
log_test ()
{
    logger();
    server_fatal("server_fatal: %d", 10);
    server_error("server_errror: %d", 10);
    server_warning("server_warning: %d", 10);
    server_info("server_info: %d", 10);
    server_debug("server_debug: %d", 10);
    http_fatal("http fatal: %d", 10);
    http_error("http errror: %d", 10);
    http_warning("http_warning: %d", 10);
    http_info("http_info: %d", 10);
    http_debug("http_debug: %d", 10);
    //if_not(!"test") {}
}
