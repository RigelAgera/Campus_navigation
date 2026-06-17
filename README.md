# Campus Navigation

一个基于 **C++17** 实现的校园导航课程项目，使用**邻接表图结构**完成校园地点与道路管理、路径查询以及多种图算法分析。

项目提供 **CLI 批处理入口**，可通过标准输入读取命令、标准输出写出结果，适合课程测试与助教批量评测。

## 项目功能

当前项目已实现以下能力：

- **数据维护**
  - 从 CSV 文件加载地点与道路数据
  - 将当前图保存回 CSV 文件
  - 新增、删除、修改地点
  - 新增、删除、修改道路
  - 打开 / 关闭道路
- **查询功能**
  - 查询单个地点完整信息
  - 按类别查询地点
  - 查询地点的邻接道路
- **图算法**
  - 连通分量分析 `COMPONENTS`
  - 双模式最短路径 `SHORTEST <from> <to> <DIST|TIME>`
  - 时刻约束最短路径 `TIMED_SHORTEST <from> <to> <time> <DIST|TIME>`
  - 必经点路径规划 `MUST_PASS <from> <to> <DIST|TIME> <k> <p1> ... <pk>`
  - 最小生成树 `MST`
  - 关键节点与关键边分析 `CRITICAL`
  - 拓展功能：分层图最短路径 `SHORTEST_K <from> <to> <k>`

## 项目结构

```text
Campus_Navigation/
├─ CMakeLists.txt              # CMake 构建配置，使用 C++17
├─ main.cpp                    # 程序入口
├─ CommandProcessor.h/.cpp     # 命令解析与分发
├─ LGraph.h/.cpp               # 图结构与图操作
├─ Algorithm.h/.cpp            # 图算法实现
├─ CsvIO.h/.cpp                # CSV 读写
├─ LocationInfo.h/.cpp         # 地点/道路数据结构
├─ GraphException.h            # 异常定义
├─ Coursework_brief.md         # 课程项目说明
├─ Report/                     # 实验报告与图片
└─ test_data/                  # 测试数据
```

## 开发环境

- 操作系统：Windows 11
- 终端：**PowerShell**
- 编译器：支持 C++17 的编译器
  - 推荐：MinGW g++、MSYS2 g++、Clang++ 或 Visual Studio 的 MSVC
- 构建工具：`cmake`（推荐 3.16 及以上）

## 编译方式

### 方式一：使用 CMake（推荐）

在项目根目录 `d:\Campus_Navigation` 下打开 PowerShell，执行：

```powershell
cmake -S . -B build
cmake --build build --config Release
```

说明：

- `cmake -S . -B build`：在 `build/` 目录生成构建文件
- `cmake --build build --config Release`：编译项目

编译成功后，可执行文件通常位于以下位置之一：

- `build/CampusNavigation.exe`
- `build/Release/CampusNavigation.exe`

若你使用的是 Visual Studio 生成器，通常会在 `build/Release/` 下；
若你使用的是 MinGW/Ninja/NMake 等生成器，通常会在 `build/` 下。

### 方式二：直接使用 g++

//unimportant

如果你的环境里已经安装了 `g++`，也可以直接编译：

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -o CampusNavigation.exe `
    main.cpp LGraph.cpp LocationInfo.cpp CsvIO.cpp Algorithm.cpp CommandProcessor.cpp
```

如果 PowerShell 中反斜杠换行不方便，也可以写成单行：

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -o CampusNavigation.exe main.cpp LGraph.cpp LocationInfo.cpp CsvIO.cpp Algorithm.cpp CommandProcessor.cpp
```

## 运行方式

程序是一个**命令行批处理程序**，会从标准输入读取命令。

### 1. 交互式启动

```powershell
.\build\CampusNavigation.exe
```

或（如果可执行文件在 `Release` 目录）：

```powershell
.\build\Release\CampusNavigation.exe
```

启动后会显示命令菜单，你可以手动输入命令，例如：

```text
LOAD test_data/base/sample_ecnu/places.csv test_data/base/sample_ecnu/roads.csv
COMPONENTS
QUIT
```

### 2. 批处理运行（推荐测试方式）

根据课程要求，程序本质上支持“从标准输入读取命令、将结果写到标准输出”。

在 **PowerShell** 中，推荐这样运行：

```powershell
Get-Content .\command.txt | .\build\CampusNavigation.exe | Set-Content .\answer.txt
```

如果你的可执行文件在 `build\Release`：

```powershell
Get-Content .\command.txt | .\build\Release\CampusNavigation.exe | Set-Content .\answer.txt
```

课程文档中的等价写法通常记作：

```powershell
.\build\CampusNavigation.exe < command.txt > answer.txt
```

含义：

- 从 `command.txt` 读取一批命令
- 将程序输出写入 `answer.txt`

在 PowerShell 里的实际示例：

```powershell
Get-Content .\test_data\base\small_cases\case_01\command.txt | .\build\CampusNavigation.exe | Set-Content .\result_case_01.txt
```

如果你的可执行文件在 `build\Release`：

```powershell
Get-Content .\test_data\base\small_cases\case_01\command.txt | .\build\Release\CampusNavigation.exe | Set-Content .\result_case_01.txt
```

## 常用命令说明

项目的 CLI 支持以下主要命令：

### 数据加载与保存

```text
LOAD <places_file> <roads_file>
SAVE <places_out_file> <roads_out_file>
```

### 地点与道路维护

```text
ADD_PLACE <place_id> <display_name> <category> <stay_time> <open_time> <close_time>
DELETE_PLACE <place_id>
UPDATE_PLACE <place_id> <field> <value>

