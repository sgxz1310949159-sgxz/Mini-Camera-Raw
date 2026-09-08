# Stage 0 Engineering Spec / 阶段零工程规格

Status / 状态：Approved v1.1

Date / 日期：2026-07-16

## Summary / 摘要

Stage 0 creates the engineering base for Mini-Camera Raw. It should not try to
implement the full ISP pipeline yet. Its job is to make later algorithm work
organized, testable, and easy to explain.

阶段零负责为 Mini-Camera Raw 创建工程基础。它暂时不应该尝试实现完整 ISP 流水线，而是要让后续算法工作变得有组织、可测试、易解释。

Stage 0 is complete when the repository has a clear structure, a build/test
baseline, an initial image data model, and written rules for what belongs in
the public project versus local learning notes.

当仓库具备清晰结构、构建/测试基线、初始图像数据模型，以及公开项目和本地学习笔记的边界规则时，阶段零才算完成。

## Scope / 范围

In scope:

阶段零包含：

- initialize the local repository
- define the public project layout
- keep personal learning notes outside git
- choose initial C++ and CMake conventions
- define the first image-buffer concepts
- prepare a minimal test strategy
- document Stage 1 entry criteria

- 初始化本地仓库
- 定义公开项目布局
- 将个人学习笔记排除在 git 之外
- 选择初始 C++ 与 CMake 约定
- 定义第一版图像缓冲区概念
- 准备最小测试策略
- 记录进入阶段一的条件

Out of scope:

阶段零不包含：

- full RAW decoding
- real demosaicing
- OpenCV UI
- OpenMP optimization
- Metal compute
- dynamic library packaging

- 完整 RAW 解码
- 真实去马赛克算法
- OpenCV UI
- OpenMP 优化
- Metal Compute
- 动态库打包

## Baseline Project Layout / 基础项目布局

```text
include/mini_camera_raw/   Public headers for the future core library / 未来核心库的公开头文件
src/                       Core C++ implementation / 核心 C++ 实现
apps/                      CLI tools and temporary validation apps / CLI 工具和临时验证程序
tests/                     Unit tests and small regression tests / 单元测试和小型回归测试
benchmarks/                Performance experiments and benchmark source / 性能实验和 benchmark 源码
cmake/                     Local CMake helper modules / 本地 CMake 辅助模块
docs/                      Public documentation and specs / 公开文档和规格
samples/                   Sample policy and optional tiny fixtures / 样例策略和可选小型测试数据
learning/                  Local-only study notes, ignored by git / 本地学习笔记，已被 git 忽略
```

## Initial Technical Decisions / 初始技术决策

- Language standard: C++17 for broad compiler and dependency compatibility.
- Build system: CMake 3.24 or newer; Ninja is the recommended local generator.
- Namespace: `mini_camera_raw`.
- Targets: a static `mini_camera_raw` core library, a `mini-camera-raw` CLI,
  and GoogleTest test executables registered with CTest.
- Dependency policy: fetch a pinned GoogleTest with CMake `FetchContent`; use
  installed packages for large runtime dependencies such as LibRaw.
- Core code must not depend on OpenCV UI types.
- CPU serial correctness comes before multithreading or GPU acceleration.
- Image-processing functions should document their expected color space, bit
  depth, and numeric range.
- Real camera RAW parsing will use LibRaw later, but Stage 0 can use synthetic
  buffers for tests.
- License: MIT, copyright `上官仙泽`.

- 语言标准：C++17，兼顾编译器和依赖兼容性。
- 构建系统：CMake 3.24 或更高版本；本地推荐使用 Ninja 生成器。
- 命名空间：`mini_camera_raw`。
- Target：`mini_camera_raw` 静态核心库、`mini-camera-raw` CLI，以及通过
  CTest 注册的 GoogleTest 测试程序。
- 依赖策略：使用 CMake `FetchContent` 获取固定版本的 GoogleTest；LibRaw
  等大型运行时依赖使用系统安装包。
