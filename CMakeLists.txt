cmake_minimun_required(VERSION 2.0)
project(yolo)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

set(LIB_SRC
    yola/log.cc
    )

add_library(yolo SHARED ${LIB_SRC})
#add_library(yolo_static STATIC $(LIB_SRC))
#SET_TARGET_PROPERTIES (yolo_static PROPERTIES OUTPUT_NAME "yolo")

add_executbale(test testa/test.cc)
add_dependencies(test yolo)
target_link_libraries(test yolo)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)