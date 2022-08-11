set(APP_NAME "app")

aux_source_directory(. SOURCE_FILES)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${LUNA_INCLUDE_DIRS})

add_executable(${APP_NAME} ${SOURCE_FILES})
target_link_libraries(${APP_NAME} lua_navmesh student)