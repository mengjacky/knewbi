#ifndef __NB_LUA_H__
#define __NB_LUA_H__
extern "C"{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
class Message;
class NbLua{

public:
    static int lstart(lua_State *L);
    //no return msg 
    static int lsend(lua_State *L);
    //got return msg
    static int lcall(lua_State *L);
    //return msg
    static int lsay(lua_State *L);

    static int lreg(lua_State *L);
    //lua callback when first launch
    static int lcallback(lua_State *L);
    static int lloopwait(lua_State *L);
    static lua_State* _InitLuaVm();
    static lua_State* _InitLuaThread(lua_State *L, Message *msg, int *ref);
    static int _Dispatch(lua_State *L, bool isremote);

};
#endif
