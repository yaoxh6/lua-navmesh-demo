get_filename_component(CURRENT_SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(LIB_NAME ${CURRENT_SOURCE_DIR_NAME}) # 工程名, 默认文件夹名

set(RECAST_NAV_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Public)
set(RECAST_NAV_INCLUDE_DIRS ${RECAST_NAV_INCLUDE_DIRS} PARENT_SCOPE)

include_directories(${RECAST_NAV_INCLUDE_DIRS})

file(GLOB_RECURSE ALL_SOURCE_FILES
        Public/*.h
        Private/Detour/*.cpp
        Private/DetourTileCache/*.cpp
)

add_library(${LIB_NAME} STATIC ${ALL_SOURCE_FILES})