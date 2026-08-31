# -

东软电动汽车充电桩应用管理平台，随着新能源汽车产业的蓬勃发展，电动汽车保有量持续增长，充电基础设施的智能化管理已成为行业刚需。当前充电桩运营面临站点分布不均、用户找桩困难、设备利用率偏低、运营数据分散等痛点，亟需一套集用户服务、设备管控、数据分析于一体的综合管理平台。本项目融合 Linux 操作系统、Qt 跨平台开发、数据库技术与大数据可视化等核心课程知识，构建一个覆盖“用户端—服务端—数据端”全链条的完整业务系统。

## 当前实现

- **Qt + C++**：使用 `Qt6::Core`、`Qt6::Network`、`Qt6::Sql` 构建跨平台 C++ 应用。
- **QSQLite 数据层**：`ChargingRepository` 负责站点与充电会话的存储和查询。
- **Socket 服务层**：`SocketServer` 监听 TCP 端口，接收用户端命令并返回 JSON。
- **多线程框架**：
  - 服务对象 `ChargingService` 运行在独立 `QThread`。
  - 每个 socket 连接由独立工作线程处理。
- **用户端演示**：`main.cpp` 内置客户端请求，演示找桩与利用率查询。

## 命令协议

- `FIND_STATIONS <城市>`：查询城市内站点
- `START_SESSION <用户ID> <站点ID>`：开始充电会话
- `END_SESSION <会话ID>`：结束充电会话
- `UTILIZATION`：查询站点利用率指标

## 构建与运行

```bash
cmake -S . -B build
cmake --build build
./build/ev_charging_platform
```
