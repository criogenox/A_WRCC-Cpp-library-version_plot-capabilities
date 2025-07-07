cmake_minimum_required(VERSION 3.31)
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)
set(PROJECT_NAME "WRCC")
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)
set(default_build_type "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(TARGET ${PROJECT_NAME})
#-----------#-----------#-----------#

set(LIBS_PATH "${PROJECT_SOURCE_DIR}/libs")
set(HEADERS_PATH "${PROJECT_SOURCE_DIR}/headers")
set(SRC_PATH "${PROJECT_SOURCE_DIR}/src")
set(DATA_PATH "${PROJECT_SOURCE_DIR}/data")
set(PYLIBS_PATH "/home/<user_name>/.local/lib/")
set(PYHS_PATH "/usr/include/python3.12/")
#-----------#-----------#-----------#

include_directories(${PYHS_PATH} ${HEADERS_PATH})
link_directories(${PYLIBS_PATH} ${LIBS_PATH})

file(GLOB_RECURSE CPPS "${SRC_PATH}/*.cpp")
#-----------#-----------#-----------#

add_executable(${TARGET} ${CPPS})
target_link_libraries(${TARGET} eqcon_lib conpon_lib python3.12)
#-----------#-----------#-----------#
