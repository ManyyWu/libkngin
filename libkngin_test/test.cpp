#include <atomic>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <string>
#include <array>
#include <memory>
#include <cstring>
#include <unistd.h>
#include <sys/eventfd.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/bits.h"
#include "../libkngin/core/error.h"
#include "../libkngin/core/sync_queue.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"

using namespace k;
using namespace std;
using namespace std::placeholders;

void fun (int &a)
{
    cerr << (uintptr_t)&a << endl;
}

void
test ()
{
    int a = 0x10;
    cerr << a << endl;
    cerr << (uintptr_t)&a << endl;
    fun(a);
}
