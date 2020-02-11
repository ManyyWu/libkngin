#include <atomic>
#include <typeinfo>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cinttypes>
#include <ctime>
#include <string>
#include <array>
#include <memory>
#include <cstring>
#ifndef _WIN32
#include <unistd.h>
#include <sys/eventfd.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <execinfo.h>
#else
#include <Winsock2.h>
#include <Windows.h>
#endif
#include "../libkngin/core/base/logfile.h"
#include "../libkngin/core/base/thread.h"
#include "../libkngin/core/base/lock.h"
#include "../libkngin/core/base/memory.h"
#include "../libkngin/core/base/buffer.h"
#include "../libkngin/core/base/bits.h"
#include "../libkngin/core/base/error.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/tcp/session.h"
#include "../libkngin/net/tcp/server.h"
#include "../libkngin/net/listener.h"

using namespace k;
using namespace std;
using namespace std::placeholders;

#warning "https://zhuanlan.zhihu.com/p/88749279"
#warning "https://github.com/ossrs/state-threads/blob/master/examples/server.c"
#warning "signal event"
/*
client:

switch (savedErrno)  //检查错误码
{
case 0:
case EINPROGRESS:  //非阻塞套接字，未连接成功返回码是EINPROGRESS表示正在连接
case EINTR:
case EISCONN:   //连接成功
  connecting(sockfd);
  break;

case EAGAIN:
case EADDRINUSE:
case EADDRNOTAVAIL:
case ECONNREFUSED:
case ENETUNREACH:
  retry(sockfd);   //重连
  break;

case EACCES:
case EPERM:
case EAFNOSUPPORT:
case EALREADY:
case EBADF:
case EFAULT:
case ENOTSOCK:
  LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
  sockets::close(sockfd);   //这几种情况不能重连，
  break;

default:
  LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
  sockets::close(sockfd);
  break;
}

*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>

void
test ()
{
return;
}

