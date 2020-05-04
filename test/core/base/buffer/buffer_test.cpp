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
      buf.write(i);
    //buf.write(char(0));
  }
  {
    k::out_buffer buf(arr, SIZE);
    while (!buf.eof())
      cout << buf.read_int32() << " ";
    cout << endl;
    //buf.read_int8();
  }
  {
    k::in_buffer buf(arr, SIZE);
    buf.write("hello world!\n", 13);
    cout << arr << endl;
  }

  return 0;
}
