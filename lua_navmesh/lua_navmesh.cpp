#include "lua_navmesh.h"
LUA_EXPORT_CLASS_BEGIN(navmesh)
LUA_EXPORT_METHOD(set_flags)
LUA_EXPORT_METHOD(set_extent)
LUA_EXPORT_METHOD(find_path)
LUA_EXPORT_CLASS_END()

int lua_push_path_point(lua_State* L, const std::vector<FNavLocation>& path_points)
{
    lua_newtable(L);
    for (int i = 0; i < path_points.size(); i++) {
        lua_pushinteger(L, i + 1);
        lua_push_vector(L, path_points[i].Location);
        lua_settable(L, -3);
    }
    return 1;
}

 navmesh::~navmesh() { 

 }

bool navmesh::init_navmesh(const char* nav_data_path, uint16_t include_flag, uint16_t exclude_flag, FVector nav_query_extent)
 { 
    auto load_res =  m_navmesh.LoadNavMeshFile(nav_data_path);
    if (!load_res) {
        return load_res;
    }
    auto init_nav_query_res =  m_navmesh.InitNavQuery(include_flag, exclude_flag, nav_query_extent);
    if (!init_nav_query_res) {
        return load_res;
    }
    return true;
}

int navmesh::set_flags(lua_State* L) { 
    auto include_flag = lua_tonumber(L, 1);
    auto exclude_flag = lua_tonumber(L, 2);
    m_navmesh.SetFlags(include_flag, exclude_flag);
    return 0;
}

int navmesh::set_extent(lua_State* L) { 
    auto query_extent = lua_to_vector(L, 1);
    m_navmesh.SetExtent(query_extent);
    return 0;
}

int navmesh::find_path(lua_State* L)
{
    auto start_pos = lua_to_vector(L, 1);
    auto end_pos = lua_to_vector(L, 2);
    if (!m_navmesh.FindPath(start_pos, end_pos)) {
        return 0;
    }
    lua_push_path_point(L, m_navmesh.GetPathPoint());
    return 1;
}