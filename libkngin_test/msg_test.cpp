#include <stdio.h>
#include "../libkngin/core/msg.h"
#include "../libkngin/core/task_base.h"

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
    class task_test {
    public:
        void
        process_msg (netmsg_test *_msg)
        {
            [this, _msg]() {
                _msg->process();
                netmsg_test *_new_msg = new netmsg_test();
                _new_msg->create(_msg, );
                this->send_msg();
            }
            _msg->release();
        }

        void
        send_msg (msg *_msg)
        {
            printf("%s\n", _msg);
            fflush(stderr);
            _msg->release();
        }
    };

    class test_action {
    public:
        test_action (task_test *_task)
            : m_task(_task)
        {
        }

        ~test_action () = default;

    public:
        bool
        action0 (int _param)
        {
            netmsg_test *_msg = new_nothrow(netmsg_test());
            if (!_msg)
                return false;
            _msg->create(ACTION_0, _param * 1);

        }

        bool
        action1 (int _param)
        {
            netmsg_test *_msg = new_nothrow(netmsg_test());
            if (!_msg)
                return false;
            _msg->create(ACTION_0, _param * 1);

        }

        bool
        action2 (int _param)
        {
            netmsg_test *_msg = new_nothrow(netmsg_test());
            if (!_msg)
                return false;
            _msg->create(ACTION_0, _param * 1);

        }

        bool
        action3 (int _param)
        {
            netmsg_test *_msg = new_nothrow(netmsg_test());
            if (!_msg)
                return false;
            if (!_msg->create(ACTION_0, _param * 1))
                return false;

        }

    protected:
        task_test *m_task;
    };

public:
    netmsg_test ()
        : msg()
    {
    }

protected:
    virtual
    ~netmsg_test ()
    {
    }

public:
    virtual bool
    create (ACTION _action, int _param)
    {
        m_info = new test_info{_action, _param};
        return msg::create((uint8_t *)m_info, sizeof(test_info), MSG_TYPE_TEST0);
    }

public:
    virtual bool
    process ()
    {
        bool _ret = false;
        test_action _act(m_task);

        switch (m_info->action) {
        case ACTION_0:
            _ret = _act.action1(m_info->param);
            break;
        case ACTION_1:
            _ret = _act.action1(m_info->param);
            break;
        case ACTION_2:
            _ret = _act.action2(m_info->param);
            break;
        case ACTION_3:
            _ret = _act.action3(m_info->param);
            break;
        default:
            assert(0);
        }
        return _ret;
    }

protected:
#pragma pack(push, 1)
    struct test_info {
        int action;
        int param;
    };
#pragma pack(pop)

    test_info *m_info;

    task_test *m_task;
};

void
msg_test ()
{

}
