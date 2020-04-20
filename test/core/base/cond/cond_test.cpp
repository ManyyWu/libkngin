#include "kngin/core/base/cond.h"
#include "kngin/core/base/mutex.h"
#include "kngin/core/base/thread.h"
#include <iostream>
#include <string>
#include <deque>

using namespace std;

int
main () {
  k::mutex m;
  k::cond c(m);
  std::deque<int> q;

  k::thread t([&] () -> int {
    {
      k::mutex::scoped_lock lock(m);
      if (q.empty()) {
        cout << "wait" << endl;
        c.wait();
        cout << "wake up, q.size = " << q.size() << endl;
        q.clear();
      }
    }
    return 0;
  });

  k::thread t1([&] () -> int {
    {
      while (true) {
        std::string s;
        cin >> s;
        if (s == "#") {
          c.signal();
          break;
        }
        {
          k::mutex::scoped_lock lock(m);
          for (auto &iter : s)
            q.push_back(iter);
          c.signal();
          cout << "signal, q.size = " << q.size() << endl;
        }
      }
    }
    return 0;
  });

  t.join();
  t1.join();

  return 0;
}
