#ifdef _WIN32
#else
#include <sys/uio.h>
#endif
#include <list>
#include <memory>
#include <vector>
#include "exception.h"
#include "buffer.h"
#include "net_buffer.h"

#ifdef __FILENAME__
#undef __FILENAME__
#endif
#define __FILENAME__ "libkngin/net/net_buffer.cpp"

__NAMESPACE_BEGIN

void
net_buffer::receive (size_t _n)
{
    check_readable(_n);
    m_windex += _n;
}

void
net_buffer::send (size_t _n)
{
    check_writeable(_n);
    m_rindex += _n;
}

size_t 
net_buffer::readable ()
{
    size_t _size = writeable();
    m_rindex = (std::min)(_size, m_rindex);
    return _size - m_rindex;
}

size_t
net_buffer::writeable ()
{
    size_t _size = size();
    m_windex = (std::min)(_size, m_windex);
    return _size - m_windex;
}

size_t 
net_buffer::size ()
{
    size_t _size = 0;
    for (auto _iter : m_list)
        _size += _iter.size();
    return _size;
}

std::vector<struct iovec> &
net_buffer::to_iovec ()
{
    std::vector<struct iovec> _iovec;
    std::swap(m_iovec, _iovec);
    size_t _size = size();
    size_t _remain = (std::min)(_size, m_windex);
    for (auto _iter : m_list) {
        if (_iter.size() > _remain)
            m_iovec.push_back({_iter.data() + _remain, _iter.size() - _remain});
        _remain -= (std::min)(_iter.size(), _remain);
    }
    return m_iovec;
}
void 
net_buffer::swap (net_buffer &_buf)
{ 
    std::swap(m_list, _buf.m_list); 
    std::swap(m_rindex, _buf.m_rindex);
    std::swap(m_windex, _buf.m_windex);
    std::swap(m_iovec, _buf.m_iovec);
}

void
net_buffer::check_readable (size_t _n)
{
    if (readable() < _n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}

void
net_buffer::check_writeable (size_t _n)
{
    if (writeable() < _n)
        throw k::exception("net_buffer::check_readable() - no enought space");
}

__NAMESPACE_END
