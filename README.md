
# chat-project

一个基于 `Qt + C++` 客户端与多服务端协作的即时通讯项目。  
当前代码实现了注册、邮箱验证码、登录分配、TCP 长连接登录、用户搜索等主链路，并预留了好友申请和聊天消息的扩展协议。

## 项目结构

```text
chat-project
├─ client/                 # Qt 客户端（UI + HTTP + TCP）
└─ server/
   ├─ VerifyServer/        # Node.js gRPC：邮箱验证码服务
   ├─ GateServer/          # C++ HTTP 网关：注册/重置/登录入口
   ├─ StatusServer/        # C++ gRPC：分配 ChatServer + 生成 token
   ├─ ChatServer/          # C++ TCP + gRPC：长连接与业务处理
   └─ ChatServer2/         # 第二个 ChatServer 节点（用于负载分配）
```

## 架构与调用链路

1. 客户端通过 HTTP 调用 `GateServer`：
   - `/get_varifycode`
   - `/user_register`
   - `/reset_pwd`
   - `/user_login`
2. `GateServer` 调用 `VerifyServer` 获取验证码/发送邮件。
3. 登录时 `GateServer` 调用 `StatusServer` 获取可用 `ChatServer` 与 `token`。
4. 客户端拿到 `host/port/token` 后建立 TCP 长连接到 `ChatServer`。
5. `ChatServer` 基于 Redis 中保存的 token 完成登录校验。

## 主要功能（当前代码状态）

- 已实现
  - 邮箱验证码获取（gRPC + 邮件发送）
  - 用户注册
  - 重置密码
  - 用户登录（HTTP -> 状态服务分配 -> TCP 登录）
  - 用户搜索（TCP 消息）
- 进行中/预留
  - 好友申请、认证、文本消息等消息号已定义，部分流程仍在开发中

## 技术栈

- 客户端：`Qt Widgets`、`QNetworkAccessManager`、`QTcpSocket`
- 网关/状态/聊天服务：`C++14`、`Boost.Asio/Beast`、`gRPC`、`protobuf`、`jsoncpp`
- 存储：`MySQL`、`Redis`
- 验证码服务：`Node.js`、`@grpc/grpc-js`、`nodemailer`、`ioredis`

## 环境依赖

### 1) 服务端（Linux/WSL 建议）

- `cmake >= 3.10`
- `g++`（支持 C++14）
- `boost`（system/filesystem）
- `jsoncpp`
- `protobuf` + `grpc`
- `hiredis`
- `mysql-connector-c++`
- `mysql-server`
- `redis-server`

可参考（Ubuntu）：

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config \
  libboost-system-dev libboost-filesystem-dev \
  libjsoncpp-dev libprotobuf-dev protobuf-compiler \
  libgrpc++-dev grpc-proto \
  libhiredis-dev libmysqlcppconn-dev \
  mysql-server redis-server
```

### 2) 验证码服务

- `Node.js 18+`

### 3) 客户端

- `Qt 6.x`（项目中有 `Qt_6_7` 数据流版本）
- `qmake` 或 Qt Creator

## 配置说明

### 1) 客户端配置

文件：`client/config.ini`

```ini
[GateServer]
host=你的网关地址
port=8080
```

客户端启动时会拼接：
`gate_url_prefix = http://{host}:{port}`

### 2) 服务端配置

文件：

- `server/GateServer/config.ini`
- `server/StatusServer/config.ini`
- `server/ChatServer/config.ini`
- `server/ChatServer2/config.ini`

需要统一以下配置项：

- `VarifyServer` 地址与端口（默认 50051）
- `StatusServer` 地址与端口（默认 50052）
- `ChatServer/ChatServer2` 对外 TCP 端口与 RPC 端口
- `Redis`、`Mysql` 连接信息

### 3) VerifyServer 配置

文件：`server/VerifyServer/config.json`

- `email.user / email.pass`：发件邮箱与授权码
- `redis`：验证码缓存
- `mysql`：当前代码主要使用 Redis+邮件，MySQL 字段保留

注意：请使用你自己的邮箱与授权码，不要提交真实凭据到公开仓库。

## 数据库准备

代码依赖：

- 数据库：`llfc`（可在 `config.ini` 中修改）
- 表：`user`（至少包含 `uid/name/email/pwd`）
- 存储过程：`reg_user(?,?,?,@result)`（注册流程调用）

仓库内未提供完整 SQL 初始化脚本，需根据上述约束自行建表与存储过程。

## 构建与运行

### 1) 启动基础服务

先启动 MySQL、Redis。

### 2) 启动 VerifyServer（Node.js）

```bash
cd server/VerifyServer
npm install
npm run server
```

默认监听：`0.0.0.0:50051`（gRPC）

### 3) 构建并启动 C++ 服务

分别在每个服务目录执行：

```bash
cd server/GateServer
mkdir -p build && cd build
cmake ..
make -j
./GateServer
```

```bash
cd server/StatusServer
mkdir -p build && cd build
cmake ..
make -j
./StatusServer
```

```bash
cd server/ChatServer
mkdir -p build && cd build
cmake ..
make -j
./ChatServer
```

```bash
cd server/ChatServer2
mkdir -p build && cd build
cmake ..
make -j
./ChatServer2
```

启动顺序建议：

1. VerifyServer
2. StatusServer
3. ChatServer / ChatServer2
4. GateServer
5. client

### 4) 启动客户端

使用 Qt Creator 打开 `client/YQchat.pro`，编译并运行。

## HTTP 接口（GateServer）

- `POST /get_varifycode`
  - 请求：`{ "email": "xxx@qq.com" }`
  - 返回：`{ "error": 0, "email": "xxx@qq.com" }`
- `POST /user_register`
  - 请求：`{ "user","email","passwd","confirm","verifycode" }`
- `POST /reset_pwd`
  - 请求：`{ "user","email","passwd","verifycode" }`
- `POST /user_login`
  - 请求：`{ "user","passwd" }`
  - 返回含：`uid`、`host`、`port`、`token`

## TCP 协议（客户端 <-> ChatServer）

报文头固定 4 字节（大端）：

- `uint16 msg_id`
- `uint16 body_len`

后接 `JSON body`。

已使用/定义消息号示例：

- `1005` 登录聊天服请求
- `1006` 登录聊天服响应
- `1007/1008` 用户搜索请求/响应
- `1009+` 好友申请、认证、聊天消息（部分待完善）

## 未完成问题

1. `client/YQchat.pro` 中存在缺失文件引用（如 `friendlabel.*`、`picturebubbble.*`），直接构建可能失败，需要先修正工程文件或补齐源码。
2. 好友申请/聊天消息链路在 UI 与服务端中有不少注释/预留代码，当前并非完整闭环。
3. 部分配置默认写死为局域网 IP（如 `192.168.x.x`），部署前必须统一修改。
4. `VerifyServer/config.json` 出现明文凭据示例，建议改为环境变量读取。



1. 补充数据库初始化 SQL（建表 + 存储过程）并纳入仓库。
2. 完成好友申请、好友认证、文本消息完整闭环与离线消息。
3. 增加 Docker Compose（MySQL/Redis/各服务）一键启动。
4. 加入最小化联调测试脚本（HTTP + TCP smoke test）。

