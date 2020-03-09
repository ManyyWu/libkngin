#include "detail/core/base/list.h"
#include <iostream>

using namespace std;

class mydata :
    public k::detail::value_base<mydata>,
    public std::enable_shared_from_this<mydata> {
public:
  mydata () {
    data_ = 0;
  }

  mydata (int val) {
    data_ = val;
  }

  int
  value () {
    return data_;
  }

private:
  int data_;
};

typedef std::shared_ptr<mydata> mydata_ptr;

int
main () {
#define SIZE 5
  k::detail::list<mydata> c(20);
  mydata_ptr data[SIZE];

  // insert
  for (int i = 0; i < SIZE; ++i) {
    data[i] = std::make_shared<mydata>(i);
    c.insert(data[i]);
  }
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // remove
  for (int i = 0; i < SIZE; ++i) {
    assert(c.exist(data[i]));
    c.remove(data[i]);
  }
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // insert
  for (int i = 0; i < SIZE; ++i) {
    data[i] = std::make_shared<mydata>(i);
    c.insert(data[i]);
  }
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // resize
  c.resize(200);
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // clear
  c.clear();
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // clear free
  c.shrink_to_fix();
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // pre alloc
  mydata_ptr tmp = std::make_shared<mydata>(11);
  c.insert(tmp);
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // auto fix
  c.resize(300);
  c.remove(tmp);
  std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

  // empty
  std::cerr << "is_empty: " << c.empty() << std::endl;

  return 0;
}
