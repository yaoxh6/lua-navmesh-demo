get_filename_component(CURRENT_SOURCE_DIR_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
set(LIB_NAME ${CURRENT_SOURCE_DIR_NAME}) # 工程名, 默认文件夹名

file(GLOB SOURCE_FILES 
    ${CMAKE_SOURCE_DIR}/include/*.h
    *.cpp
    *.cc
    *.c
    *.h
)
add_library(${LIB_NAME} STATIC ${SOURCE_FILES})