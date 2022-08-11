#include <stdio.h>
#include <cassert>
#include "lua_navmesh/lua_navmesh_export.h"
#include "student/Student.h"
extern "C" int luaopen_luna(lua_State * L);

int main(int argc, char** argv)
{
	lua_State* m_L = luaL_newstate();
	if (!m_L)
		return false;
	luaL_requiref(m_L, "navmesh", luaopen_navmesh, 0);
	luaL_requiref(m_L, "student", luaopen_student, 0);
	luaL_openlibs(m_L);

	auto ret = luaL_dofile(m_L, "../../app/demo/main.lua");
	if (ret == 0) {
		printf("import main.lua success.\n");
	}
	else {
		printf("import main.lua failed. Error: %s", lua_tostring(m_L, -1));
	}
    return 0;
}