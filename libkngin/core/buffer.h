#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include "define.h"

using std::string;

__NAMESPACE_BEGIN

class buffer {
public:
    buffer   ();

    // buffer (uint8_t *_buf, size_t _n) throw();

    buffer   (uint8_t **_buf, size_t _n);

    // buffer (buffer &_buf);

    buffer   (buffer &&_buf);

    ~buffer  ();

public:
    

protected:
    std::string m_buf;

    size_t      m_max;
};

__NAMESPACE_END

#endif /* _BUFFER_H_ */