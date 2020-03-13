#include "detail/core/base/list.h"
#include <iostream>

using namespace std;

class mydata :
    public k::detail::entry_base<mydata> {
public:
  mydata () {
    data_ = 0;
  }

  mydata (int val) {
    data_ = val;
  }

  void
  set_value (int value) {
    data_ = value;
  }

  int
  value () {
    return data_;
  }

private:
  int data_;
};

int
main () {
#define SIZE 5
  k::detail::list<mydata> c;
  mydata data[SIZE];

  // insert
  for (int i = 0; i < SIZE; ++i) {
    data[i].set_value(i);
    c.insert(data[i]);
  }
  std::cerr << "first: " << c.begin()->value() << std::endl;
  std::cerr << "end: " << c.end()->value() << std::endl;
  std::cerr << "size:" << c.size() << std::endl;
  std::cerr << "is_empty: " << c.empty() << std::endl;

  // remove
  for (int i = 0; i < SIZE; ++i) {
    assert(c.exist(data[i]));
    c.remove(data[i]);
  }
  std::cerr << "size:" << c.size() << std::endl;

  // insert
  for (int i = 0; i < SIZE; ++i) {
    data[i].set_value(i);
    c.insert(data[i]);
  }
  std::cerr << "size:" << c.size() << std::endl;

  // clear
  c.clear();
  std::cerr << "size:" << c.size() << std::endl;

  // empty
  std::cerr << "is_empty: " << c.empty() << std::endl;

  return 0;
}
