#include <atomic>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/eventfd.h>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/bits.h"
#include "epoller.h"

using namespace k;
using namespace std;
using namespace std::placeholders;

void print()
{
    cout << "empty" << endl;
}

//展开函数
template <class T, class ...Args> void print(T &head, Args... rest)
{
    cout << "parameter " << head << endl;
    print(rest...);
}


void
test ()
{
}