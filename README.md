# Mini-Camera Raw

Mini-Camera Raw is a summer personal project for learning image signal
processing, RAW development, and C++ engineering through a small, transparent
RAW-to-RGB processor.

Mini-Camera Raw 是一个暑期个人项目，用一个小而透明的 RAW 到 RGB 处理器来学习图像信号处理、RAW 解析和 C++ 工程实践。

The goal is not to clone the full Adobe Camera Raw feature set. The goal is to
build a clear pipeline that can read RAW image data, run basic ISP steps, expose
simple tone/color controls, and document the principles behind each stage.

项目目标不是完整复刻 Adobe Camera Raw 的全部功能，而是搭建一条清晰可解释的处理流水线：读取 RAW 图像数据，运行基础 ISP 步骤，提供简单的影调与色彩控制，并记录每个阶段背后的原理。

## Project Focus / 项目重点

- Build a readable C++ core for RAW ingestion and ISP-style processing.
- Prioritize correctness, explainability, and testability before speed.
- Keep UI experiments separate from the core engine.
- Record technical decisions and learning notes as the project evolves.
- Treat GPU/Metal acceleration as an advanced extension after the CPU pipeline
  is stable.

- 构建可读性强的 C++ 核心，用于 RAW 读取和 ISP 风格处理。
- 先保证正确性、可解释性和可测试性，再追求速度。
- 将 UI 实验与核心算法引擎分离。
- 随着项目推进持续记录技术决策和学习笔记。
- 在 CPU 流水线稳定之后，再把 GPU/Metal 加速作为高级扩展。

## Public Repository Contents / 公开仓库内容

This repository is intended to contain:

- source code for the core engine and small test applications
- CMake/build configuration
- public technical documentation and project specs
- unit tests, synthetic fixtures, and benchmark code
- sanitized examples that are safe and licensed to publish

本仓库计划包含：

- 核心引擎和小型测试程序的源代码
- CMake 与构建配置
- 可公开的技术文档和项目规格
- 单元测试、合成测试数据和 benchmark 代码
- 经过清理、确认可发布且授权安全的示例

Personal study notes, private RAW photos, generated outputs, and the original
planning document are kept local-only. See `docs/repository-boundary.md`.

个人学习笔记、私人 RAW 照片、生成输出和原始策划书仅保留在本地。详见 `docs/repository-boundary.md`。

## Current Status / 当前状态

The Stage 0 build/test skeleton and P2 owned image model are available: a
static core library, a minimal CLI, GoogleTest/CTest integration, Linux CI,
and validated Bayer/linear working buffers. Stage 0 acceptance and the separate
Chinese learning recap are complete; see `tasks/stage0-closeout.md`. P3 now has
LibRaw ingest and explicit sensor normalization with synthetic tests. Numeric
integration on three authorized Sony A7C II `.ARW` samples passed; real-sample
diagnostic previews were inspected and the user accepted a coarse visual check
on 2026-09-18. Separate P3 learning completion was also user-confirmed. P3 local
acceptance is complete; P4 entry is described in `tasks/p4-handoff.md`.

阶段零构建测试骨架与 P2 owned 图像模型已经可用，包括静态核心库、最小 CLI、
GoogleTest/CTest 集成、Linux CI，以及经过校验的 Bayer/线性工作 buffer。
阶段零验收和独立的中文学习复盘已完成，详见 `tasks/stage0-closeout.md`。P3 已有
LibRaw 读取、显式传感器归一化和合成测试；三张已授权 Sony A7C II `.ARW` 的数值集成
通过，本地诊断预览已检查，用户于 2026-09-18 确认粗略视觉检查无问题及独立 P3 学习完成。
P3 本地验收完成，P4 启动交接见 `tasks/p4-handoff.md`。

P4 white balance and scalar bilinear demosaic are implemented locally. Debug and
ASan/UBSan each pass 58/58 tests; three authorized samples pass full-image numeric
checks. Separate learning and design are user-confirmed; user coarse visual acceptance passed on 2026-09-19. See [P4 verification](tasks/p4-verification-review.md).

P4 白平衡与标量双线性去马赛克已在本地实现。Debug 及 ASan/UBSan 均通过 58/58
测试，三张授权样张全图数值检查通过。独立学习及设计已由用户确认，用户粗略视觉验收通过。
详见 [P4 验证](tasks/p4-verification-review.md)。

Key references / 关键文档：

- [Stage 0 engineering specification / 阶段零工程规格](docs/stage0-engineering-spec.md)
- [ISP pipeline contract / ISP 流水线契约](docs/pipeline-contract.md)
- [Validation policy / 验证策略](docs/validation-policy.md)
- [P3 contract / P3 契约](docs/p3-raw-normalization-spec.md)
- [P4 contract / P4 契约](docs/p4-white-balance-demosaic-spec.md)
- [Implementation plan / 实施计划](tasks/plan.md)

## Build and Test / 构建与测试

Requirements / 环境要求：

- CMake 3.24 or newer / CMake 3.24 或更高版本
- a C++17 compiler / 支持 C++17 的编译器
- Ninja
- pkg-config and installed LibRaw 0.21+ / pkg-config 与系统安装的 LibRaw 0.21+

Install runtime build dependencies with `brew install pkg-config libraw` on
macOS or `sudo apt-get install pkg-config libraw-dev` on Ubuntu. LibRaw remains
required with tests disabled. The CLI currently exposes only the version smoke
path; P3/P4 processing is available through the C++ library API.

macOS 使用 `brew install pkg-config libraw`，Ubuntu 使用
`sudo apt-get install pkg-config libraw-dev` 安装依赖；关闭测试时仍需 LibRaw。
CLI 当前仅提供版本 smoke 路径，P3/P4 处理通过 C++ 库 API 使用。

Configure, build, test, and run the CLI smoke path:

配置、构建、测试并运行 CLI smoke 路径：

```sh
cmake -S . -B build/default -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/default
ctest --test-dir build/default --output-on-failure
./build/default/apps/mini-camera-raw --version
```

The first test-enabled configure downloads the pinned GoogleTest dependency.
To build the library and CLI without downloading test dependencies:

第一次启用测试的配置会下载固定版本的 GoogleTest。若只构建核心库和 CLI，且不下载
测试依赖：

```sh
cmake -S . -B build/no-tests -G Ninja -DBUILD_TESTING=OFF \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build/no-tests
./build/no-tests/apps/mini-camera-raw --version
```

## Directory Map / 目录结构

```text
include/mini_camera_raw/   Public C++ headers / 公开 C++ 头文件
src/                       Core implementation / 核心实现
apps/                      Small executables and temporary UI experiments / 小型程序和临时 UI 实验
tests/                     Unit and regression tests / 单元测试和回归测试
benchmarks/                Performance measurement tools / 性能测量工具
docs/                      Public project documentation / 公开项目文档
samples/                   Public sample policy and optional tiny fixtures / 示例数据策略和可选小型测试数据
learning/                  Local-only study notes, ignored by git / 本地学习笔记，已被 git 忽略
```
