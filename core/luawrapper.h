#ifndef __LUA_WRAPPER_H__
#define __LUA_WRAPPER_H__
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#define METHOD(class, name) {#name, &class::name}
        
template <typename T> class LuaWrapper{
public:
    typedef struct {
        T *pT;
    }UserData;
    typedef int (T::*RegFunc)(lua_State *L);
    typedef struct {
        const char *name;
        RegFunc func;
    }RegMethod;
    static void Register(lua_State *L, const char* cstr = NULL){
        lua_newtable(L);
        int methods = lua_gettop(L);
        luaL_newmetatable(L, T::className);
        int metatable = lua_gettop(L);

        lua_pushliteral(L, "__index");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);

        lua_pushliteral(L, "__metatable");
        lua_pushvalue(L, methods);
        lua_settable(L, metatable);

        lua_pushliteral(L, "__gc");
        lua_pushcfunction(L, Destructor);
        lua_settable(L, metatable);

        lua_pushcfunction(L, Constructor);
        if(cstr == NULL){
            lua_setglobal(L, T::className);
        }else{
            lua_setglobal(L, cstr);
        }

        for(RegMethod *l = T::methods; l->name; l++){
            lua_pushlightuserdata(L, (void*)l);
            lua_pushcclosure(L, CallBack, 1);
            lua_setfield(L, methods, l->name);
        }

    }
    static T* Check(lua_State* L, int arg){
        UserData* ud = static_cast<UserData*>(luaL_checkudata(L, arg, T::className));
        return ud->pT;
    }
    static int CallBack(lua_State *L){
        printf("CallBack ..%d\n", lua_gettop(L));
        T *obj = Check(L, 1);
        lua_remove(L, 1);
        RegMethod *l = static_cast<RegMethod*>(lua_touserdata(L, lua_upvalueindex(1)));
        return (obj->*(l->func))(L);
        
    }
    static int Constructor(lua_State *L){
        printf("Constructor ..\n");
        T *obj = new T(L);
        UserData* ud = static_cast<UserData*>(lua_newuserdata(L, sizeof(UserData)));
        ud->pT = obj;
        luaL_getmetatable(L, T::className);
        lua_setmetatable(L, -2);
        return 1;
    }
    static int Destructor(lua_State *L){
        T *obj = Check(L, 1);
        delete obj;
        return 0;
    }
private:
    LuaWrapper();
};
#endif
