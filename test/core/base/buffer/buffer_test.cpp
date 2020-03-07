#include "kngin/core/base/buffer.h"
#include <iostream>

using namespace std;

int
main () {
#define SIZE 100
  char arr[SIZE];
  {
    k::in_buffer buf(arr, SIZE);
    for (int i = 0; i < SIZE / 4; ++i)
      buf.write_int32(i);
    //buf.write_int8(0);
  }
  {
    k::out_buffer buf(arr, SIZE);
    while (!buf.eof())
      cout << buf.read_int32() << " ";
    cout << endl;
    //buf.read_int8();
  }

  return 0;
}
