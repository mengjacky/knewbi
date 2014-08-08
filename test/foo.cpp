#include "luawrapper.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}
class Foo{
public: static const char className[] ;
    static LuaWrapper<Foo>::RegMethod methods[];
    Foo(lua_State *L);
    int Add(lua_State *L);
    int Minus(lua_State *L);
private:
    int m;
};
const char Foo::className[] = "Foo";
LuaWrapper<Foo>::RegMethod Foo::methods[] = {

    METHOD(Foo, Add),
    METHOD(Foo, Minus),
    {0, 0}
};
Foo::Foo(lua_State *L){
    m = luaL_checknumber(L, -1);
}
int Foo::Add(lua_State *L){
    int a = luaL_checknumber(L, 1);
    int b = luaL_checknumber(L, 2);
    lua_pushnumber(L, (a+b)*m);
    return 1;
}
int Foo::Minus(lua_State *L){

    int a = luaL_checknumber(L, 1);
    int b = luaL_checknumber(L, 2);
    printf("%d %d %d\n", a, b, m);
    lua_pushnumber(L, (a - b)*m);
    return 1; 
}
int main(int argc, char *argv[]){

    if(argc < 2){
        printf("lack arg.");
    }
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    LuaWrapper<Foo>::Register(L);
    if(luaL_loadfile(L, argv[1]) || lua_pcall(L, 0, 0, 0)){
        printf("error:%s", lua_tostring(L, -1));
        return -1;
    }
}
