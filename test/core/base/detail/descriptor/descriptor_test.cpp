#include "detail/core/base/descriptor.h"
#include "kngin/core/base/buffer.h"
#include <iostream>
#include <cstring>
#if !defined(KNGIN_SYSTEM__WIN32)
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

int
main () {
#define DATA "hello world!\n"
#if defined(KNGIN_NOT_SYSTEM_WIN32)
  int f = 0;

  {
    auto len = ::strnlen(DATA, 1024);
    k::out_buffer buf(DATA, len);
    k::detail::descriptor::write(f, buf);
  }
  {
    cerr << k::detail::descriptor::readable(f) << endl;
  }
  {
    k::detail::descriptor::set_nonblock(f, true);
    cerr << "nonblock:" << k::detail::descriptor::nonblock(f) << endl;
  }
  {
    k::detail::descriptor::set_closeexec(f, true);
    cerr << "closeexec:" << k::detail::descriptor::closeexec(f) << endl;
  }
  {
    k::detail::descriptor::close(f);
  }
  {
    cerr << k::detail::descriptor::readable(f) << endl;
  }
#endif

  return 0;
}
