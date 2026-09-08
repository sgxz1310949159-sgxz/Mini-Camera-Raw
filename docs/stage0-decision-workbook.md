# Stage 0 Decision Workbook / 阶段零决策表

Status / 状态：Resolved

Date / 日期：2026-07-09

## Decision Record / 决策记录

The following decisions were accepted on 2026-07-09; D6 scheduling was updated on 2026-09-08 by shifting post-pause milestones by 37 days:

以下决定已于 2026-07-09 确认；D6 排期于 2026-09-08 更新，将暂停后节点顺延 37 天：

- D1: GoogleTest.
- D2: static core library plus CLI.
- D3: hybrid dependency policy.
- D4: MIT license, copyright name `上官仙泽`.
- D5: Sony A7C II (`ILCE-7CM2`) and local `.ARW` samples.
- D6: about 28 hours per week; formal start 2026-07-14; pause
  2026-07-18 through 2026-09-08; core acceptance deadline 2026-10-01;
  extension and portfolio work is scheduled for 2026-10-08 through 2026-11-06.

- D1：GoogleTest。
- D2：静态核心库加 CLI。
- D3：混合依赖策略。
- D4：MIT 许可证，版权署名为 `上官仙泽`。
- D5：Sony A7C II（`ILCE-7CM2`）及本地 `.ARW` 样张。
- D6：每周约 28 小时；2026-07-14 正式开始；2026-07-18 至
  2026-09-08 暂停；2026-10-01 完成核心版本验收；扩展功能和作品整理
  安排在 2026-10-08 至 2026-11-06。

Chinese is valid UTF-8 in the MIT license, so transliteration is unnecessary.

MIT 许可证文件可以使用 UTF-8 中文署名，因此不需要改用拼音。

## Already Resolved / 已解决

- Language: C++17.
- Build system: CMake.
- Namespace: `mini_camera_raw`.
- Correct scalar CPU implementation before parallel or GPU work.
- Public project contains a core library; personal learning work remains local.
- GitHub repository:
  `sgxz1310949159-sgxz/Mini-Camera-Raw`.

- 语言：C++17。
- 构建系统：CMake。
- 命名空间：`mini_camera_raw`。
- 先完成正确的标量 CPU 实现，再考虑并行和 GPU。
- 公开项目包含核心库；个人学习内容仅本地保留。
- GitHub 仓库：`sgxz1310949159-sgxz/Mini-Camera-Raw`。

## Considered Decisions / 已评估的决策

### D1. Test Framework / 测试框架

**A. GoogleTest (Recommended / 推荐)**

- Pros: widely used in C++ engineering, explicit assertions, good CMake and
  CTest integration, includes GoogleMock if later needed.
- Cons: more ceremony and a larger dependency than the smallest alternatives.

- 优点：C++ 工程中使用广泛，断言语义明确，与 CMake/CTest 集成成熟，后续需要时包含 GoogleMock。
- 缺点：比最轻量方案更有仪式感，依赖规模也更大。

**B. Catch2**

- Pros: readable test cases and assertions, good CMake/CTest integration.
- Cons: less representative of some industrial C++ codebases; version 3 is no
  longer the old single-header model.

- 优点：测试用例和断言可读性好，与 CMake/CTest 集成良好。
- 缺点：在部分工业 C++ 项目中代表性不如 GoogleTest；版本 3 已不是旧式单头文件模式。

**C. Minimal custom test executable**

- Pros: no dependency.
- Cons: weak diagnostics, soon needs replacement, and teaches less reusable
  testing practice.

- 优点：没有第三方依赖。
- 缺点：诊断能力弱，很快需要替换，也不利于学习可迁移的测试实践。

Recommended decision / 推荐决定：`A`

### D2. Stage 0 Targets / 阶段零 Target

**A. Static core library plus CLI (Recommended / 推荐)**

- `mini_camera_raw` static library owns image types and processing logic.
- `mini-camera-raw` CLI links to the library and provides an end-to-end entry.
- Tests link directly to the core library.

