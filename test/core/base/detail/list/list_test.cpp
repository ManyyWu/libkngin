#include "detail/core/base/list.h"
#include "detail/core/base/obj_mgr.h"
#include <iostream>

using namespace std;

class mydata : public k::detail::list_entry<mydata> {
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

class myobj : public k::detail::obj_entry<myobj> {
public:
  myobj () {
    data_ = 0;
  }

  myobj (int val) {
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
  // list
  {
    std::cerr << "list test" << std::endl;
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
    std::cerr << "is_empty: " << c.empty() << std::endl << std::endl;
  }

  // obj_mgr
  {
    std::cerr << "obj_mgr test" << std::endl;
    typedef std::shared_ptr<myobj> mydata_ptr;
    k::detail::obj_mgr<myobj> c(20);

    mydata_ptr data[SIZE];

    // insert
    for (int i = 0; i < SIZE; ++i) {
      data[i] = std::make_shared<myobj>(i);
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
      data[i] = std::make_shared<myobj>(i);
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
    mydata_ptr tmp = std::make_shared<myobj>(11);
    c.insert(tmp);
    std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

    // auto fix
    c.resize(300);
    c.remove(tmp);
    std::cerr << "size:" << c.size() << " max_size:" << c.max_size() << std::endl;

    // empty
    std::cerr << "is_empty: " << c.empty() << std::endl;
  }

  return 0;
}
