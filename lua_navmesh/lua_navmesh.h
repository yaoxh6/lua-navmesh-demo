#pragma once
#include "luna.h"
#include "recast_navmesh.h"

FVector static lua_to_vector(lua_State* L, int index)
{
    FVector v;
    lua_getfield(L, index, "x");
    v.x = (dtReal)lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "y");
    v.y = (dtReal)lua_tonumber(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, index, "z");
    v.z = (dtReal)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return v;
}

int static lua_push_vector(lua_State* L, const FVector& v)
{
    lua_newtable(L);
    lua_pushnumber(L, v.x);
    lua_setfield(L, -2, "x");
    lua_pushnumber(L, v.y);
    lua_setfield(L, -2, "y");
    lua_pushnumber(L, v.z);
    lua_setfield(L, -2, "z");
    return 1;
}

class navmesh {
public:
    navmesh() = default;
    ~navmesh();
    DECLARE_LUA_CLASS(navmesh);
    int find_path(lua_State* L);
    bool init_navmesh(const char* nav_data_path, uint16_t include_flag, uint16_t exclude_flag, FVector nav_query_extent);
    int set_flags(lua_State* L);
    int set_extent(lua_State* L);

private:
    recast_navmesh m_navmesh;
};

