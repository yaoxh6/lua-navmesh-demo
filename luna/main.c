#define LUA_LIB

#include <lauxlib.h>
#include <lua.h>

extern void cache_lua_state(lua_State* L);

LUAMOD_API int luaopen_luna(lua_State* L)
{
    luaL_checkversion(L);
    cache_lua_state(L);

    luaL_Reg l[] = {
        { NULL, NULL },
    };
    luaL_newlib(L, l);
    return 1;
}