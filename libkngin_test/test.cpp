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
#include "../libkngin/core/sync_queue.h"
#include "../libkngin/net/socket.h"
#include "../libkngin/net/sockopts.h"

using namespace k;
using namespace std;
using namespace std::placeholders;

void
test ()
{
#warning "包装系统api， 取消所有头文件引用, k::core::api, k::net::api"
#warning "std::function &&改&"
#warning "xxx_cb 改"
#warning "返回值处理"
/*
    addrinfo *_list = nullptr;
    addrinfo _info;
    memset(&_info, 0, sizeof(_info));
    _info.ai_flags = AI_PASSIVE;
    _info.ai_family = AF_UNSPEC;
    _info.ai_socktype = 0;

    int _ret = getaddrinfo("192.168.0.2", "20000", &_info, &_list);
    if (_ret < 0) {
        cerr << "getaddrinfo() error - " << strerror(errno) << endl;
        return;
    }

    addrinfo *_temp = _list;
    while (_temp) {
        char _name[INET6_ADDRSTRLEN];
        char _port[10];
        _ret = getnameinfo(_temp->ai_addr, _temp->ai_addrlen, _name, INET6_ADDRSTRLEN, _port, 10, 0);
        if (_ret < 0) {
            cerr << "getnameinfo() error - " << strerror(errno) << endl;
            freeaddrinfo(_list);
            return;
        }
        char _ip[INET6_ADDRSTRLEN];
        inet_ntop(_temp->ai_addr->sa_family, (in_addr *)(&(((sockaddr_in *)(_temp->ai_addr))->sin_addr)), _ip, sizeof(_ip));
        cerr << strerror(errno) << endl;
        cerr << "name: " << _name << ", ip: " << _ip << ", port: " << _port << endl;
        _temp = _temp->ai_next;
    }

    freeaddrinfo(_list);
    */
}
