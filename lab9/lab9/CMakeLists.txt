cmake_minimum_required(VERSION 3.0)
project(lab9 VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEPS_DIR "../deps")

set(CSCI441_DIR ${DEPS_DIR}/csci441)
include_directories(${CSCI441_DIR}/include)

set(GLM_DIR ${DEPS_DIR}/glm)
include_directories(${GLM_DIR})

set(BITMAP_DIR ${DEPS_DIR}/bitmap)
include_directories(${BITMAP_DIR}/include)


set(PLF_DIR ${DEPS_DIR}/plf)
include_directories(${PLF_DIR}/include)

set(LAB ${PROJECT_NAME})
add_executable(${LAB} main.cpp)
target_link_libraries(${LAB})