ADD_ROAD <from_id> <to_id> <distance> <walk_time> <status>
DELETE_ROAD <from_id> <to_id>
UPDATE_ROAD <from_id> <to_id> <field> <value>
CLOSE_ROAD <from_id> <to_id>
OPEN_ROAD <from_id> <to_id>
```

### 查询命令

```text
QUERY_PLACE <place_id>
QUERY_CATEGORY <category>
ADJ <place_id>
```

### 图算法命令

```text
COMPONENTS
SHORTEST <from_id> <to_id> <DIST|TIME>
TIMED_SHORTEST <from_id> <to_id> <time> <DIST|TIME>
MUST_PASS <from_id> <to_id> <DIST|TIME> <k> <p1> ... <pk>
MST
CRITICAL
SHORTEST_K <from_id> <to_id> <k>
QUIT
```

## 测试数据说明

项目自带测试数据目录：`test_data/`

主要分为：

- `test_data/base/`：基础必做功能测试
  - `small_cases/`：小规模功能测试
  - `medium_cases/`：中规模测试
  - `large_cases/`：大规模压力测试
  - `sample_ecnu/`：示例校园数据
- `test_data/expand/`：拓展功能测试

每个测试用例目录通常包含：

- `places.csv`
- `roads.csv`
- `command.txt`
- `answer.txt`

其中：

- `command.txt`：输入命令
- `answer.txt`：参考输出

> 注意：根据 `test_data/README.md` 的说明，`answer.txt` 主要用于语义核对，不一定要求和你的输出逐字符完全一致，但结果含义应正确。

## 如何测试

### 方法一：手动运行单个测试用例

先进入具体测试目录，例如：

```powershell
Get-ChildItem .\test_data\base\small_cases
```

然后执行：

```powershell
Get-Content .\test_data\base\small_cases\case_01\command.txt | .\build\CampusNavigation.exe | Set-Content .\case_01_out.txt
```

再查看输出：

```powershell
Get-Content .\case_01_out.txt
```

需要对照参考答案时，可查看：

```powershell
Get-Content .\test_data\base\small_cases\case_01\answer.txt
```

### 方法二：使用自定义命令文件测试

你也可以自己编写一个 `command.txt`，例如：

```text
LOAD test_data/base/sample_ecnu/places.csv test_data/base/sample_ecnu/roads.csv
COMPONENTS
SHORTEST P0001 P0002 DIST
MST
QUIT
```

然后运行：

```powershell
Get-Content .\command.txt | .\build\CampusNavigation.exe
```

### 方法三：保存输出后与参考结果人工对比

```powershell
Get-Content .\test_data\base\small_cases\case_02\command.txt | .\build\CampusNavigation.exe | Set-Content .\case_02_out.txt
```

然后分别打开：

```powershell
Get-Content .\case_02_out.txt
Get-Content .\test_data\base\small_cases\case_02\answer.txt
```

## CSV 数据格式

### `places.csv`

```csv
place_id,display_name,category,stay_time,open_time,close_time
P0001,Library,Teaching,30,08:00,22:00
P0002,Canteen,Dining,40,06:30,20:30
```

### `roads.csv`

```csv
from_id,to_id,distance,walk_time,status
P0001,P0002,180,3,open
P0001,P0003,240,4,closed
```

程序应兼容：

- 带表头的 CSV
- 不带表头的 CSV
- 含空行的 CSV

## 输出与注意事项

- 程序在 Windows 下会设置控制台为 UTF-8，以正确显示中文。
- 命令字与参数之间使用空格分隔。
- 若命令或参数错误，程序会输出 `ERROR ...`。
- `QUIT` 用于结束程序。
- 当前程序启动后会先打印菜单，并在每次命令后输出 `cmd> ` 提示符；在做批处理比对时，请留意这一点。

## 课程要求对应关系

本项目符合课程要求中的关键约束：

- 使用 **C++17** 编写
- 提供 **CLI 批处理入口**
- 支持从 CSV 读入 / 保存数据
- 实现图的动态维护与核心算法
- 可通过重定向方式进行批量测试

## 参考文件

- `Coursework_brief.md`：课程项目说明
- `test_data/README.md`：测试数据说明
- `Report/Report.md`：实验报告与设计分析

如果你要提交课程作业，建议将本文件与源代码、实验报告一起打包提交。