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
#include "../libkngin/core/logfile.h"
#include "../libkngin/core/thread.h"
#include "../libkngin/core/lock.h"
#include "../libkngin/core/memory.h"
#include "../libkngin/core/buffer.h"
#include "../libkngin/core/bits.h"
#include "../libkngin/core/error.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"
#include "../libkngin/net/tcp/session.h"
#include "../libkngin/net/tcp/server.h"
#include "../libkngin/net/listener.h"

using namespace k;
using namespace std;
using namespace std::placeholders;
#warning "返回值处理"
#warning "头文件宏改 KNGIN_HEAD_FILE_XXXXX"
#warning "新建面向应用的错误码，抽象!!!!!!!!!!!!!!!!!!!!!!!!!!"
#warning "应用层不接触任何底层错误"
#warning "private"
#warning "nrv优化"
#warning "https://zhuanlan.zhihu.com/p/88749279"
#warning "https://github.com/ossrs/state-threads/blob/master/examples/server.c"
#warning "防止shared_ptr拷贝"
#warning "ET mode"
/*
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
error (char *msg)
{
    perror(msg);
    exit(0);
}
void
test ()
{
}

