#include <iostream>
#include "kngin/core/base/bits.h"

using namespace std;

void
show_bits (int val)
{
  for (size_t i = 0; i < TYPE_BITS(int); ++i)
    cout << k::is_bits_set(val, i);
  cout << endl;
}

int
main () {
  int val = 0;

  cout << "set all" << endl;
  for (size_t i = 0; i < TYPE_BITS(int); ++i) {
    val = k::set_bits(val, i);
    show_bits(val);
  }
  cout << "clear 31" << endl;
  val = k::clear_bits(val, 31);
  show_bits(val);
  cout << "clear all" << endl;
  for (size_t i = 0; i < TYPE_BITS(int); ++i)
    val = k::clear_bits(val, i);
  show_bits(val);
  cout << "set 0, 1, 2, 3, 31" << endl;
  val = k::set_bits(val, 0, 1, 2, 3, 31);
  show_bits(val);
  cout << "clear 1, 2, 3" << endl;
  val = k::clear_bits(val, 1, 2 ,3);
  show_bits(val);
  cout << "bits_value 0, 1, 2, 3" << endl;
  val = k::bits_value<int>(0, 1, 2 ,3, 31);
  show_bits(val);

  return 0;
}
