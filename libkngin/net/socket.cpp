#include <unistd.h>
#include "define.h"
#include "filefd.h"
#include "logfile.h"
#include "socket.h"

__NAMESPACE_BEGIN

socket::socket (int _sockfd)
    : filefd(_sockfd)
{
}

socket::~socket ()
{
}

__NAMESPACE_END
