#ifndef __NB_SERVER_H__
#define __NB_SERVER_H__
#include "luawrapper.h"
#include "lua.hpp"
class NbServer{
public:
    static const char className[] ;
    static LuaWrapper<NbServer>::RegMethod methods[];
    int Launch(lua_State *L);
    int Listen(lua_State *L); 
    
    NbServer(lua_State *L);
private:
    int threadnum_;

};
#endif
