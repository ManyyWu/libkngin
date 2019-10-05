#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "define.h"
#include "noncopyable.h"
#include "epoller_event.h"
#include "socket.h"

__NAMESPACE_BEGIN

class connection : noncopyable {
public:
    virtual socket *
    get_socket      ();

    virtual epoller_event *
    get_epoller_event ();

protected:
    socket m_socket;

    epoller_event  m_event;
};

__NAMESPACE_END

#endif /* _CONNECTION_H_ */