cmake_minimum_required(VERSION 3.12)
set(CMAKE_SUPPRESS_REGENERATION TRUE CACHE BOOL "Disable Zero Check Project")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(PROJ_NAME "luna-navmesh-demo")
project(${PROJ_NAME})

set(LUA_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lua/src")
aux_source_directory(${LUA_SRC_PATH} LUA_SRC)

set(LUNA_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/luna")
include_directories(${LUA_SRC_PATH})
aux_source_directory(${LUNA_SRC_PATH} LUNA_SRC)
add_library(luna STATIC ${LUA_SRC} ${LUNA_SRC})
set(LUNA_INCLUDE_DIRS ${LUA_SRC_PATH} ${LUNA_SRC_PATH})

add_subdirectory(Navmesh)
add_subdirectory(lua_navmesh)
add_subdirectory(app)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "app")