- `mini_camera_raw` 静态库负责图像类型和处理逻辑。
- `mini-camera-raw` CLI 链接核心库，提供端到端入口。
- 测试直接链接核心库。

**B. Core library only**

- Smaller Stage 0, but there is no executable smoke-test path.

- 阶段零更小，但缺少可执行的 smoke test 路径。

**C. CLI only**

- Fastest scaffold, but algorithm logic and command-line concerns will become
  coupled and need restructuring.

- 脚手架最快，但算法逻辑会与命令行职责耦合，后续需要重构。

Recommended decision / 推荐决定：`A`

### D3. Dependency Acquisition / 依赖获取方式

**A. Hybrid policy (Recommended / 推荐)**

- Fetch a pinned GoogleTest/Catch2 version with CMake `FetchContent`.
- Use installed packages for large runtime dependencies such as LibRaw.
- Consume dependencies through namespaced CMake targets when available.
- Do not vendor dependency source trees into this repository.

- 使用 CMake `FetchContent` 获取固定版本的 GoogleTest/Catch2。
- LibRaw 等较大的运行时依赖使用系统安装包。
- 可用时通过带命名空间的 CMake target 使用依赖。
- 不把第三方源码树复制进本仓库。

**B. vcpkg for all dependencies**

- More uniform cross-platform dependency management, but adds a second system
  to learn and maintain before the ISP work begins.

- 跨平台依赖管理更统一，但在 ISP 工作开始前就增加了第二套需要学习和维护的系统。

**C. System packages only**

- Simple locally, but test-framework versions and clean-checkout setup are less
  reproducible.

- 本地简单，但测试框架版本和干净 checkout 的配置不够可复现。

Recommended decision / 推荐决定：`A`

### D4. License Identity / 许可证署名

Recommended license / 推荐许可证：MIT

Choose the exact public copyright line. Examples:

请选择准确的公开版权署名，例如：

```text
Copyright (c) 2026 sgxz1310949159-sgxz
```

or / 或：

```text
Copyright (c) 2026 <your public name>
```

Required answer / 必须回答：the exact name or handle to publish.

必须回答：希望公开显示的准确姓名或账号。

### D5. First Camera and RAW Sample / 第一种相机与 RAW 样张

Preferred source order / 推荐来源优先级：

1. A self-shot RAW file intentionally selected for this public-learning
   project, kept local unless separately approved for publication.
2. A clearly redistributable public RAW sample with recorded license and source.
3. Synthetic Bayer data for Stage 0 tests, which will be used regardless.

1. 本人拍摄并明确用于本公开学习项目的 RAW；除非单独确认，否则文件仍仅本地保留。
2. 有明确再分发许可并记录来源的公开 RAW 样张。
3. 阶段零始终使用合成 Bayer 数据测试，不受真实样张选择影响。

Required answer / 必须回答：

- camera brand and model / 相机品牌与型号
- RAW extension, if known / RAW 扩展名（如果知道）
- whether suitable non-private RAW files already exist / 是否已有适合且不涉及隐私的 RAW

If available, prepare two or three untouched files under the ignored local
directory `samples/raw/`:

如果已有样张，请在被 git 忽略的本地目录 `samples/raw/` 下准备两到三张未经修改的文件：

- a daylight scene containing neutral white/gray and several colors
- a high-dynamic-range scene with both shadows and highlights
- optionally, a low-light scene for later noise and clipping study

- 一张包含中性白色/灰色和多种颜色的日光场景
- 一张同时包含阴影和高光的高动态范围场景
- 可选：一张低照度场景，供后续研究噪声和裁剪

Keep the original files and metadata unchanged. Avoid faces, addresses,
documents, location-sensitive scenes, or anything unsuitable for possible
screenshots. The RAW files remain local unless publication is approved
separately.

保持原始文件和元数据不变。避免人脸、地址、证件、位置敏感场景，以及不适合出现在截图中的内容。
除非之后单独确认公开，RAW 文件始终仅本地保留。

### D6. Available Project Time / 可投入时间

The roadmap must fit the real summer schedule. Provide:

路线图必须符合真实暑期安排，请提供：

