#include "kngin/core/base/timestamp.h"
#include <iostream>
using namespace std;

int
main () {
  k::timestamp t(1000);
  cout << t.value() << endl;
  cout << (k::timestamp(1000) == t) << endl;
  cout << (k::timestamp(1000) != t) << endl;

  cout << k::timestamp::monotonic().value() << endl;
  cout << k::timestamp::realtime().value() << endl;

  return 0;
}
