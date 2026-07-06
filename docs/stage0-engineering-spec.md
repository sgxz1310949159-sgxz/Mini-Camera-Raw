# Stage 0 Engineering Spec / 阶段零工程规格

Status / 状态：Draft v0.1  
Date / 日期：2026-07-06

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
- Build system: CMake.
- Namespace: `mini_camera_raw`.
- Core code must not depend on OpenCV UI types.
- CPU serial correctness comes before multithreading or GPU acceleration.
- Image-processing functions should document their expected color space, bit
  depth, and numeric range.
- Real camera RAW parsing will use LibRaw later, but Stage 0 can use synthetic
  buffers for tests.

- 语言标准：C++17，兼顾编译器和依赖兼容性。
- 构建系统：CMake。
- 命名空间：`mini_camera_raw`。
- 核心代码不能依赖 OpenCV UI 类型。
- 先保证 CPU 串行版本正确，再考虑多线程或 GPU 加速。
- 图像处理函数应该记录其预期色彩空间、位深和数值范围。
- 真实相机 RAW 解析后续使用 LibRaw，但阶段零可以用合成 buffer 做测试。

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

- 先使用拥有所有权的 buffer，再设计外部内存视图
- 避免在线性空间和非线性空间之间做隐式转换
- 让元数据显式存在，而不是藏在注释里

## Stage 0 Deliverables / 阶段零交付物

- `README.md` with project purpose and directory map.
- `.gitignore` that excludes local learning notes, RAW files, build products,
  and the original planning document.
- public docs for project foundation and repository boundary.
- initial CMake skeleton.
- minimal placeholder library or executable that builds.
- a first unit-test target using synthetic data.
- an initial `ImageBuffer` or equivalent design note/header.

- `README.md`：说明项目目的和目录结构。
- `.gitignore`：排除本地学习笔记、RAW 文件、构建产物和原始策划书。
- 项目基础说明和仓库边界说明等公开文档。
- 初始 CMake 骨架。
- 能构建的最小占位库或可执行程序。
- 使用合成数据的第一批单元测试目标。
- 初始 `ImageBuffer` 或等价设计说明/头文件。

## Acceptance Criteria / 验收标准

Stage 0 is done when:

阶段零完成标准：

- the project builds from a clean checkout using CMake
- a basic test command runs successfully
- no local learning notes or RAW files appear in normal `git status`
- the repository has a clear public/private content boundary
- Stage 1 can start without redesigning the folder structure

- 从干净 checkout 中可以用 CMake 构建项目
- 基础测试命令能成功运行
- 正常 `git status` 中不会出现本地学习笔记或 RAW 文件
- 仓库有清晰的公开/私有内容边界
- 阶段一可以在不重做目录结构的前提下开始

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

## Open Decisions / 待确认决策

- Confirm GitHub repository owner and URL.
- Confirm license author string before adding `LICENSE`.
- Choose the first camera RAW format and sample image set.
- Choose the first test framework when writing code.
- Decide whether Stage 0 should include only a CLI placeholder or also an empty
  core library target.

- 确认 GitHub 仓库所有者和 URL。
- 添加 `LICENSE` 前确认许可证作者署名字符串。
- 选择第一种相机 RAW 格式和样张集合。
- 写代码时选择第一版测试框架。
- 决定阶段零只包含 CLI 占位程序，还是也包含空的核心库 target。

