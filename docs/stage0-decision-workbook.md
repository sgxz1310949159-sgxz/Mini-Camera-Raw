# 阶段0 阶段零决策表

状态：Resolved

日期：2026-07-09

## 决策记录

以下决定已于 2026-07-09 确认；D6 排期于 2026-09-08 更新，将暂停后节点顺延 37 天：

- D1：GoogleTest。

- D2：静态核心库加 CLI。

- D3：混合依赖策略。

- D4：MIT 许可证，版权署名为 `上官仙泽`。

- D5：Sony A7C II（`ILCE-7CM2`）及本地 `.ARW` 样张。

- D6：每周约 28 小时；2026-07-14 正式开始；2026-07-18 至
  2026-09-08 暂停；2026-10-01 完成核心版本验收；扩展功能和作品整理
  安排在 2026-10-08 至 2026-11-06。

MIT 许可证文件可以使用 UTF-8 中文署名，因此不需要改用拼音。

## 已解决

- 语言：C++17。

- 构建系统：CMake。

- 命名空间：`mini_camera_raw`。

- 先完成正确的标量 CPU 实现，再考虑并行和 GPU。

- 公开项目包含核心库；个人学习内容仅本地保留。

- GitHub 仓库：`sgxz1310949159-sgxz/Mini-Camera-Raw`。

## 已评估的决策

### D1 测试框架

**A. GoogleTest（推荐）**

- 优点：C++ 工程中使用广泛，断言语义明确，与 CMake/CTest 集成成熟，后续需要时包含 GoogleMock。

- 缺点：比最轻量方案更有仪式感，依赖规模也更大。

**B. Catch2**

- 优点：测试用例和断言可读性好，与 CMake/CTest 集成良好。

- 缺点：在部分工业 C++ 项目中代表性不如 GoogleTest；版本 3 已不是旧式单头文件模式。

**C. Minimal custom test executable**

- 优点：没有第三方依赖。

- 缺点：诊断能力弱，很快需要替换，也不利于学习可迁移的测试实践。

推荐决定：`A`

### D2 阶段零 Target

**A. Static core library plus CLI（推荐）**

- `mini_camera_raw` 静态库负责图像类型和处理逻辑。

- `mini-camera-raw` CLI 链接核心库，提供端到端入口。

- 测试直接链接核心库。

**B. Core library only**

- 阶段零更小，但缺少可执行的 smoke test 路径。

**C. CLI only**

- 脚手架最快，但算法逻辑会与命令行职责耦合，后续需要重构。

推荐决定：`A`

### D3 依赖获取方式

**A. Hybrid policy（推荐）**

- 使用 CMake `FetchContent` 获取固定版本的 GoogleTest/Catch2。

- LibRaw 等较大的运行时依赖使用系统安装包。

- 可用时通过带命名空间的 CMake target 使用依赖。

- 不把第三方源码树复制进本仓库。

**B. vcpkg for all dependencies**

- 跨平台依赖管理更统一，但在 ISP 工作开始前就增加了第二套需要学习和维护的系统。

**C. System packages only**

- 本地简单，但测试框架版本和干净 checkout 的配置不够可复现。

推荐决定：`A`

### D4 许可证署名

推荐许可证：MIT

请选择准确的公开版权署名，例如：

```text
Copyright (c) 2026 sgxz1310949159-sgxz
```

或：

```text
Copyright (c) 2026 <your public name>
```

必须回答：希望公开显示的准确姓名或账号。

### D5 第一种相机与 RAW 样张

推荐来源优先级：

1. 本人拍摄并明确用于本公开学习项目的 RAW；除非单独确认，否则文件仍仅本地保留。

2. 有明确再分发许可并记录来源的公开 RAW 样张。

3. 阶段零始终使用合成 Bayer 数据测试，不受真实样张选择影响。

必须回答：

- 相机品牌与型号

- RAW 扩展名（如果知道）

- 是否已有适合且不涉及隐私的 RAW

如果已有样张，请在被 git 忽略的本地目录 `samples/raw/` 下准备两到三张未经修改的文件：

- 一张包含中性白色/灰色和多种颜色的日光场景

- 一张同时包含阴影和高光的高动态范围场景

- 可选：一张低照度场景，供后续研究噪声和裁剪

保持原始文件和元数据不变。避免人脸、地址、证件、位置敏感场景，以及不适合出现在截图中的内容。
除非之后单独确认公开，RAW 文件始终仅本地保留。

### D6 可投入时间

路线图必须符合真实暑期安排，请提供：

- 每周大约可投入多少小时

- 预计开始与结束日期

- 预计无法推进的时间段

- 是否有固定的作品展示截止时间

## 建议的工程默认值

以下是技术建议。如果不同意，再指出需要修改的项：

- CMake 最低版本：3.24。

- 本地默认构建生成器：安装 Ninja 后使用 Ninja，否则使用平台默认生成器。

- Bayer 源数据：拥有所有权、连续、行优先的 `uint16_t`。

- 工作图像：拥有所有权、连续、行优先的 `float`。

- 初始 RGB 布局：为保持简单，使用交错 RGB。

- 黑白电平定义名义上的 `[0, 1]` 区间，但工作值可以小于 `0` 或大于 `1`。
  白平衡、颜色转换和曝光阶段不得隐式裁剪，只有显式命名的阶段可以执行裁剪。

- 整数变换使用精确比较；浮点变换同时使用绝对和相对容差。

- 生成图像和大型 notebook 输出仅本地保留。

## 本地工具准备

初始环境已有 Apple Clang、Git 和 Homebrew。
CMake 4.3.4、Ninja 1.13.2、
pkg-config 2.5.1 与 LibRaw 0.22.1 已于 2026-07-09 安装。

现在需要：

```sh
brew install cmake ninja
```

阶段一接入 LibRaw 前需要：

```sh
brew install pkg-config libraw
```

安装操作只在用户确认后执行。上述软件包均在获得明确许可后安装。

## 资料依据

- GoogleTest 官方 CMake 快速入门使用 C++17、CMake、`FetchContent`、CTest 和
  `gtest_discover_tests`。

- Catch2 官方提供带命名空间的 CMake target 和 CTest 自动发现集成。

- CMake 官方将 `find_package()` 和 `FetchContent` 作为主要依赖集成方式。

- LibRaw 将源数据读取与可选的 dcraw 风格后处理区分开。

## 结果

以上决定已纳入正式阶段零规格、ADR、流水线契约、验证策略和实施计划。

相关资料与证据链接（保留原记录来源）：

- <https://google.github.io/googletest/quickstart-cmake.html>

- <https://catch2-temp.readthedocs.io/en/latest/cmake-integration.html>

- <https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html>

- <https://www.libraw.org/docs/API-overview.html>
