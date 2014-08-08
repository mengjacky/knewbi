#include "nbserver.h"
#include "nbactor.h"
#include "nbcore.h"
const char NbServer::className[] = "NbServer";
LuaWrapper<NbServer>::RegMethod NbServer::methods[] = {

    METHOD(NbServer, Launch),
    METHOD(NbServer, Listen),
    {0, 0}
};

NbServer::NbServer(lua_State *L){
    threadnum_ = luaL_checkint(L, 1);
    if(lua_iscfunction(L, 2)){
        Actor::SetEnv(L, LUA_FCALLBACK);
    }
}

int NbServer::Launch(lua_State *L){
    printf("reg actor\n");
    const char *actname = lua_tostring(L, 1);
    KnewbiCore *core = KnewbiCore::Instance();
    Actor *actor = core->RegActor(actname, LOCAL_ACTOR, 0);
    if(!actor->Launch(L)){
        lua_pushnumber(L, 1);

    }else{
        lua_pushnil(L);
    }
    return 1;
}
int NbServer::Listen(lua_State *L){
    //TODO launch gate actor
    KnewbiCore *core = KnewbiCore::Instance();
    if(!core->Start(threadnum_))
    {
        printf("start error\n");
    }
    return 0;
}
