//#include "msg.h"
//
//#ifdef KNGIN_FILENAME
#undef KNGIN_FILENAME
#endif
#define KNGIN_FILENAME "libkngin_test/msg_test.h"
//using namespace k;
//
//enum ACTION {
//    ACTION_ERR = 0, //                          S->C error param = error code
//    ACTION_0,       // C-S action0 param = num, S->C ok    param = num * 1
//    ACTION_1,       // C-S action1 param = num, S->C ok    param = num * 2
//    ACTION_2,       // C-S action2 param = num, S->C ok    param = num * 3
//    ACTION_3        // C-S action3 param = num, S->C ok    param = num * 4
//};
//
//#pragma pack(push, 1)
//struct msg_head {
//    int version;
//    int type;
//    int size;
//};
//#pragma pack(pop)
//
//#define MSG_TYPE_TEST0 MSG_BEGIN + 0U
//
//class netmsg_test : public msg {
//public:
//
//public:
//    netmsg_test (task_base *task)
//    : msg(task), info_(nullptr)
//    {
//    }
//
//protected:
//    virtual
//    ~netmsg_test ()
//    {
//        kdelete_array(info_);
//    }
//
//public:
//    virtual bool
//    create (ACTION action, int param)
//    {
//        //knew(info_, test_info, ({action, param}));
//        knew(info_, test_info, ({action, param}));
//        assert(info_);
//        if (!info_)
//            return false;
//        return msg::create((uint8_t *)info_, sizeof(test_info), MSG_TYPE_TEST0);
//    }
//
//public:
//    virtual bool
//    process ()
//    {
//        return [this] () mutable -> bool {
//            switch (this->info_->action) {
//            case ACTION_0:
//                return [this] (int param) -> bool {
//                    netmsg_test *msg = nullptr;
//                    knew(msg, netmsg_test, (this->task()));
//                    if (!msg)
//                        return false;
//                    if_not (msg->create(ACTION_0, param * 1)) {
//                        kdelete(msg);
//                        return false;
//                    }
//                    this->task()->recv_reply_msg((msg **)&msg);
//                    return true;
//                }(this->info_->param);
//                break;
//            case ACTION_1:
//                return [this] (int param) -> bool {
//                    netmsg_test *msg = nullptr;
//                    knew(msg, netmsg_test, (this->task()));
//                    if (!msg)
//                        return false;
//                    if_not (msg->create(ACTION_1, param * 2)) {
//                        kdelete(msg);
//                        return false;
//                    }
//                    this->task()->recv_reply_msg((msg **)&msg);
//                    return true;
//                }(this->info_->param);
//                break;
//            case ACTION_2:
//                return [this] (int param) -> bool {
//                    netmsg_test *msg = nullptr;
//                    knew(msg, netmsg_test, (this->task()));
//                    if (!msg)
//                        return false;
//                    if_not (msg->create(ACTION_2, param * 3)) {
//                        kdelete(msg);
//                        return false;
//                    }
//                    this->task()->recv_reply_msg((msg **)&msg);
//                    return true;
//                }(this->info_->param);
//                break;
//            case ACTION_3:
//                return [this] (int param) -> bool {
//                    netmsg_test *msg = nullptr;
//                    knew(msg, netmsg_test, (this->task()));
//                    if (!msg)
//                        return false;
//                    if_not (msg->create(ACTION_3, param * 3)) {
//                        kdelete(msg);
//                        return false;
//                    }
//                    this->task()->recv_reply_msg((msg **)&msg);
//                    return true;
//                }(this->info_->param);
//                break;
//            default:
//                log_error("invalid message type");
//                break;
//            }
//            return false;
//        }();
//    }
//
//    const uint8_t *
//    get_buf ()
//    {
//        return buf_;
//    }
//
//protected:
//#pragma pack(push, 1)
//    struct test_info {
//        int action;
//        int param;
//    };
//#pragma pack(pop)
//
//    struct test_info *info_;
//};
