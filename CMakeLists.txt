cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME cpplua)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

find_package(Lua REQUIRED)
include_directories(${LUA_INCLUDE_DIR})

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_tests ./test/main.cpp)
target_link_libraries(${PROJECT_NAME}_tests ${GTEST_BOTH_LIBRARIES} ${LUA_LIBRARIES})