- 核心代码不能依赖 OpenCV UI 类型。
- 先保证 CPU 串行版本正确，再考虑多线程或 GPU 加速。
- 图像处理函数应该记录其预期色彩空间、位深和数值范围。
- 真实相机 RAW 解析后续使用 LibRaw，但阶段零可以用合成 buffer 做测试。
- 许可证：MIT，版权署名为 `上官仙泽`。

## Supported RAW Baseline / RAW 支持基线

The first real-camera path targets Sony A7C II (`ILCE-7CM2`) `.ARW` files.
Three local samples have been confirmed by file headers. They remain ignored
by git and are not approved for redistribution. Stage 0 tests use generated
synthetic Bayer fixtures, so a public checkout does not depend on private RAW
files.

第一条真实相机处理路径以 Sony A7C II（`ILCE-7CM2`）的 `.ARW` 文件为目标。
目前已通过文件头确认 3 个本地样张。样张继续被 git 忽略，且未获准再分发。
阶段零测试使用生成的合成 Bayer fixture，因此公开 checkout 不依赖私有 RAW 文件。

## Data Model Direction / 数据模型方向

Stage 0 should define the minimum concepts needed before Stage 1:

阶段零应该在进入阶段一之前定义最小必要概念：

- image width and height
- channel count
- pixel format, for example `uint16` source data or `float32` working data
- color state, for example Bayer, linear RGB, or display RGB
- row stride or a documented contiguous layout
- owned storage using standard C++ containers for the first version

- 图像宽度和高度
- 通道数
- 像素格式，例如 `uint16` 源数据或 `float32` 工作数据
- 色彩状态，例如 Bayer、线性 RGB 或显示 RGB
- 行跨度，或明确记录的连续内存布局
- 第一版使用标准 C++ 容器持有数据所有权

The first implementation should prefer clarity:

第一版实现应该优先追求清晰：

- use owned buffers before designing external memory views
- avoid implicit conversion between linear and nonlinear spaces
- keep metadata explicit rather than hidden in comments
- treat black and white references as the nominal `[0, 1]` interval, not as
  a permanent storage clamp
- permit out-of-range working values when the stage contract allows them;
  clipping must be an explicit operation

- 先使用拥有所有权的 buffer，再设计外部内存视图
- 避免在线性空间和非线性空间之间做隐式转换
- 让元数据显式存在，而不是藏在注释里
- 把黑白电平视为名义 `[0, 1]` 区间的参考，而不是永久存储裁剪边界
- 阶段契约允许时保留超范围工作值；裁剪必须是显式操作

### P2 Acceptance Contract / P2 验收契约

The initial owned image model is accepted only when all of the following are
true:

第一版 owned 图像模型只有在以下条件全部满足时才通过验收：

- public metadata and buffer types implement the combinations and error
  semantics recorded in ADR-002
- zero dimensions, invalid channel/stride/storage/state combinations, and all
  relevant `size_t` multiplication overflows are rejected before allocation or
  access
- deterministic tests cover one-pixel, odd-size, ordinary Bayer, padded-row,
  and float working buffers, including const and mutable access
- copy behavior owns independent storage and move behavior transfers the
  allocation without adding a custom ownership abstraction
- no operation performs color conversion, normalization, clipping, or a
  per-sample finite-value scan
- a Ninja clean configure/build and full CTest run pass, and a separate clean
  `BUILD_TESTING=OFF` configure/build also passes

- 公开 metadata 与 buffer 类型实现 ADR-002 中记录的合法组合和错误语义
- 零尺寸、非法通道/stride/storage/状态组合，以及所有相关 `size_t` 乘法
  溢出都必须在分配或访问前被拒绝
- 确定性测试覆盖单像素、奇数尺寸、普通 Bayer、带行 padding 和 float 工作
  buffer，并包含 const 与可变访问
- copy 行为持有独立 storage，move 行为转移 allocation，且不增加自定义所有权
  抽象
- 不执行色彩转换、归一化、裁剪或逐样本有限值扫描
- 使用 Ninja 的干净配置、构建和完整 CTest 通过；另一个干净的
  `BUILD_TESTING=OFF` 配置与构建也通过

## Schedule and Scope Guard / 时间与范围约束

