cmake_minimum_required(VERSION 3.10)
project(StatusServer)

set(CMAKE_CXX_STANDARD 14)  # 使用C++14，避免MySQL C++ Connector兼容性问题
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

# 使用 pkg-config 查找库
find_package(PkgConfig REQUIRED)

# 查找所需的库
find_package(Boost REQUIRED COMPONENTS system filesystem)
pkg_check_modules(JSONCPP jsoncpp)
pkg_check_modules(PROTOBUF REQUIRED protobuf)
pkg_check_modules(GRPC REQUIRED grpc++)

# 查找 hiredis
find_library(HIREDIS hiredis HINTS /usr/lib/x86_64-linux-gnu /usr/local/lib)

# 查找 MySQL C++ Connector
find_library(MYSQLCPPCONN mysqlcppconn HINTS /usr/lib/x86_64-linux-gnu /usr/local/lib64)

# 查找 JsonCpp 库 - 使用正确的路径
find_library(JSONCPP_LIB jsoncpp HINTS /usr/local/lib /usr/lib/x86_64-linux-gnu)

# 打印调试信息
message(STATUS "HIREDIS: ${HIREDIS}")
message(STATUS "MYSQLCPPCONN: ${MYSQLCPPCONN}")
message(STATUS "JSONCPP_LIB: ${JSONCPP_LIB}")
message(STATUS "JSONCPP_LIBRARIES: ${JSONCPP_LIBRARIES}")
message(STATUS "PROTOBUF_LIBRARIES: ${PROTOBUF_LIBRARIES}")
message(STATUS "GRPC_LIBRARIES: ${GRPC_LIBRARIES}")

# 包含目录
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
    /usr/local/include
    /usr/include/hiredis
    /usr/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 添加可执行文件
add_executable(StatusServer
    ConfigMgr.cpp
    AsioIOServicePool.cpp
    RedisMgr.cpp
    RedisConPool.cpp
    Defer.cpp
    SqlConnection.cpp
    MysqlMgr.cpp
    MysqlDao.cpp
    MysqlConPool.cpp
    StatusServer.cpp
    StatusServiceImpl.cpp
    ChatServer.cpp
    ChatConPool.cpp
    ChatGrpcClient.cpp
    message.pb.cc
    message.grpc.pb.cc
)

# 链接库 - 使用 pkg-config 提供的完整链接信息
target_link_libraries(StatusServer
    ${JSONCPP_LIB}
    ${GRPC_LIBRARIES}
    ${PROTOBUF_LIBRARIES}
    ${Boost_LIBRARIES}
    ${HIREDIS}
    ${MYSQLCPPCONN}
    pthread
    dl
    z
    gpr
    protoc
)

# 添加编译标志
target_compile_options(StatusServer PRIVATE
    ${GRPC_CFLAGS_OTHER}
    ${PROTOBUF_CFLAGS_OTHER}
    -fpermissive
    -Wno-deprecated-declarations
    -Wno-noexcept
    -D_GLIBCXX_USE_CXX11_ABI=1
)

# 添加链接目录
target_link_directories(StatusServer PRIVATE
    ${GRPC_LIBRARY_DIRS}
    ${PROTOBUF_LIBRARY_DIRS}
    /usr/local/lib
    /usr/local/lib64
    /usr/lib/x86_64-linux-gnu
)

# 复制配置文件
configure_file(${CMAKE_SOURCE_DIR}/config.ini ${CMAKE_BINARY_DIR}/config.ini COPYONLY)