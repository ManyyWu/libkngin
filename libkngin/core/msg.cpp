#include <cstdint>
#include <cstring>
#include <algorithm>
#include "msg.h"

__NAMESPACE_BEGIN

msg::msg (int _type)
{
    assert(_type > 0 && _type <= MAX_MSG_TYPE);

    m_type = _type;
    m_size = 0;
    m_buf = NULL;
}

msg::msg (const uint8_t *_buf, int _size, int _type)
{
    assert(_type > 0 && _type <= MAX_MSG_TYPE);
    assert(_size >= 0 && _size <= MAX_MSG_SIZE);

    m_type = _type;
    m_size = _size;
    if (_size && _buf) {
        m_buf = new uint8_t[_size];
        assert(m_buf);
        memcpy(m_buf, _buf, std::min(_size, MAX_MSG_SIZE));
    } else {
        m_buf = NULL;
        m_size = 0;
    }
};

msg::msg (const msg &_msg)
{
    assert(_msg.m_type > 0 && _msg.m_type <= MAX_MSG_TYPE);
    assert(_msg.m_size >= 0 && _msg.m_size <= MAX_MSG_SIZE);

    m_type = _msg.m_type;
    m_size = _msg.m_size;
    if (_msg.m_size && _msg.m_buf) {
        m_buf = new uint8_t[_msg.m_size];
        assert(m_buf);
        memcpy(m_buf, _msg.m_buf, std::min(_msg.m_size, MAX_MSG_SIZE));
    } else {
        m_buf = NULL;
        m_size = 0;
    }
}

msg::msg (msg &&_msg)
{
    assert(_msg.m_type > 0 && _msg.m_type <= MAX_MSG_TYPE);
    assert(_msg.m_size >= 0 && _msg.m_size <= MAX_MSG_SIZE);
    m_type = _msg.m_type;
    m_size = _msg.m_size;
    m_buf = _msg.m_buf;
    _msg.m_buf = NULL;
    _msg.m_size = 0;
    _msg.m_type = INVALID_MSG_TYPE;
}

msg::~msg ()
{
    delete m_buf;
}

int
msg::process ()
{

}

bool
msg::parse ()
{
// parse head
// parse body
}

__NAMESPACE_END
