#ifndef _EVENT_H_
#define _EVENT_H_

#include <functional>
#include "define.h"
#include "noncopyable.h"
#include "filefd.h"

__NAMESPACE_BEGIN

class event_loop;
class event : public filefd {
public:
    typedef std::function<void ()> event_cb;

public:
    event  ();

    virtual
    ~event ();
};

__NAMESPACE_END

#endif /* _EVENT_H_ */