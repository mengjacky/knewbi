#include "nbactor.h"
#include "nblua.h"
#include "nbcore.h"
#include "stdint.h"
int Actor::Launch(const char* actname, const char* path)
{
    l_ = NbLua::_InitLuaVm();
    printf("path = %s\n", path);
    lua_pushlightuserdata(l_, (void*)this);
    SetEnv(l_, "actor");
    
    if(luaL_loadfile(l_, path)){
        printf("load file error:%s\n", lua_tostring(l_, -1));
        return -1;
    }
    if(lua_pcall(l_, 0, 0, 0)){
        printf("launch error:%s \n", lua_tostring(l_, -1));
        return -1;
    }
    printf("launch %s success\n", actname);
    return 0;
}

void Actor::CreateEnv(lua_State *L){
    lua_pushlightuserdata(L, NB_TAG); 
    lua_newtable(L);
    //lua_rawsetp(L, LUA_REGISTRYINDEX, NB_TAG);
    lua_rawset(L, LUA_REGISTRYINDEX);
}
void Actor::SetEnv(lua_State *L, const char* key){
    //lua_rawgetp(L, LUA_REGISTRYINDEX, NB_TAG);
    lua_pushlightuserdata(L, NB_TAG); 
    lua_rawget(L, LUA_REGISTRYINDEX); 
    lua_insert(L, -2);
    lua_setfield(L, -2, key);
    lua_pop(L, 1);

}
void Actor::GetEnv(lua_State *L, const char *key){
    //lua_rawgetp(L, LUA_REGISTRYINDEX, NB_TAG); 
    lua_pushlightuserdata(L, NB_TAG); 
    lua_rawget(L, LUA_REGISTRYINDEX); 
    lua_getfield(L, -1, key);
    lua_replace(L, -2);
}

Session* Actor::GetSession(uint64_t sessionid){
        std::map<uint64_t, Session*>::iterator it = ctx->sessions.find(sessionid);
        if(it != ctx->sessions.end()){
            return it->second;
        }
        return NULL;
}

int Actor::HandleMsg(Message *msg){
    if(msg->dst != handle){
        printf("dispatch message error, should be in %d, but in %d\n", msg->dst, handle);
        return ERROR_DISPATCH; 
    }
    int rv ;
    Session *s = NULL;
    lua_State *co;
    int ref = 0;
    int type = GETTYPE(msg->type);
    //int isret = ISRET(msg->type);
    
    printf("handle msg=%d\n", type);
    switch (type){
        case MSG_LOCAL_RES:
        {

            s = GetSession(msg->session);
            if(s == NULL){
                return ERROR_NOT_EXIST_SESSION ;
            }
            if(s->status != SS_SUSPEND){
                DestroySession(&s);
                return ERROR_SESSION_STATUS;
            }
            if(s->co == NULL){
                DestroySession(&s);
                return ERROR_NOT_EXIST_CO;
            }
            s->status = SS_DONE;
            ctx->cur_session = s;
            co = s->co;
            ref = s->co_ref;
            lua_pushlstring(co, msg->data, msg->size);
            rv = lua_resume(co, 1);
            goto RESUME;
        }
         
        case MSG_LOCAL_REQ:
        {
            co = NbLua::_InitLuaThread(l_, msg, &ref);
            //ctx->comessage[co] = msg;
            GetEnv(l_, LUA_FCALLBACK);
            lua_xmove(l_, co, 1);
            lua_pushlstring(co, msg->data, msg->size);
            rv = lua_resume(co, 1);
            goto RESUME;
        }

        default:
        {
            printf("error msgtype");
            return 1;
        }
    }
RESUME:
    switch(rv){
        case LUA_YIELD:
            {
                printf("lua yield\n");
                //delete pre session
                if(s != NULL){
                    DestroySession(&s);
                }
                //get sesion from lua_yield
                //s = (Session*)lua_touserdata(l_, -1);
                s = ctx->cur_session;
                s->co = co;
                s->co_ref = ref;
                break;
            }
        case 0:
            {
                Message *org_msg;
                lua_pushthread(co);
                lua_getfenv(co, -1);
                lua_getfield(co, -1, "msg");
                org_msg = (Message*)lua_touserdata(co, -1);
                lua_pop(co, 3);

                DestroyMsg(&org_msg);
                if(org_msg != msg){
                    DestroyMsg(&msg);
                }
                //delete cur_session
                if(ctx->cur_session->co == co){
                    ctx->cur_session->status = SS_DONE;
                    DestroySession(&ctx->cur_session);
                }
                luaL_unref(l_, LUA_REGISTRYINDEX, ref);
                break;
            }
    }
    return 0;
}
void Actor::DestroySession(Session **s){
    if(*s == NULL){
        printf("session is null\n");
        return ;
    }
    std::map<uint64_t, Session*>::iterator it = ctx->sessions.find((*s)->id);
    if(it != ctx->sessions.end()){
        ctx->sessions.erase(it);
    }
    if((*s)->status != SS_DONE){
        printf("status error, should be done\n");
    }
    delete *s;
    *s = NULL;

}
void Actor::DestroyMsg(Message **msg){
    delete *msg;
    *msg = NULL;
}

