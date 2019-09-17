#include <cstdint>
#include <cstring>
#include <algorithm>
#include "msg.h"

__NAMESPACE_BEGIN

msg::msg ()
    : m_buf(NULL), m_size(0), m_type(INVALID_MSG_TYPE)
{
}

msg::~msg ()
{
    delete m_buf;
}

bool
msg::create (uint32_t _type)
{
    if (!_type || _type > MAX_MSG_TYPE)
        return false;

    m_type = _type;
    m_size = 0;
    m_buf = NULL;

    return true;
}

bool
msg::create (const uint8_t *_buf, uint32_t _size, uint32_t _type)
{
    if ((!_type || _type > MAX_MSG_TYPE) || (_size > MAX_MSG_SIZE))
        return false;

    m_type = _type;
    m_size = _size;
    if (_size && _buf) {
        m_buf = new uint8_t[_size];
        if (!m_buf)
            return false;
        memcpy(m_buf, _buf, std::min(_size, MAX_MSG_SIZE));
    } else {
        m_buf = NULL;
        m_size = 0;
    }

    return true;
}

bool
msg::create (const msg &_msg)
{
    if ((!_msg.m_type || _msg.m_type > MAX_MSG_TYPE) || (_msg.m_size > MAX_MSG_SIZE))
        return false;

    m_type = _msg.m_type;
    m_size = _msg.m_size;
    if (_msg.m_size && _msg.m_buf) {
        m_buf = new uint8_t[_msg.m_size];
        if (!m_buf)
            return false;
        memcpy(m_buf, _msg.m_buf, std::min(_msg.m_size, MAX_MSG_SIZE));
    } else {
        m_buf = NULL;
        m_size = 0;
    }

    return true;
}

bool
msg::create (msg &&_msg)
{
    if ((!_msg.m_type || _msg.m_type > MAX_MSG_TYPE) || (_msg.m_size > MAX_MSG_SIZE))
        return false;

    m_type = _msg.m_type;
    m_size = _msg.m_size;
    m_buf = _msg.m_buf;
    _msg.m_buf = NULL;
    _msg.m_size = 0;
    _msg.m_type = INVALID_MSG_TYPE;

    return true;
}

bool
msg::process ()
{
    return true;
}

__NAMESPACE_END
