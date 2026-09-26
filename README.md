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

P3–P6 provide RAW ingest/normalization, white balance/demosaic, color conversion,
sRGB/PNG output, exposure and histogram controls. P5/P6 merged through PR #2/#3.
P7's recorded-scope acceptance includes a fresh 102/102 local test run, exact-P6-merge
Linux CI evidence, Release without tests, and actual learner recap; see
[P7 evidence](tasks/p7-verification-review.md). These checks do not establish a
complete photographic editor, general RAW support or highlight reconstruction.
P3–P6 已提供 RAW 读取/归一化、白平衡/去马赛克、色彩转换、sRGB/PNG 输出、曝光和
直方图能力；P5/P6 已经 PR #2/#3 合并。P7 在已记录范围内完成新本地 102/102、
精确 P6 合并提交的 Linux CI 复核、关闭测试 Release 及本人复盘，详见综合证据。
这些检查不证明完整摄影编辑器、通用 RAW 支持或高光重建已经实现。

The user provisionally accepted P8 contracts → P9 continuous preview → P10
highlight/output review → P11 tonal-region editing → P12 color editing → P13
integrated acceptance. Each stage requires a detailed purpose/learning briefing and
confirmation before learning. The current P6 shoulder curve remains implemented
but is provisionally excluded from the future formal photo path. See
[stage gates and backup](docs/stage-gates-and-backup.md) and [entry audit](tasks/p8-entry-audit.md).
用户暂定采用 P8 契约→P9 连续预览→P10 高光/输出复核→P11 亮度分区→P12 颜色→
P13 综合验收。每阶段须先汇报目的及学习细节，经确认后开始学习。当前 P6 肩部实现
仍保留，但暂不进入未来正式照片路径；详见阶段关口、备份与入口检查。

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
- libpng 1.6.x and zlib from maintained patched packages / 带维护补丁的 libpng 1.6.x 与 zlib

Install runtime build dependencies with `brew install pkg-config libraw libpng` on
macOS or `sudo apt-get install pkg-config libraw-dev libpng-dev` on Ubuntu. LibRaw and libpng/zlib remain
required with tests disabled. P6 now connects P3–P5 through the processing CLI with explicit exposure, tone and
highlight policy. See [CLI usage](docs/cli.md).

macOS 使用 `brew install pkg-config libraw libpng`，Ubuntu 使用
`sudo apt-get install pkg-config libraw-dev libpng-dev` 安装依赖；关闭测试时仍需 LibRaw 和 libpng/zlib。
P6 现通过处理 CLI 串联 P3–P5，显式设置曝光、影调及高光策略，见 [CLI 用法](docs/cli.md)。

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


## P5 library path / P5 库调用路径

P5 supports explicit camera-to-linear-sRGB conversion and RGB16 sRGB PNG output.
Color-space identity is separate from processing state/transfer encoding; only sRGB
is implemented today. Negative and above-one working values survive until an output
copy is clipped and encoded. Existing unspecified images are not assumed sRGB.
P5 提供明确的相机到线性 sRGB 转换及 RGB16 sRGB PNG 输出，空间身份与处理状态/
传递编码分别表达；目前只实现 sRGB。线性负值/超一值保留，输出副本才裁剪编码，
已有未知空间图像不默认视为 sRGB。

```cpp
#include "mini_camera_raw/raw_decoder.h"
#include "mini_camera_raw/white_balance.h"
#include "mini_camera_raw/demosaic.h"
#include "mini_camera_raw/color_transform.h"
#include "mini_camera_raw/display_encode.h"
#include "mini_camera_raw/png_writer.h"
#include <stdexcept>

// path is an authorized local RAW; output_path must not already exist.
// path 为已授权本地 RAW；output_path 必须不存在。
auto raw = mini_camera_raw::decode_raw(path);
if (!raw.sensor.camera_wb_tile) throw std::runtime_error("missing camera WB");
auto linear = mini_camera_raw::normalize(raw.image, raw.sensor.levels);
auto gains = mini_camera_raw::normalize_camera_wb(
    *raw.sensor.camera_wb_tile, linear.metadata().cfa_pattern);
auto balanced = mini_camera_raw::apply_white_balance(linear, gains);
auto camera = mini_camera_raw::demosaic_bilinear(balanced);
auto matrix = mini_camera_raw::camera_to_working_matrix(raw.sensor);
auto working = mini_camera_raw::transform_camera_rgb(camera, matrix);
// Preserve working for later editing. Explicit hard-clip output branch:
// 保留 working 用于后续编辑；以下为明确的高光硬裁剪输出分支。
auto clipped_camera = mini_camera_raw::clip_camera_highlights(camera);
auto display_working = mini_camera_raw::transform_camera_rgb(clipped_camera, matrix);
auto encoded = mini_camera_raw::encode_srgb16(display_working);
mini_camera_raw::write_png16(encoded, output_path);
```

The PNG writer preserves raster orientation, refuses existing targets, and adds no
private EXIF. A crash can leave an incomplete new file. Caller owns a stable output
directory/path during writing; concurrent path mutation is not supported. This P5 library example is retained; the P6 processing CLI is documented separately. See [P5 verification](tasks/p5-verification-review.md)
and [ADR-007](docs/decisions/ADR-007-p5-color-and-output.md).
PNG 写入保留像素方向、拒绝已有目标、不复制私人 EXIF；崩溃可能留下不完整新文件。
调用方须在写入期间保持目录/路径稳定，不支持并发修改路径。本 P5 库示例保留，P6 处理 CLI 另有文档；见 P5 验证及 ADR-007。

The explicit camera-domain highlight clip sacrifices output highlight detail to
avoid the magenta fully-saturated core observed with direct encoding. It is not
reconstruction. P6 renders from preserved data and applies the same EV/tone parameters to both
explicit branches; the clipped preview never becomes the editing source.
显式相机域高光裁剪以牺牲输出高光细节为代价，避免直接编码时出现的完全饱和核心
粉紫；这不是重建。P6 从保留数据渲染，两个显式分支使用同一组曝光/影调参数，不使用裁剪预览作编辑源。
