cmake_minimum_required(VERSION 3.10)
project(GateServer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# 使用 pkg-config 查找库
find_package(PkgConfig REQUIRED)

# 查找所需的库
find_package(Boost REQUIRED COMPONENTS system filesystem)
pkg_check_modules(JSONCPP jsoncpp)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)

# 查找 redis-plus-plus 和 hiredis
find_library(REDIS_PLUS_PLUS redis++ HINTS /usr/local/lib)
find_library(HIREDIS hiredis HINTS /usr/local/lib)

# 包含目录
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    /usr/local/include  # redis-plus-plus 头文件路径
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 添加可执行文件 - 包含所有源文件
add_executable(GateServer 
    GateServer.cpp
    HttpConnection.cpp
    LogicSystem.cpp
    ConfigMgr.cpp
    CServer.cpp
    AsioIOServicePool.cpp
    VerifyGrpcClient.cpp
    GrpcConnectionPool.cpp
    RedisMgr.cpp
    RedisConPool.cpp
    message.pb.cc
    message.grpc.pb.cc
     
)

# 链接库 - 使用 pkg-config 提供的库信息并添加 redis-plus-plus 和 hiredis
target_link_libraries(GateServer
    ${GRPC_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${Boost_LIBRARIES}
    ${JSONCPP_LIBRARIES}
    ${REDIS_PLUS_PLUS}  # 链接 redis-plus-plus
    ${HIREDIS}          # 链接 hiredis
    pthread
    dl
    z
)

# 添加编译标志
target_compile_options(GateServer PRIVATE 
    ${GRPC_CFLAGS_OTHER}
    ${PROTOBUF_CFLAGS_OTHER}
)

# 添加链接目录
target_link_directories(GateServer PRIVATE 
    ${GRPC_LIBRARY_DIRS}
    ${PROTOBUF_LIBRARY_DIRS}
    /usr/local/lib  # redis-plus-plus 和 hiredis 库路径
)

# 复制配置文件
configure_file(${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/config.ini COPYONLY)