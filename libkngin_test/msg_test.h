#include "../libkngin/core/msg.h"

using namespace k;

enum ACTION {
    ACTION_ERR = 0, //                          S->C error param = error code
    ACTION_0,       // C-S action0 param = num, S->C ok    param = num * 1
    ACTION_1,       // C-S action1 param = num, S->C ok    param = num * 2
    ACTION_2,       // C-S action2 param = num, S->C ok    param = num * 3
    ACTION_3        // C-S action3 param = num, S->C ok    param = num * 4
};

#pragma pack(push, 1)
struct msg_head {
    int version;
    int type;
    int size;
};
#pragma pack(pop)

#define MSG_TYPE_TEST0 MSG_BEGIN + 0U

class netmsg_test : public msg {
public:

public:
    netmsg_test (task_base *_task)
    : msg(_task), m_info(NULL)
    {
    }

protected:
    virtual
    ~netmsg_test ()
    {
        kdelete_array(m_info);
    }

public:
    virtual bool
    create (ACTION _action, int _param)
    {
        //knew(m_info, test_info, ({_action, _param}));
        knew(m_info, test_info, ({_action, _param}));
        kassert_r0(m_info);
        if (!m_info)
            return false;
        return msg::create((uint8_t *)m_info, sizeof(test_info), MSG_TYPE_TEST0);
    }

public:
    virtual bool
    process ()
    {
        return [this] () mutable -> bool {
            switch (this->m_info->action) {
            case ACTION_0:
                return [this] (int _param) -> bool {
                    netmsg_test *_msg = NULL; 
                    knew(_msg, netmsg_test, (this->task()));
                    if (!_msg)
                        return false;
                    if_not (_msg->create(ACTION_0, _param * 1)) {
                        kdelete(_msg);
                        return false;
                    }
                    this->task()->recv_reply_msg((msg **)&_msg);
                    return true;
                }(this->m_info->param);
                break;
            case ACTION_1:
                return [this] (int _param) -> bool {
                    netmsg_test *_msg = NULL; 
                    knew(_msg, netmsg_test, (this->task()));
                    if (!_msg)
                        return false;
                    if_not (_msg->create(ACTION_1, _param * 2)) {
                        kdelete(_msg);
                        return false;
                    }
                    this->task()->recv_reply_msg((msg **)&_msg);
                    return true;
                }(this->m_info->param);
                break;
            case ACTION_2:
                return [this] (int _param) -> bool {
                    netmsg_test *_msg = NULL; 
                    knew(_msg, netmsg_test, (this->task()));
                    if (!_msg)
                        return false;
                    if_not (_msg->create(ACTION_2, _param * 3)) {
                        kdelete(_msg);
                        return false;
                    }
                    this->task()->recv_reply_msg((msg **)&_msg);
                    return true;
                }(this->m_info->param);
                break;
            case ACTION_3:
                return [this] (int _param) -> bool {
                    netmsg_test *_msg = NULL; 
                    knew(_msg, netmsg_test, (this->task()));
                    if (!_msg)
                        return false;
                    if_not (_msg->create(ACTION_3, _param * 3)) {
                        kdelete(_msg);
                        return false;
                    }
                    this->task()->recv_reply_msg((msg **)&_msg);
                    return true;
                }(this->m_info->param);
                break;
            default:
                log_error("invalid message type");
                break;
            }
            return false;
        }();
    }

    const uint8_t *
    get_buf ()
    {
        return m_buf;
    }

protected:
#pragma pack(push, 1)
    struct test_info {
        int action;
        int param;
    };
#pragma pack(pop)

    struct test_info *m_info;
};
