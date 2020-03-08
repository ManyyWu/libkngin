#include "kngin/core/event/event_loop.h"
#include <iostream>

using namespace std;

int
main () {
  k::event_loop loop;
  loop.run();

  return 0;
}