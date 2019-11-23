#ifndef _NET_BUFFER_H_
#define _NET_BUFFER_H_

#ifdef _WIN32
#else
#include <sys/uio.h>
#endif
#include <list>
#include <vector>
#include <atomic>
#include "define.h"
#include "buffer.h"

KNGIN_NAMESPACE_K_BEGIN

#ifdef _WIN32
struct iovec {
    void * iov_base; /* Pointer to data. */
    size_t iov_len;  /* Length of data. */
};
#endif

class net_buffer {
public:
    typedef std::list<buffer> buffer_list;

public:
    net_buffer     () : m_list(), m_rindex(0), m_windex(0), m_iovec() {}

    ~net_buffer    () = default;

public:
    buffer_list &
    list            () { return m_list; }

    void
    receive         (size_t _n);

    void
    send            (size_t _n);

    size_t 
    readable        ();

    size_t
    writeable       ();

    size_t 
    size            ();

    std::vector<struct iovec> &
    to_iovec        ();

    void
    swap            (net_buffer &_buf);

protected:
    void
    check_readable  (size_t _n);

    void
    check_writeable (size_t _n);

protected:
    buffer_list               m_list;

    size_t                    m_rindex;

    size_t                    m_windex;

    std::vector<struct iovec> m_iovec;
};

KNGIN_NAMESPACE_K_END

#endif /* _NET_BUFFER_H_ */
