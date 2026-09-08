# Implementation Plan / 实施计划

Status / 状态：Approved baseline / 已确认基线

Date / 日期：2026-07-09

P2 refinement / P2 细化：2026-07-16

## Delivery Definition / 交付定义

The fixed 2026-10-01 acceptance is the **core summer baseline**, not the full
original Stage 0-4 ambition. It must demonstrate an explainable RAW-to-image
CPU path plus a small tone engine. Stage 3 color/local controls and Stage 4
optimization/API export remain 2026-10-08 to 2026-11-06 extensions.

固定的 2026-10-01 验收定义为**暑期核心基线**，而不是原始阶段零到阶段四的全部设想。
核心版本必须展示可解释的 RAW 到图像 CPU 路径和小型影调引擎。阶段三色彩/局部控制与
阶段四优化/API 导出保留为 2026-10-08 至 2026-11-06 的扩展。

Schedule revised on 2026-09-08: the pause is extended through 2026-09-08; resume on 2026-09-09. All post-pause windows shift by 37 days. Weekly effort remains 28 hours; completed milestones and work-package estimates are unchanged.

2026-09-08 排期更新：暂停延长至 2026-09-08，2026-09-09 恢复；暂停后的所有窗口顺延 37 天。每周仍投入 28 小时，已完成节点和工作包工时估算保持不变。

## Schedule / 日程

| Window / 时间 | Budget / 预算 | Milestone / 里程碑 |
|---|---:|---|
| 2026-07-09 to 07-13 | preparation / 准备期 | Approve contracts, install tools, prepare learning workspace / 确认契约、安装工具、准备学习空间 |
| 2026-07-14 to 07-17 | about 16 h / 约 16 小时 | Stage 0 build, test, image-model skeleton / 阶段零构建、测试和图像模型骨架 |
| 2026-07-18 to 09-08 | paused / 暂停 | No planned project work / 不安排项目任务 |
| 2026-09-09 to 09-15 | about 28 h / 约 28 小时 | RAW ingest, metadata, normalization, white balance / RAW 读取、元数据、归一化和白平衡 |
| 2026-09-16 to 09-22 | about 28 h / 约 28 小时 | Bilinear demosaic, color/display conversion, file output / 双线性去马赛克、色彩/显示转换和文件输出 |
| 2026-09-23 to 09-29 | about 28 h / 约 28 小时 | Exposure, histogram, one tone curve, CLI integration / 曝光、直方图、一种影调曲线和 CLI 集成 |
| 2026-09-30 to 10-01 | about 8 h / 约 8 小时 | Acceptance, defect fixes, reproducibility and learning recap / 验收、缺陷修复、复现检查和学习复盘 |
| 2026-10-08 to 2026-11-06 / 2026-10-08 至 2026-11-06 | flexible / 弹性 | Stage 3 and 4 extensions selected by evidence / 根据证据选择阶段三、四扩展 |

## Work Packages / 工作包

### P0. Documentation and Tool Baseline / 文档与工具基线

Files / 文件：

- `LICENSE`
- `docs/stage0-engineering-spec.md`
- `docs/decisions/*.md`
- `docs/pipeline-contract.md`
- `docs/validation-policy.md`
- `docs/development-environment.md`

Done when / 完成条件：

- all blocking decisions are recorded
- CMake and Ninja versions are recorded
- the public/private sample boundary is explicit

- 所有阻塞决定均有记录
- 已记录 CMake 与 Ninja 版本
- 公开与私有样张边界明确

### P1. Build and Test Skeleton / 构建与测试骨架

Estimated effort / 预计用时：6 h

Files / 文件：

- `CMakeLists.txt`
- `src/CMakeLists.txt`
- `apps/CMakeLists.txt`
- `tests/CMakeLists.txt`
- `.github/workflows/ci.yml`

Behavior and tests / 行为与测试：

- configure with CMake 3.24+
- build static library and CLI with C++17
- fetch pinned GoogleTest and discover tests through CTest
- run one CLI smoke test locally and one Linux CI build

- 使用 CMake 3.24+ 配置
- 以 C++17 构建静态库与 CLI
- 获取固定版本 GoogleTest，并由 CTest 发现测试
- 本地运行一个 CLI smoke test，并完成一次 Linux CI 构建

### P2. Image Model / 图像模型

Estimated effort / 预计用时：8 h

Files / 文件：

- `include/mini_camera_raw/image_buffer.h`
- `include/mini_camera_raw/image_metadata.h`
- `src/image_buffer.cpp`
- `tests/image_buffer_test.cpp`

Behavior and tests / 行为与测试：

- represent owned contiguous Bayer `uint16_t` and working `float` buffers
- expose dimensions, element stride, pixel format, CFA pattern, color state,
  and declared nominal range
- allow explicit row padding while keeping one contiguous owned allocation
- reject zero dimensions, storage mismatch, short stride, unknown enums,
  incompatible state combinations, and overflow in row/element/byte counts
- test one-pixel, odd-size, ordinary, padded-row, and float working images,
  typed const/mutable access, bounds, and ordinary copy/move ownership behavior
- verify with a Ninja clean build and full CTest run, then a separate clean
  `BUILD_TESTING=OFF` build

- 表达连续且拥有所有权的 Bayer `uint16_t` 与工作 `float` buffer
- 显式提供尺寸、以元素计的行跨度、像素格式、CFA 排列、色彩状态和声明的
  名义范围
