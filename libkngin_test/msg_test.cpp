#include "../libkngin/core/msg.h"

using namespace k;

class netmsg : msg {
protected:
#pragma pack(push, 1)
    struct msg_head {
        int version;
        int type;
        int size;
    };
#pragma pack(pop)
};
