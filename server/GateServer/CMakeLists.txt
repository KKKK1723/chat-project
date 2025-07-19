# cmake_minimum_required(VERSION 3.10)
# project(GateServer)

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# # 查找 Boost
# find_package(Boost REQUIRED COMPONENTS system)

# # 查找 jsoncpp
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(JSONCPP jsoncpp)

# include_directories(
#     ${Boost_INCLUDE_DIRS}
#     ${JSONCPP_INCLUDE_DIRS}
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

# # 自动收集所有 cpp 文件
# file(GLOB SOURCES "*.cpp")

# add_executable(GateServer ${SOURCES})

# target_link_libraries(GateServer
#     ${Boost_LIBRARIES}
#     ${JSONCPP_LIBRARIES}
#     protobuf
#     grpc++
#     grpc
#     pthread
# )

cmake_minimum_required(VERSION 3.10)
project(GateServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 强制使用新安装的protobuf
set(CMAKE_PREFIX_PATH "/usr/local" ${CMAKE_PREFIX_PATH})
link_directories(/usr/local/lib)

find_package(Boost REQUIRED COMPONENTS system filesystem)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
pkg_check_modules(PROTOBUF REQUIRED protobuf>=3.13.0)

include_directories(
    ${Boost_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(GateServer 
    GateServer.cpp
    HttpConnection.cpp
    LogicSystem.cpp
    ConfigMgr.cpp
    CServer.cpp
    message.pb.cc
    message.grpc.pb.cc
)

target_link_libraries(GateServer
    /usr/local/lib/libprotobuf.so
    grpc++
    grpc
    ${Boost_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    pthread
)