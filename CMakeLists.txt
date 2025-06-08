cmake_minimum_required(VERSION 3.5)
project(TDFM)

include_directories(include)

set(libsource
    src/base_op.c 
    src/tdfm.c
)

add_library(TDFM SHARED ${libsource})

add_executable(test_init test/test_init.c)
add_executable(test_read test/test_read.c)

add_executable(lbash src/lbash.c src/base_op.c)

add_executable(control src/tdfm_control.c)

target_link_libraries(lbash TDFM)
target_link_libraries(test_init TDFM)
target_link_libraries(test_read TDFM)
target_link_libraries(control TDFM)


add_executable(fm_TDFM WIN32
    src/fm/main.cpp 
    src/fm/msg.cpp 
    src/fm/archive.cpp
    src/fm/win_utils.cpp
)

target_link_libraries(fm_TDFM TDFM)

target_compile_options(TDFM PRIVATE "/MT")
target_compile_options(fm_TDFM PRIVATE "/MT")