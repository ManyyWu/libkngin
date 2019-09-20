#include <cstdint>
#include <cstring>
#include <algorithm>
#include "define.h"
#include "task_base.h"
#include "common.h"
#include "msg.h"

__NAMESPACE_BEGIN

msg::msg (task_base *_task)
    : m_buf(NULL), m_size(0), m_type(INVALID_MSG), m_task(_task)
{
}

msg::~msg ()
{
    safe_release(m_buf);
}

bool
msg::create (uint32_t _type)
{
    if (!_type || _type > MAX_MSG)
        return false;

    if (m_buf)
        safe_release(m_buf);
    m_type = _type;
    return true;
}

bool
msg::create (const uint8_t *_buf, uint32_t _size, uint32_t _type)
{
    if ((!_type|| _type > MAX_MSG) || (_size > MAX_MSG_SIZE))
        return false;

    if (m_buf)
        safe_release(m_buf);
    if (_size && _buf) {
        m_buf = new_nothrow(uint8_t[_size]);
        assert(m_buf);
        if (!m_buf)
            return false;
        memcpy(m_buf, _buf, std::min(_size, MAX_MSG_SIZE));
        m_type = _type;
        m_size = _size;
    } else {
        m_buf = NULL;
        m_size = 0;
    }
    return true;
}

bool
msg::create (const msg *_msg)
{
    if ((!_msg->m_type || _msg->m_type > MAX_MSG) || (_msg->m_size > MAX_MSG_SIZE))
        return NULL;

    if (m_buf)
        safe_release(m_buf);
    if (_msg->m_size && _msg->m_buf) {
        m_buf = new_nothrow(uint8_t[_msg->m_size]);
        assert(m_buf);
        if (!m_buf)
            return NULL;
        memcpy(m_buf, _msg->m_buf, std::min(_msg->m_size, MAX_MSG_SIZE));
        m_type = _msg->m_type;
        m_size = _msg->m_size;
    } else {
        m_buf = NULL;
        m_size = 0;
    }
    return true;
}

void
msg::release ()
{
    if (m_buf)
        safe_release(m_buf);
    m_type = INVALID_MSG;
    m_size = 0;
    delete this;
}

void
msg::process ()
{
}

const uint8_t *
msg::buf ()
{
    return m_buf;
}

uint32_t
msg::size ()
{
    return m_size;
}

uint32_t
msg::type ()
{
    return m_type;
}

task_base *
msg::task ()
{
    assert(m_task);
    return m_task;
}

__NAMESPACE_END
