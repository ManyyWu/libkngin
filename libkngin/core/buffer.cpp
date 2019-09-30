#include <exception>
#include <memory>
#include "buffer.h"
#include "memory.h"
#include "define.h"

__NAMESPACE_BEGIN

buffer::buffer ()
{
}

buffer::buffer (uint8_t *_buf, size_t _n)
{
}

buffer::buffer (uint8_t **_buf, size_t _n) 
    try 
    : m_buf(make_shared<uint8_t[]>(_n))
{
} catch (...) { m_buf.reset(); }

buffer::buffer (buffer &&_buf)
    : m_buf(std::move(_buf.m_buf))
{
}

__NAMESPACE_END