- Formal project start: 2026-07-14.
- Planned pause: 2026-07-18 through 2026-09-08, inclusive.
- Available effort: about 28 hours per active week.
- Core acceptance deadline: 2026-10-01.
- Extension and portfolio polish window: 2026-10-08 through 2026-11-06.

- 正式开始：2026-07-14。
- 计划暂停：2026-07-18 至 2026-09-08，含首尾日期。
- 可投入时间：有效周每周约 28 小时。
- 核心版本验收截止：2026-10-01。
- 扩展功能和作品整理：2026-10-08 至 2026-11-06。

Because the pause leaves roughly three active development weeks before the
fixed deadline, the September–October acceptance baseline prioritizes a correct,
explainable CPU pipeline over breadth. UI, Metal, general camera support,
advanced local adjustments, and aggressive optimization are 2026-10-08 to 2026-11-06 or
optional work unless earlier checkpoints finish with verified margin.

由于暂停期后到固定截止日期前大约只剩 3 个有效开发周，9—10 月验收基线必须优先保证
CPU 流水线正确、可解释，而不是追求功能数量。UI、Metal、通用相机支持、高级局部调整
和激进性能优化属于 2026-10-08 至 2026-11-06扩展或可选内容，除非前序检查点提前完成并留下经过验证的余量。

## Stage 0 Deliverables / 阶段零交付物

- `README.md` with project purpose and directory map.
- `.gitignore` that excludes local learning notes, RAW files, build products,
  and the original planning document.
- public docs for project foundation and repository boundary.
- initial CMake skeleton.
- minimal placeholder library or executable that builds.
- a first unit-test target using synthetic data.
- an initial `ImageBuffer` or equivalent design note/header.
- accepted ADRs, pipeline contract, validation policy, and task plan.

- `README.md`：说明项目目的和目录结构。
- `.gitignore`：排除本地学习笔记、RAW 文件、构建产物和原始策划书。
- 项目基础说明和仓库边界说明等公开文档。
- 初始 CMake 骨架。
- 能构建的最小占位库或可执行程序。
- 使用合成数据的第一批单元测试目标。
- 初始 `ImageBuffer` 或等价设计说明/头文件。
- 已确认的 ADR、流水线契约、验证策略和任务计划。

## Acceptance Criteria / 验收标准

Stage 0 is done when:

阶段零完成标准：

- the project builds from a clean checkout using CMake
- a basic test command runs successfully
- no local learning notes or RAW files appear in normal `git status`
- the repository has a clear public/private content boundary
- Stage 1 can start without redesigning the folder structure
- public documentation contains the accepted decisions and no unresolved
  Stage 0 blocker

- 从干净 checkout 中可以用 CMake 构建项目
- 基础测试命令能成功运行
- 正常 `git status` 中不会出现本地学习笔记或 RAW 文件
- 仓库有清晰的公开/私有内容边界
- 阶段一可以在不重做目录结构的前提下开始
- 公开文档记录了已确认决定，且不存在未解决的阶段零阻塞项

Suggested future commands:

建议后续命令：

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Learning Checklist / 学习检查清单

Before moving deeply into Stage 1, study and summarize:

深入阶段一之前，先学习并总结：

- RAW versus JPEG
- Bayer/CFA sampling
- bit depth, black level, white level, and clipping
- image memory layout and row-major traversal
- linear light versus display-referred RGB
- CMake targets, include directories, and test targets

- RAW 与 JPEG 的区别
- Bayer/CFA 采样
- 位深、黑电平、白电平和裁剪
- 图像内存布局和行优先遍历
- 线性光与显示参考 RGB 的区别
- CMake target、include 目录和测试 target

Personal notes for these topics should go under `learning/` first. Polished
summaries can later be moved into `docs/` if they become useful public material.

这些主题的个人笔记应该先放在 `learning/` 下。等内容整理成熟、适合公开展示后，再重写为 `docs/` 下的正式文档。

## Decision Status / 决策状态

All Stage 0 blocking decisions are resolved. The considered options and final
answers remain recorded in `docs/stage0-decision-workbook.md`.

所有阶段零阻塞决定均已解决。备选方案和最终答案保留在
`docs/stage0-decision-workbook.md` 中。
