#include <cstdio>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "define.h"
#include "logfile.h"
#include "task_base.h"
#include "common.h"
#include "msg.h"

__NAMESPACE_BEGIN

msg::msg (task_base *_task)
    : m_buf(NULL), m_size(0), m_type(INVALID_MSG), m_task(_task)
{
    kassert(_task);
}

msg::~msg ()
{
    kdelete_array(m_buf);
}

bool
msg::create (uint32_t _type)
{
    kassert_r0(__msg_valid(_type));

    if (m_buf)
        kdelete_array(m_buf);
    m_type = _type;
    return true;
}

bool
msg::create (const uint8_t *_buf, uint32_t _size, uint32_t _type)
{
    kassert_r0(__msg_valid(_type));
    kassert_r0(__both(_buf, _size));

    if (m_buf)
        kdelete_array(m_buf);
    if (_size && _buf) {
        m_buf = knew_array(uint8_t, _size);
        if_not (m_buf)
            return false;
        memcpy(m_buf, _buf, (std::min)(_size, MAX_MSG_SIZE));
        m_type = _type;
        m_size = _size;
    } else {
        m_buf = NULL;
        m_size = 0;
    }
    return true;
}

bool
msg::create (uint8_t **_buf, uint32_t _size, uint32_t _type)
{
    kassert_r0(__msg_valid(_type));
    kassert_r0(__both(*_buf, _size));

    if (m_buf)
        kdelete_array(m_buf);
    m_buf = NULL;
    if (_buf) {
        m_buf = *_buf;
        (*_buf) = NULL;
    }
    m_type = _type;
    if (_size && *_buf)
        m_size = _size;

    return true;
}

bool
msg::create (const msg *_msg)
{
    kassert_r0(_msg);
    kassert_r0(__msg_valid(_msg->m_type));
    kassert_r0(__both(_msg->m_buf, _msg->m_size));

    if (m_buf)
        kdelete_array(m_buf);
    if (_msg->m_size && _msg->m_buf) {
        m_buf = knew_array(uint8_t, _msg->m_size);
        kassert_r0(m_buf);
        if (!m_buf)
            return NULL;
        memcpy(m_buf, _msg->m_buf, (std::min)(_msg->m_size, MAX_MSG_SIZE));
        m_type = _msg->m_type;
        m_size = _msg->m_size;
    } else {
        m_buf = NULL;
        m_size = 0;
    }
    return true;
}

bool
msg::create (msg **_msg)
{
    kassert_r0(_msg);
    kassert_r0(__msg_valid((*_msg)->m_type));
    kassert_r0(__both((*_msg)->m_buf, (*_msg)->m_size));

    if (m_buf)
        kdelete_array(m_buf);
    if (*_msg) {
        m_buf = (*_msg)->m_buf;
        (*_msg)->m_buf = NULL;
        m_type = (*_msg)->m_type;
        (*_msg)->m_type = INVALID_MSG;
        m_size = (*_msg)->m_size;
        (*_msg)->m_size = 0;
        *_msg = NULL;
    }

    return true;
}

void
msg::release ()
{
    m_type = INVALID_MSG;
    m_size = 0;
    delete this;
}

bool
msg::process ()
{
    return true;
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
    kassert_r0(m_task);
    return m_task;
}

void
msg::dump ()
{
    uint32_t _len = 80;
    _len += 2 * m_size;
    _len = (std::min)(_len, (uint32_t)__LOG_BUF_SIZE); // max: 4096

    char *_buf = knew_array(char, ((m_size) + _len));
    if_not (_buf) {
        server_fatal("failed to dump msg, size = %#x", m_size);
        delete[] _buf;
        return;
    }
    snprintf(_buf, _len, "*** [dump msg]:\n*** [type]: %#010d\n*** [size]: %#010d\n*** [data]: ",
             m_type, m_size);
    uint32_t _start = (uint32_t )strnlen(_buf, 80);
    _buf[_start] = '\0';
    uint32_t i = 0;
    for (; i < m_size * 2;) {
        char _temp[3];
        sprintf(_temp, "%02x", (uint8_t)m_buf[i]);
        _buf[_start + i++] = _temp[0];
        _buf[_start + i++] = _temp[1];
    }
    _buf[_start + i] = '\0';
    server_dump(_buf, (uint32_t)strnlen(_buf, _len));
    kdelete_array(_buf);
}

__NAMESPACE_END
