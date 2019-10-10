#include <sys/timerfd.h>
#include "define.h"
#include "filefd.h"
#include "timestamp.h"

__NAMESPACE_BEGIN

class timer : public filefd {
public:
    typedef std::function<void (timestamp)> timer_cb;

public:
    timer  (timestamp _ms);

    virtual
    ~timer ();
};

__NAMESPACE_END