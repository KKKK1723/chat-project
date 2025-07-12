cmake_minimum_required(VERSION 3.10)
project(GateServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 查找 Boost
find_package(Boost REQUIRED COMPONENTS system)

# 查找 jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 自动收集所有 cpp 文件
file(GLOB SOURCES "*.cpp")

add_executable(GateServer ${SOURCES})

target_link_libraries(GateServer
    ${Boost_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    pthread
) 