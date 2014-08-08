extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#include "nblua.h"
#include "nbactor.h"
#include "nbcore.h"
int NbLua::lstart(lua_State *L){
    printf("===start args=%d ===\n", lua_gettop(L));
    int threadnum = lua_tonumber(L, 1); 
    const char* main = lua_tostring(L, 2);
    if(threadnum == 0){
        threadnum = 1;
    }
    if(main == NULL){
        printf("plz set main lua file\n");
        return 0;
    }
    KnewbiCore *core = KnewbiCore::Instance();
    core->Init("");
    
    if(!core->Start(threadnum))
    {
        printf("start error\n");
    }
    //launch main actor
    Actor *actor = core->RegActor("main", LOCAL_ACTOR, 0); 
    lua_pushstring(L, "main");
    lua_replace(L, -3);
    actor->Launch(L);
    //send to main 
    lua_State *ml = actor->l_;
    lua_settop(ml, 0);
    lua_pushstring(ml, "main");
    lua_pushstring(ml, "CALLBACK MAIN");
    _Dispatch(ml, false);
    //Actor::CreateEnv(L);
    //lua_pushlightuserdata(L, (void*)actor);
    //Actor::SetEnv(L, "actor");

    return 0;

}
int NbLua::lsend(lua_State *L){
    return _Dispatch(L, false);
    //should yield here
    //return 0;
}
int NbLua::lcall(lua_State* L){
    return _Dispatch(L, true);
}

int NbLua::lsay(lua_State* L){
    Message *org_msg;
    lua_pushthread(L);
    lua_getfenv(L, -1);
    lua_getfield(L, -1, "msg");
    org_msg = (Message*)lua_touserdata(L, -1);
    lua_pop(L, 3);
    if(org_msg == NULL){
        printf("not exist orignal message \n");
        lua_pushnumber(L, -1);
        return 1;
    }
    size_t len;
    const char *ret_data = lua_tolstring(L, -1, &len);

    if(ISRET(org_msg->type)){
        printf("msg type dont return msg\n");
        //push msg, error num
        lua_pushnumber(L, -1);
        return 1;
    }

    KnewbiCore *nb = KnewbiCore::Instance();
    
    Ctx *dctx = nb->GetCtxByHandle(org_msg->source); 
    Ctx *sctx = nb->GetCtxByHandle(org_msg->dst); 
    int type = dctx->actor->GetActorType();
    if( type == REMOTE_ACTOR){
        //TODO send to gate actor   
        //put remote actor handle in type
        //replace dcxt to gate actor
    }
    if(type == LOCAL_ACTOR){
        type = MSG_LOCAL_RES;
    }
    nb->Dispatch(dctx, sctx, type, ret_data, len, true);
    lua_pushnil(L);
    return 1;
    
}

int NbLua::lreg(lua_State *L){
    printf("reg actor\n");
    const char *actname = lua_tostring(L, 1);
    KnewbiCore *core = KnewbiCore::Instance();
    Actor *actor = core->RegActor(actname, LOCAL_ACTOR, 0);
    actor->Launch(L); return 1;
}
int NbLua::lcallback(lua_State *L){
    printf("callback func\n");
    luaL_checktype(L, 1, LUA_TFUNCTION);
    Actor::SetEnv(L, LUA_FCALLBACK);
    return 0;
}
int NbLua::lloopwait(lua_State *L){
    int second = lua_tonumber(L, -1);
    if(second == 0){
        second = 5;
    }
    while(1){
        sleep(second);
    }
    return 0;
}

lua_State* NbLua::_InitLuaVm(){ 
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    Actor::CreateEnv(L);
    return L;
}
int NbLua::_Dispatch(lua_State *L, bool isret){

    const char *dst = lua_tostring(L, 1);
    size_t size = 0;
    const char *data = lua_tolstring(L, 2, &size);

    Actor::GetEnv(L, "actor");
    Actor *act = static_cast<Actor*>(lua_touserdata(L, -1));
    if(act == NULL){
        printf("lua state get actor is NULL\n");
        return -1;
    }

    KnewbiCore *nb = KnewbiCore::Instance();
    Ctx* dctx = nb->GetCtxByName(dst);
    if(dctx == NULL){
        printf("not exist service:%s\n", dst);
        return 0;
    }
    int type = dctx->actor->GetActorType();
    if(type == REMOTE_ACTOR){
        type = MSG_REMOTE_REQ;
        //TODO should send to gate actor
        // put handle int type, 
        // dctx = gate;

    }else if(type == LOCAL_ACTOR){
        type = MSG_LOCAL_REQ;
    }
    if(isret){
        type = SETRET(type);
    }
    nb->Dispatch(act->ctx, dctx, type, data, size, true);
    if(!isret){
        return 0;
    }
    return lua_yield(L, 0);
}

lua_State* NbLua::_InitLuaThread(lua_State *L, Message *msg, int *ref){
    int base = lua_gettop(L);
    lua_State *co = lua_newthread(L);
    /*{{{ ref coroutine, prevent lua gc*/
    //lua_rawgetp(L, LUA_REGISTRYINDEX, NB_TAG); 
    lua_pushlightuserdata(L, NB_TAG);
    lua_rawget(L, LUA_REGISTRYINDEX); 
    lua_pushvalue(L, -2);
    *ref = luaL_ref(L, -2);
    if(*ref == LUA_NOREF){
        printf("ref error\n");
    }

    /*}}} */
    //set env
    lua_createtable(L, 0, 1); //t1 = {}
    lua_createtable(L, 0, 1); // t2 = {} 
    lua_getglobal(L, "_G"); // _G = {}
    lua_setfield(L, -2, "__index"); // t2.__index = _G
    lua_setmetatable(L, -2); // t1.mt = t2
    
    lua_pushlightuserdata(L, msg); //t1.msg = msg
    lua_setfield(L, -2, "msg");
    // setfenv(L, co) t1 table
    lua_setfenv(L, -2);
    
    lua_settop(L, base);
    return co;
}
extern "C" int luaopen_knewbi(lua_State *L){
    luaL_Reg l[] = {
        {"send", NbLua::lsend},
        {"call", NbLua::lcall},
        {"say", NbLua::lsay},
        {"start", NbLua::lstart},
        {"reg", NbLua::lreg},
        {"callback", NbLua::lcallback},
        {"loopwait", NbLua::lloopwait},
        {NULL, NULL}
    };
    luaL_register(L, "knewbi", l);
    return 1;
}
