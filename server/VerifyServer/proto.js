// 1. 引入需要的库
const path = require('path')           // 处理文件路径
const grpc = require('@grpc/grpc-js')  // gRPC 核心库
const protoLoader = require('@grpc/proto-loader')  // 加载 .proto 文件

// 2. 找到 .proto 文件
const PROTO_PATH = path.join(__dirname, 'message.proto')

// 3. 把 .proto 文件内容读取并转换
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {/*一堆配置*/})
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

// 4. 导出可以用的服务
const message_proto = protoDescriptor.message
module.exports = message_proto