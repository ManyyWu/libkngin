#include "buffer.h"
#include "define.h"

__NAMESPACE_BEGIN

using std::string;

buffer::buffer ()
{
}

buffer::buffer (uint8_t **_buf, size_t _n)
{

}

buffer::buffer (buffer &&_buf)
    : _s(std::move(_buf._s))
{
}

__NAMESPACE_END
