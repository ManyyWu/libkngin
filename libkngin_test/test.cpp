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
#warning "signal ergent"
#warning "adjust size of epoll_event_set"
#warning "std::error_code"
#warning "noncopyable"
#warning "buf idx 重新设计"
#warning "end of __XXXX__"
#warning "构造函数, 删除多余的log_error, 直接抛出信息和详情, 如果带错误码不需要带详情"
#warning "同步最好直接返回错误，异步在回调函数中返回"
#warning "close bind connect 等都有ec版本，不带ec版本"
#warning "头文件宏改 KNGIN_HEAD_FILE_XXXXX"
#warning "新建面向应用的错误码，抽象!!!!!!!!!!!!!!!!!!!!!!!!!!"
#warning "应用层不接触任何底层错误"
#warning "private"
#warning "函数体换行"
#warning "nrv优化"
#warning "析构函数全部KNGIN_NOEXP"
#warning "pthread相关错误直接记录日志抛k::exceptino(除析构函数), 网络错误提供两种选择"
#warning "system_error_str"
#warning "信号"
#warning "pimpl() public, self() public, class pimpl public"
#warning "参数错误throw k::exception("invalid arguments") 或使用arg_check(), 不添加其他信息"
#warning "final"
#warning "新建对象但不泄漏给用户的地方需要catch"
#warning "https://zhuanlan.zhihu.com/p/88749279"
#warning "https://github.com/ossrs/state-threads/blob/master/examples/server.c"
#warning "static_cast"

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

void
test ()
{


}
