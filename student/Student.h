#pragma once
extern "C" {
#include "lua.h"  
#include "lauxlib.h"  
#include "lualib.h"  
}
extern "C" int luaopen_student(lua_State * L);