#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "define.h"
#include "address.h"
#include "epoller_event.h"
#include "socket.h"

__NAMESPACE_BEGIN

class connection {
public:
    connection        () = delete;

    explicit
    connection        ();

public:
    bool
    get_localaddr     (const address &_addr);




protected:
    socket         m_socket;

    epoller_event  m_event;
};

__NAMESPACE_END

#endif /* _CONNECTION_H_ */