- approximate hours per week / 每周大约可投入多少小时
- expected start and end dates / 预计开始与结束日期
- periods when the project must pause / 预计无法推进的时间段
- whether a portfolio demo has a fixed deadline / 是否有固定的作品展示截止时间

## Proposed Engineering Defaults / 建议的工程默认值

These are technical proposals. Reply with changes only if you disagree:

以下是技术建议。如果不同意，再指出需要修改的项：

- CMake minimum version: 3.24.
- Default local build generator: Ninja when installed; otherwise the platform
  default generator.
- Source Bayer storage: owned, contiguous, row-major `uint16_t`.
- Working image storage: owned, contiguous, row-major `float`.
- Initial RGB layout: interleaved RGB for simplicity.
- Numeric convention after black-level normalization: black and white
  references define the nominal `[0, 1]` interval, but working values may be
  below `0` or above `1`. White balance, color conversion, and exposure must
  not clip implicitly. Clipping is allowed only in an explicitly named stage.
- Exact comparisons for integer transforms; absolute and relative tolerances
  for floating-point transforms.
- Generated images and large notebook outputs remain local.

- CMake 最低版本：3.24。
- 本地默认构建生成器：安装 Ninja 后使用 Ninja，否则使用平台默认生成器。
- Bayer 源数据：拥有所有权、连续、行优先的 `uint16_t`。
- 工作图像：拥有所有权、连续、行优先的 `float`。
- 初始 RGB 布局：为保持简单，使用交错 RGB。
- 黑白电平定义名义上的 `[0, 1]` 区间，但工作值可以小于 `0` 或大于 `1`。
  白平衡、颜色转换和曝光阶段不得隐式裁剪，只有显式命名的阶段可以执行裁剪。
- 整数变换使用精确比较；浮点变换同时使用绝对和相对容差。
- 生成图像和大型 notebook 输出仅本地保留。

## Local Tool Preparation / 本地工具准备

The machine baseline initially contained Apple Clang, Git, and Homebrew.
CMake 4.3.4, Ninja 1.13.2, pkg-config 2.5.1, and LibRaw 0.22.1 were installed
on 2026-07-09.

初始环境已有 Apple Clang、Git 和 Homebrew。CMake 4.3.4、Ninja 1.13.2、
pkg-config 2.5.1 与 LibRaw 0.22.1 已于 2026-07-09 安装。

Required now / 现在需要：

```sh
brew install cmake ninja
```

Required before LibRaw integration in Stage 1 / 阶段一接入 LibRaw 前需要：

```sh
brew install pkg-config libraw
```

Installation should happen only after user approval. All packages listed above
were installed with explicit approval.

安装操作只在用户确认后执行。上述软件包均在获得明确许可后安装。

## Source Basis / 资料依据

- GoogleTest's official CMake quickstart uses C++17, CMake, `FetchContent`,
  CTest, and `gtest_discover_tests`:
  <https://google.github.io/googletest/quickstart-cmake.html>
- Catch2 officially exports namespaced CMake targets and provides CTest
  discovery integration:
  <https://catch2-temp.readthedocs.io/en/latest/cmake-integration.html>
- CMake documents `find_package()` and `FetchContent` as its primary dependency
  integration methods:
  <https://cmake.org/cmake/help/latest/guide/using-dependencies/index.html>
- LibRaw separates source-data loading from optional dcraw-style
  postprocessing:
  <https://www.libraw.org/docs/API-overview.html>

- GoogleTest 官方 CMake 快速入门使用 C++17、CMake、`FetchContent`、CTest 和
  `gtest_discover_tests`。
- Catch2 官方提供带命名空间的 CMake target 和 CTest 自动发现集成。
- CMake 官方将 `find_package()` 和 `FetchContent` 作为主要依赖集成方式。
- LibRaw 将源数据读取与可选的 dcraw 风格后处理区分开。

## Result / 结果

These decisions are incorporated into the approved Stage 0 specification,
ADRs, pipeline contract, validation policy, and implementation plan.

以上决定已纳入正式阶段零规格、ADR、流水线契约、验证策略和实施计划。
