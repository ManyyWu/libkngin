#include "kngin/core/base/log.h"
#include <iostream>

using namespace std;

int
main () {
  auto &mylogfile = k::query_logger().add_logfile("test", KNGIN_LOG_MODE_STDERR | KNGIN_LOG_MODE_CALLBACK,
      [] (const char *file, k::log_level level, const char *data, size_t len) {
    cerr << "file: " << file << "\nlevel: " << static_cast<int>(level) << "\ndata: " << data << endl;
  });
  mylogfile.write_fatal(" %s\n", "test");
  mylogfile.write_error(" test\n");
  mylogfile.write_warning(" test\n");
  mylogfile.write_info(" test\n");
  mylogfile.write_debug(" test\n");

  log_fatal("%s", "test");
  log_error("test");
  log_warn("test");
  log_info("test");
  log_debug("test");
  return 0;
}
