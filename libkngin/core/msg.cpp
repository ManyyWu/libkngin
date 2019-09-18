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

msg *
msg::create (uint32_t _type)
{
    if (!_type || _type > MAX_MSG_TYPE)
        return false;

    msg *_msg = new msg();
    assert(_msg);
    return _msg;
}

msg *
msg::create (const uint8_t *_buf, uint32_t _size, uint32_t _type)
{
    if ((!_type || _type > MAX_MSG_TYPE) || (_size > MAX_MSG_SIZE))
        return false;

    msg *_msg = new msg();
    assert(_msg);
    if (!_msg)
        return NULL;

    _msg->m_type = _type;
    _msg->m_size = _size;
    if (_size && _buf) {
        _msg->m_buf = new uint8_t[_size];
        if (!_msg->m_buf)
            return false;
        memcpy(_msg->m_buf, _buf, std::min(_size, MAX_MSG_SIZE));
    } else {
        _msg->m_buf = NULL;
        _msg->m_size = 0;
    }

    return _msg;
}

msg *
msg::create (const msg *_msg)
{
    if ((!_msg->m_type || _msg->m_type > MAX_MSG_TYPE) || (_msg->m_size > MAX_MSG_SIZE))
        return false;

    msg *_new_msg = new msg();
    assert(_new_msg);
    if (!_new_msg)
        return NULL;

    _new_msg->m_type = _msg->m_type;
    _new_msg->m_size = _msg->m_size;
    if (_msg->m_size && _msg->m_buf) {
        _new_msg->m_buf = new uint8_t[_msg->m_size];
        if (!_new_msg->m_buf)
            return false;
        memcpy(_new_msg->m_buf, _msg->m_buf, std::min(_msg->m_size, MAX_MSG_SIZE));
    } else {
        _new_msg->m_buf = NULL;
        _new_msg->m_size = 0;
    }

    return _new_msg;
}

bool
msg::process ()
{
    return true;
}

__NAMESPACE_END
