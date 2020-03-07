#include "detail/core/base/descriptor.h"
#include "kngin/core/base/buffer.h"
#include <iostream>

using namespace std;

int
main () {
  k::detail::fd_type in = 0;

  cerr << "read:" << endl;
  {
    char arr[11];
    k::in_buffer buf(arr, 10);
    k::detail::readn(in, buf);
    arr[10] = 0;
    cerr << arr;
  }

  cerr << endl << "read:" << endl;
  {
    char arr[1024];
    k::in_buffer buf(arr, 1024);
    k::detail::read(in, buf);
    buf.write_int8(0);
    cerr << arr;
  }

  return 0;
}
