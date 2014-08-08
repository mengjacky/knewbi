#include  <map> 
#include <string.h>
#include <vector>

#include "nbcore.h"
#include "nbactor.h"
#include "msgqueue.h"
#include "nbthread.h"
#include "scheduler.h"
std::map<std::string, Ctx*> g_map_ctx;
std::vector<Ctx*> g_ctx_list; 
KnewbiCore* KnewbiCore::inst = NULL;
GlobalQueue *g_act_queue ;
int KnewbiCore::Setup(const char* main)
{
    g_act_queue = new GlobalQueue();
    Actor *actor = RegActor(MAIN_LUA, LOCAL_ACTOR, 0); 
    return actor->Launch(MAIN_LUA, main);
}
int KnewbiCore::Start(int threadnum){
    //1. start worker thread
    for(int i = 0; i < threadnum; ++i){
        Scheduler *s = new Scheduler();
        Thread *thd = new Thread(s);
        thd->Start();
    }
    //2. start facade thread
    //3. start socket server 
    return 1;
}
Actor* KnewbiCore::RegActor(const char* actname, int type, uint32_t address)
{
    Ctx *ctx = new Ctx();
    ctx->handle = GenHandle();

    Actor *actor = new Actor();
    actor->handle = ctx->handle;
    actor->SetActorType(type);
    actor->ctx = ctx;
    ctx->actor = actor;
    std::string tmp(actname);
    if(type == REMOTE_ACTOR){
        actor->address = address;

    }else if(type == LOCAL_ACTOR){
        actor->address = handle;
    }

    g_map_ctx[actname] = ctx;
    g_ctx_list.push_back(ctx);

    return actor;
}
Ctx* KnewbiCore::GetCtxByName(const char* name){
    std::map<std::string, Ctx*>::iterator it = g_map_ctx.begin();
    if(it == g_map_ctx.end()) {
        return NULL;
    }
    return it->second; 
}
Ctx* KnewbiCore::GetCtxByHandle(int handle){
    if(handle > (int)g_ctx_list.size()){
        return NULL;
    }
    return g_ctx_list[handle];
}
void KnewbiCore::Dispatch(Ctx *sctx, Ctx *dctx, int type, const char* data, int size, bool iscopy){
    char* d = (char*)data;
    if(iscopy){
        d = new char[size];
        memcpy(d, data, size);
    }
    Message *m = new Message();
    m->data = d;
    m->size = size;
    m->source = sctx->handle;
    m->dst = dctx->handle;
    m->type = type;
    if(ISRET(type)){
        uint64_t id = GenSessionId();
        Session *s = new Session();
        s->id = id;
        s->status = SS_SUSPEND;
        sctx->sessions[id] = s;
        sctx->cur_session = s;
    }
    dctx->actor->PushMsg(m);
}

