#include "kngin/core/base/string.h"
#include "kngin/core/base/timestamp.h"
#include <iostream>

using namespace std;

char txt[11] = "0123456789";

int
main () {
  {
    k::timestamp begin = k::timestamp::realtime();
    for (int i = 0; i < 1000000; ++i)
    {
      std::string s;
      for (int j = 0; j < 10; ++j)
        s += txt;
    }
    cout << "std::string += cost: " << (k::timestamp::realtime() - begin).value() << "ms\n";
  }

  {
    k::timestamp begin = k::timestamp::realtime();
    for (int i = 0; i < 1000000; ++i)
    {
      std::string s;
      k::format_string(s, "%s%s%s%s%s%s%s%s%s%s",
                       txt, txt, txt, txt, txt, txt, txt, txt, txt, txt);
    }
    cout << "k::make_string cost: " << (k::timestamp::realtime() - begin).value() << "ms\n";
  }

  // std::string += cost: 72132ms
  // k::make_string cost: 24201ms
  return 0;
}
