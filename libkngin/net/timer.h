#include <sys/timerfd.h>
#include "define.h"
#include "filefd.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

class timer : public filefd {
public:
    typedef std::function<void (timestamp)> timer_cb;

public:
    timer        ();

    virtual
    ~timer       ();

public:
    void
    set_time     (const timestamp &_ms, bool _abs = false);

    void
    set_time     (const timestamp &&_ms, bool _abs = false);

    void
    set_interval (const timestamp &_ms);

    void
    set_interval (const timestamp &&_ms);
};

__NAMESPACE_END