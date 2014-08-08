#ifndef __NB_CORE_H__
#define __NB_CORE_H__
extern "C" {
#include "lualib.h"
#include "lua.h"
}
#include <stdint.h>
#include <map>
#define SS_READY 10
#define SS_SUSPEND 11
#define SS_RUNNING 12
#define SS_DONE 13
class Actor;
struct Ctx;
class GlobalQueue;
class Message;
struct Session{
    uint64_t id;
    lua_State *co;
    int co_ref;
    int8_t status;
};
struct Ctx{
    int handle; 
    Actor *actor;
    Session *cur_session;
    std::map<uint64_t, Session*>  sessions;
};
extern GlobalQueue *g_act_queue;
class KnewbiCore{
public:
    KnewbiCore():
        handle(0),
        sessionid(0)
    {};
    int Start(int threadnum);
    int Setup(const char* main);
    int GenHandle(){
        //should be thread safe
        return handle ++;
    }
    uint64_t GenSessionId(){
        return sessionid++;
    }
    Actor* RegActor(const char *name, int type, uint32_t address);
    Ctx* GetCtxByName(const char* name);
    Ctx* GetCtxByHandle(int handle);
    void Dispatch(Ctx *sctx, Ctx *dctx, int type, const char* data, int size, bool iscopy);
    static KnewbiCore* Instance(){
        if(!inst){
            inst = new KnewbiCore();
        }
        return inst;
    }
private:
    int handle;
    static KnewbiCore* inst;
    uint64_t sessionid;
};
#endif