- 在保持单块连续 owned allocation 的同时允许显式行 padding
- 拒绝零尺寸、storage 不匹配、过短 stride、未知枚举、不兼容状态组合，以及
  行大小/元素数/字节数计算溢出
- 测试单像素、奇数尺寸、普通、带行 padding 和 float 工作图像，并覆盖带类型
  的 const/可变访问、边界与普通 copy/move 所有权行为
- 使用 Ninja 干净构建和完整 CTest 验证，再执行独立的干净
  `BUILD_TESTING=OFF` 构建

Dependencies / 依赖：P1 build/test skeleton and accepted ADR-002 / P1 构建测试
骨架与已确认的 ADR-002。

Acceptance oracle / 验收判据：the public API contract in ADR-002 and the P2
acceptance contract in `docs/stage0-engineering-spec.md` / ADR-002 的公开 API
契约与 `docs/stage0-engineering-spec.md` 的 P2 验收契约。

### P3. RAW Ingest and Sensor Normalization / RAW 读取与传感器归一化

Estimated effort / 预计用时：22 h

Files / 文件：

- `include/mini_camera_raw/raw_decoder.h`
- `src/raw_decoder_libraw.cpp`
- `include/mini_camera_raw/normalize.h`
- `src/normalize.cpp`
- `tests/normalize_test.cpp`

Behavior and tests / 行为与测试：

- decode the mosaic and required metadata without LibRaw postprocessing
- normalize with explicit per-CFA-position black levels and white level
- preserve mathematically valid values outside nominal `[0,1]`
- validate synthetic exact cases and run local A7C II integration checks

- 在不启用 LibRaw 后处理的情况下读取马赛克与必要元数据
- 使用分 CFA 位置黑电平和白电平显式归一化
- 保留名义 `[0,1]` 外的数学有效值
- 验证合成精确样例，并运行本地 A7C II 集成检查

### P4. White Balance and Bilinear Demosaic / 白平衡与双线性去马赛克

Estimated effort / 预计用时：24 h

Files / 文件：

- `include/mini_camera_raw/white_balance.h`
- `src/white_balance.cpp`
- `include/mini_camera_raw/demosaic.h`
- `src/demosaic_bilinear.cpp`
- `tests/demosaic_test.cpp`

Behavior and tests / 行为与测试：

- apply positive finite gains according to CFA phase
- implement a readable scalar bilinear reference
- define border handling explicitly
- test constant fields, channel impulses, ramps, odd dimensions, and each
  supported CFA phase

- 按 CFA 相位应用有限正增益
- 实现清晰的标量双线性参考版本
- 显式定义边界处理
- 测试常量场、通道脉冲、渐变、奇数尺寸和每种受支持 CFA 相位

### P5. Color, Display, and File Output / 色彩、显示与文件输出

Estimated effort / 预计用时：20 h

Files / 文件：

- `include/mini_camera_raw/color_transform.h`
- `src/color_transform.cpp`
- `include/mini_camera_raw/display_encode.h`
- `src/display_encode.cpp`
- `tests/color_transform_test.cpp`

Behavior and tests / 行为与测试：

- apply a documented camera-to-working matrix direction
- preserve negative and above-one intermediate RGB
- apply explicit sRGB encoding and final clipping
- write one documented PNG or TIFF output path

- 应用方向明确的相机到工作空间矩阵
- 保留负值和大于一的中间 RGB
- 显式应用 sRGB 编码和最终裁剪
- 实现一种有文档说明的 PNG 或 TIFF 输出路径

### P6. Minimal Tone Engine and CLI / 最小影调引擎与 CLI

Estimated effort / 预计用时：18 h

Files / 文件：

- `include/mini_camera_raw/tone.h`
- `src/tone.cpp`
- `apps/mini_camera_raw.cpp`
- `tests/tone_test.cpp`
- `docs/cli.md`

Behavior and tests / 行为与测试：

- exposure uses `2^EV` in linear working RGB
- histogram binning has documented range and out-of-range policy
- one monotonic contrast/tone curve is implemented and tested
- CLI accepts input, output, and a small stable parameter set

- 曝光在线性工作 RGB 中使用 `2^EV`
- 直方图分箱有明确范围与超范围策略
- 实现并测试一种单调对比度/影调曲线
- CLI 接受输入、输出和一组小而稳定的参数

### P7. Acceptance / 验收

Estimated effort / 预计用时：10 h

Evidence / 证据：

- clean local configure, build, and CTest pass
- Linux CI pass
- numeric report for each implemented stage
- visual record for the local sample categories
- Chinese learning recap explaining formulas, ranges, failure modes, and
  remaining uncertainty

- 本地干净配置、构建和 CTest 通过
- Linux CI 通过
- 每个已实现阶段都有数值验证记录
- 本地样张类别都有视觉检查记录
- 中文学习复盘能够解释公式、范围、失效模式和剩余疑问

## Scope Change Rules / 范围变更规则

At each weekly checkpoint, protect P3-P5 correctness first. If the schedule
slips, reduce P6 controls to exposure only before weakening tests,
documentation, or learning work. Do not pull Stage 3/4 work into September–October until
the acceptance evidence above is complete.

每周检查点首先保护 P3-P5 的正确性。如果进度落后，先把 P6 缩减到仅保留曝光，
不要削弱测试、文档或学习任务。在上述验收证据齐全之前，不把阶段三、四工作提前到 9—10 月。
