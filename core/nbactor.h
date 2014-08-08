#ifndef __NB_ACTOR_H__
#define __NB_ACTOR_H__
extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include <string>
#include <stdint.h>
#include "msgqueue.h"
static int NB = 0;
#define NB_TAG (&NB)

#define REMOTE_ACTOR 1
#define LOCAL_ACTOR 2
#define LUA_FCALLBACK "dispatch"
#define MAIN_LUA "main"
extern GlobalQueue *g_act_queue;
class Ctx;
class Session;
class Actor{
public:
    int Launch(const char* actname, const char* path);
    static void CreateEnv(lua_State *L);
    static void SetEnv(lua_State *L, const char* key);
    static void GetEnv(lua_State *L, const char* key);
    virtual int HandleMsg(Message* msg);
    virtual void DestroySession(Session **s);
    virtual void DestroyMsg(Message **msg);
    int GetActorType(){ return type;}
    void SetActorType(int t){ type = t;}

    void PushMsg(Message *msg){
        //lock  
        msgqueue_.PushMsg(msg);
        if(msgqueue_.size() == 1){
            g_act_queue->PushActor(this);
        }
    }
    Message* PopMsg(){
        return msgqueue_.PopMsg(); 
    }
    int MsgSize(){
        return msgqueue_.size();
    }
    //int Resume(const char *arg, int size);
    Session* GetSession(uint64_t sessionid);
public:
    lua_State* l_;
    std::string name;
    uint32_t address;
    int handle;
    Ctx *ctx;
private:
    MsgQueue msgqueue_;
    int type;
};

class LuaActor : public Actor {
    int HandleMsg(Message* msg);
};
class CActor : public Actor {

    int HandleMsg(Message* msg);
};
#endif
