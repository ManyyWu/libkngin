#include "kngin/core/base/cond.h"
#include "kngin/core/base/mutex.h"
#include <iostream>
#include <thread>
#include <string>
#include <deque>

using namespace std;

int
main () {
  k::mutex m;
  k::cond c(m);
  std::deque<int> q;

  thread t([&] () {
    {
      k::mutex::scoped_lock lock(m);
      if (q.empty()) {
        cout << "wait" << endl;
        c.timed_wait(1000);
        cout << "wake up, q.size = " << q.size() << endl;
        q.clear();
      }
    }
  });

  thread t1([&] () {
    {
      while (true) {
        std::string s;
        cin >> s;
        if (s == "#")
          break;
        {
          k::mutex::scoped_lock lock(m);
          for (auto iter : s)
            q.push_back(iter);
          c.signal();
          cout << "signal, q.size = " << q.size() << endl;
        }
      }
    }
  });

  t.join();
  t1.join();

  return 0;
}
