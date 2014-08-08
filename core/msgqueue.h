#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__
#include <deque>
#define MSG_ERROR   120
#define ERROR_DISPATCH 121
#define ERROR_NOT_EXIST_SESSION 122
#define ERROR_SESSION_STATUS 123
#define ERROR_NOT_EXIST_CO 124
#define MSG_LOCAL_REQ  1
#define MSG_LOCAL_RES  2 
#define MSG_REMOTE_REQ 3 
#define MSG_REMOTE_RES 4 
#define MSG_SHIFT_RETURN 24 
#define ISRET(type)  (type) & (1 << MSG_SHIFT_RETURN)
#define GETTYPE(type) (type) & 0xFFFFFF
#define SETRET(type) (1 << MSG_SHIFT_RETURN) | (type)
class Actor;
class Message {
public:
    const char *data;
    int size;
    uint16_t type;
    uint64_t session;
    int source;
    int dst;
    ~Message(){
        if(data != NULL){
            delete data;
            data = NULL;
        }
    }
};
class MsgQueue{
public:
    MsgQueue()
    {
        //queues_.resize(1024);
    }
    void PushMsg(Message *msg){
        printf("push msg\n");
        queues_.push_back(msg);
    }
    Message* PopMsg(){
        Message *msg = NULL;
        std::deque<Message*>::iterator it = queues_.begin();
        if(it != queues_.end()){
            msg = *it;
            queues_.pop_front();
        }
        return msg;
    }
    int size(){
        return queues_.size();
    }
private:
    std::deque<Message*> queues_;
};
class GlobalQueue{
public:
    void PushActor(Actor* actor){
        printf("push actor\n");
        queues_.push_back(actor);
    }
    Actor* PopActor(){
        printf("queue size=%d\n", (int)queues_.size());
        Actor *act = NULL;
        std::deque<Actor*>::iterator it = queues_.begin();
        if(it != queues_.end()){
            act = *it;
            queues_.pop_front();
        }
        return act;
    }
    int size(){
        return queues_.size();
    }
private:
    std::deque<Actor*> queues_;
};
#endif
