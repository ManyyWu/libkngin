#include <sys/eventfd.h>
#include <cstring>
#include "define.h"
#include "event.h"
#include "filefd.h"
#include "common.h"

__NAMESPACE_BEGIN

event::event ()
    : filefd(::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK))
{
    if_not (__fd_valid(m_fd)) {
        log_fatal("eventfd() error - %s:%d", strerror(errno), errno);
        throw exception("event::event() erorr");
    }
}

event::~event()
{
}

__NAMESPACE_END
