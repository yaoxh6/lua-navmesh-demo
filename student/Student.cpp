#include <iostream>
#include "Student.h"
using namespace std;

class StudentTag
{
public:
	char *strName;
	int iAge;
};

static int Student(lua_State *L)
{
	size_t iBytes = sizeof(StudentTag);
	StudentTag *pStudent;
	pStudent = (StudentTag *)lua_newuserdata(L, iBytes);

	 //设置元表
	luaL_getmetatable(L, "Student");
	lua_setmetatable(L, -2);

	return 1; // 新的userdata已经在栈上了
}

static int GetName(lua_State *L)
{
	StudentTag *pStudent = (StudentTag *)luaL_checkudata(L, 1, "Student");
	lua_pushstring(L, pStudent->strName);
	return 1;
}

static int SetName(lua_State *L)
{
	// 第一个参数是userdata
	StudentTag *pStudent = (StudentTag *)luaL_checkudata(L, 1, "Student");

	// 第二个参数是一个字符串
	const char *pName = luaL_checkstring(L, 2);
	luaL_argcheck(L, pName != NULL && pName != "", 2, "Wrong Parameter");

	pStudent->strName =(char*) pName;
	return 0;
}

static int GetAge(lua_State *L)
{
	StudentTag *pStudent = (StudentTag *)luaL_checkudata(L, 1, "Student");
	lua_pushinteger(L, pStudent->iAge);
	return 1;
}

static int SetAge(lua_State *L)
{
	StudentTag *pStudent = (StudentTag *)luaL_checkudata(L, 1, "Student");

	int iAge = luaL_checkinteger(L, 2);
	luaL_argcheck(L, iAge >= 6 && iAge <= 100, 2, "Wrong Parameter");
	pStudent->iAge = iAge;
	return 0;
}

static  luaL_Reg arrayFunc_meta[] =
{
	{ "getName", GetName },
	{ "setName", SetName },
	{ "getAge", GetAge },
	{ "setAge", SetAge },
	{ NULL, NULL }
};

static luaL_Reg arrayFunc[] =
{
	{ "new", Student},
	{ NULL, NULL }
};

int luaopen_student(lua_State *L)
{
	// 创建一个新的元表
	luaL_newmetatable(L, "Student");
	// 元表.__index = 元表
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, arrayFunc_meta, 0);
	luaL_newlib(L, arrayFunc);
	return 1;
}
