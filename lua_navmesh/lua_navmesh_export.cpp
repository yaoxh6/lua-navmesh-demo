#include "luna.h"
#include "lua_navmesh.h"
#include "lua_navmesh_export.h"
#include <cassert>

static int lua_navmesh_create(lua_State* L)
{
    const char* nav_data_path = lua_tostring(L, 1);
    auto include_flag = lua_tonumber(L, 2);
    auto exclude_flag = lua_tonumber(L, 3);
    auto nav_query_extent = lua_to_vector(L, 4);
    auto m_navmesh = new navmesh();
    auto init_res = m_navmesh->init_navmesh(nav_data_path, include_flag, exclude_flag, nav_query_extent);
    if (!init_res) {
        m_navmesh->~navmesh();
        return 0;
    }
    lua_push_object(L, m_navmesh);
    return 1;
}

int luaopen_navmesh(lua_State* L)
{
    luaL_checkversion(L);
    luaL_Reg l[] = {
        { "create", lua_navmesh_create},
        { NULL, NULL },
    };
    luaL_newlib(L, l);
    return 1;
}
