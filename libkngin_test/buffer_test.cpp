#include <iostream>
#include "../libkngin/core/base/buffer.h"

#define KNGIN_FILENAME "libkngin_test/buffer_test.cpp"

using namespace std;
using namespace k;

void
buffer_test ()
{
    char _arr[10];
    cerr << in_buffer(_arr, 10).write_bytes("0123456789", 10).dump() << endl;
    cerr << out_buffer(_arr, 10).dump() << endl;
}
