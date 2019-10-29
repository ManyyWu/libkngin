#ifndef _NET_BUFFER_H_
#define _NET_BUFFER_H_

#include <list>
#include <atomic>
#include "define.h"
#include "buffer.h"

__NAMESPACE_BEGIN

class net_buffer {
public:
    typedef filefd::buffer_list buffer_list;

public:
    net_buffer      () : m_use_buf1(true)

    ~net_buffer     () = default;

public:
    buffer_list &
    buffer1         () { return m_buf[0]; }

    buffer_list &
    buffer2         () { return m_buf[1]; }

    void
    buffer1_receive ();

    void
    buffer2_receive ();

protected:
    buffer_list       m_buf[2];

    std::atomic<bool> m_use_buf1;
};

__NAMESPACE_END

#endif /* _NET_BUFFER_H_ */
