# Implementation Plan / 实施计划

## Active P6 checkpoint / 当前 P6 检查点 — 2026-09-25

P6 implements exposure, histogram, one monotonic tone curve and CLI. The user
confirmed study and approved the corrected right-column output on 2026-09-25.
ADR-009 supersedes ADR-008's render order. Original baseline: a4130b0, 76 tests.
P6 实现曝光、直方图、一种单调影调曲线及 CLI。用户已确认学习，并于 2026-09-25
批准右列输出修正。ADR-009 替代 ADR-008 输出顺序；原始基线 a4130b0，76 项测试。

Follow-up sequence / 跟进顺序：
1. Update contract/ADR, reproduce negative-EV failure / 更新契约与 ADR、复现负 EV 失败。
2. Fix derivative ordering and CLI explanation, run full tests / 修正输出副本顺序及 CLI 说明、全套测试。
3. Run sanitizers/no-tests build, three local samples and review / sanitizer、关闭测试构建、三样张及审查。
4. Record evidence; keep broader tone appearance deferred / 记录证据，影调观感继续延期。

| Slice / 增量 | Dependencies / 依赖 | Files / 文件 | Acceptance / 验收 |
|---|---|---|---|
| S1 exposure / 曝光 | approved contract / 契约确认 | tone.h, tone.cpp, tone_test.cpp, src/CMakeLists.txt, tests/CMakeLists.txt | signed EV, neutral identity, errors, ownership / 带符号 EV、中性恒等、错误及所有权 |
| S2 histogram / 统计 | approved contract / 契约确认 | histogram.h, histogram.cpp, histogram_test.cpp, two CMake files / 两个 CMake 文件 | exact edges, conservation, padding, RGB/Y / 精确边界、守恒、padding、RGB/Y |
| S3 tone state / 影调状态 | approved state / 状态确认 | image_metadata.h, image_buffer.cpp, image_buffer_test.cpp, display_encode.cpp, display_encode_test.cpp | new state validation and encoder compatibility / 新状态校验及编码兼容 |
| S4 tone curve / 影调曲线 | S1,S3 | tone.h, tone.cpp, tone_test.cpp, histogram.cpp, histogram_test.cpp | known values, Y monotonicity, ratios, new-state histogram / 参考值、Y 单调、比例、新状态统计 |
| S5 render branch / 输出分支 | S1–S4 | render_pipeline.h, render_pipeline.cpp, render_pipeline_test.cpp, two CMake files / 两个 CMake 文件 | both modes apply EV/tone once; P5 neutral regression / 两模式只应用一次曝光影调，P5 中性回归 |
| S6 CLI / 命令行 | S5 | apps/mini_camera_raw.cpp, apps/CMakeLists.txt, tests/cli_test.cmake, tests/CMakeLists.txt, docs/cli.md | strict arguments, --version, nonoverwrite, synthetic end-to-end / 严格参数、版本、不覆盖、合成端到端 |

render_pipeline files may remain app-internal; settle that boundary in the final
render-cli spec. Each slice starts with failing observable tests, then implementation,
targeted tests, full suite and tests-first code review. Do not proceed past a failed
checkpoint. Update specs before implementation if review changes a contract.
render_pipeline 可保持应用内部，最终 render-cli 规格明确边界。每增量先写可观察的
失败测试，再实现、定向测试、全套及先看测试的代码审查；失败不继续，契约变化先改规格。

Use the preparation document's configure/build/full-test commands; after test names
exist, targeted CTest filters are `-R 'Exposure|Histogram|Tone|Render|Cli'`. Verify
the selected count is nonzero. Final checks: fresh sanitizer and BUILD_TESTING=OFF
builds, authorized in-place samples, numeric/visual evidence, git diff --check,
ignore-boundary check. Agreed sample checks are complete; broader tone appearance
is deferred. Commit, push, PR and merge are now authorized.
用准备文档的配置/构建/全套命令，测试命名落实后用上述过滤器定向运行并确认数量非零。
最后做新 sanitizer/关闭测试构建、授权样张原位数值及视觉验证、diff 与忽略边界检查。
约定样张检查已完成，更广影调观感延期；现已授权提交、推送、PR 及合并。

Accepted: curve/state and ADR-009 output order. Daylight/dark checks and the
corrected backlit candidate are accepted within the agreed scope. Broader tone
appearance remains deferred. Publish after review, then require Linux CI success
and no unresolved required review finding before merging.
已接受曲线/状态及 ADR-009 输出顺序；日光/暗景检查与逆光修正候选在约定范围内已
确认。更广影调观感延期。完成审查后发布，Linux CI 通过且无未解决必改项后合并。

Implementation detail: S2 added a private shared luminance helper. S6 was split
into argument tests/CLI implementation, then synthetic decoder-substitute execution
and PNG checks reusing the existing independent decoder in a test-only header.
Production has no synthetic-input switch. Final evidence: [P6 verification](p6-verification-review.md).
实施细化：S2 增加内部共享亮度 helper；S6 分参数测试/CLI 实施、合成解码替身执行及
PNG 验证两个检查点，复用原独立 PNG 解码器到测试头文件。生产不提供合成输入开关，
最终证据见 P6 验证。

---

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

P3 execution slices (2026-09-10); total estimate remains 22 h:

P3 实施增量（2026-09-10），总估算仍为 22 小时：

1. Contract and ADR-005, then normalization TDD (normalize header/source/test
   and source/test CMake). Accept exact hand vectors and invalid-input rejection;
   verify `ctest --test-dir build/p3 -R Normalize --output-on-failure`.
2. RAW public metadata and private LibRaw adapter with synthetic layout tests
   (raw header/source, private header, decoder tests and CMake registration).
   Depends on slice 1; accept crop/pitch/CFA/ownership checks and error tests;
   verify `ctest --test-dir build/p3 -R Raw --output-on-failure`.
3. Build discovery, dependency documentation and review evidence. Depends on 2;
   run all commands in `docs/p3-raw-normalization-spec.md`, then review tests first.
4. Authorized local Sony sample integration, numeric/mosaic checks and separate
   learning acceptance remain explicit gates; do not mark them done from synthetic tests.

1. 先完成契约与 ADR-005，再以 TDD 完成归一化（头/源/测试和两处 CMake）；
   验收手算向量和非法输入拒绝，执行上述 Normalize 定向测试。
2. 完成 RAW 公开元数据和私有 LibRaw 适配层及合成布局测试（RAW 头/源、私有头、
   解码测试和 CMake 注册）；依赖增量 1，验收裁剪/pitch/CFA/所有权与错误路径。
3. 完成构建发现、依赖文档及审查证据；依赖增量 2，执行 P3 规格全部命令，先审测试。
4. 已授权 Sony 样张集成、数值/马赛克检查和独立学习验收仍为明确门槛，不凭合成测试勾选。

### P4. White Balance and Bilinear Demosaic / 白平衡与双线性去马赛克

Entry update 2026-09-18: P3 local acceptance is complete, including user-confirmed
learning and coarse visual inspection. Follow [P4 handoff](p4-handoff.md) to
confirm P4 learning, then specify the algorithm/API before implementation.

2026-09-18 启动更新：P3 本地验收已完成，包括用户确认的学习完成和粗略视觉检查。
按 [P4 交接](p4-handoff.md) 确认 P4 学习，再定义算法/API 规格并实施。

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

## P4 Preparation Update / P4 准备更新 — 2026-09-18

The user confirmed P4 is not yet learned; prepare design only, with learning in a
separate task. The [P4 draft](../docs/p4-white-balance-demosaic-spec.md) and proposed
ADR-006 are ready for later review. No P4 algorithm is implemented.

用户确认尚未学习 P4，本轮只准备设计，学习另行进行。P4 草案及 ADR-006 提案已备好，
供后续审阅；尚未实现 P4 算法。

- [x] Verify isolated branch, required HEAD and clean starting tree / 核对独立分支、指定 HEAD 及初始干净状态。
- [x] Confirm learning status: not started / 确认学习状态：尚未开始。
- [x] Rebuild baseline and pass 42/42 CTest / 重建基线并通过 42/42 CTest。
- [x] Prepare draft formulas, API, boundaries and test vectors / 准备公式、API、边界及测试向量草案。
- [ ] Complete separate learning and learner hand calculation / 完成独立学习及学习者手算。
- [ ] Review draft and accept ADR before detailed implementation planning / 审阅草案并接受 ADR，再细化实施计划。
- [ ] Implement WB then demosaic with TDD and numeric/visual review / 以 TDD 先实现白平衡后实现去马赛克，完成数值/视觉审查。

Baseline: CMake 4.3.4, Apple Clang 21.0.0, LibRaw 0.22.1; Debug/Ninja build in
`build/p4-baseline`, CommandLineTools and existing GoogleTest source cache via
`FETCHCONTENT_SOURCE_DIR_GOOGLETEST`. Linker reported duplicate `-lc++` warnings;
build and tests passed. No new sanitizer or real-photo run at this design checkpoint.
No commit or push.

基线工具版本如上；Debug/Ninja 构建位于 `build/p4-baseline`，使用 CommandLineTools
及指定变量复用现有 GoogleTest 源码缓存。链接器提示重复 `-lc++`，构建及测试通过。
本轮仅设计，未新增 sanitizer 或真实照片运行；未提交或推送。

## P4 Execution / P4 实施 — 2026-09-19

Learning completed and three ADR choices accepted by the user. Continue locally;
no commit/push. Earlier preparation entries are historical.
用户已确认学习完成及三项 ADR 选择。本地继续，不提交或推送。此前准备记录为历史状态。

1. WB slice: header/source/test plus source/test CMake (five files). Accept literal
   gains, camera normalization, all CFA, numeric errors and layout invariance.
   Verify build and `ctest --test-dir build/p4 -R WhiteBalance --output-on-failure`,
   then full suite and review. / 白平衡增量涉及头/源/测试及两处 CMake，验收增益、相位、
   数值异常及布局不变量，执行上述定向及全套测试并审查。
2. Demosaic slice: header/source/test plus two CMake files; depends on accepted
   contract and slice 1 checkpoint. Accept known stencils, all CFA, borders, minimum
   sizes and generated oracle. Verify `ctest --test-dir build/p4 -R Demosaic --output-on-failure`,
   full suite and review. / 去马赛克增量同样五文件，依赖契约及白平衡检查点；验收模板、
   相位、边界、小尺寸及生成参考，执行定向及全套测试并审查。
3. Integration: full Debug and ASan/UBSan suites, no-tests build, authorized raw
   numeric and visual diagnostics, verification report and checklist updates.
   Inspect diagnostics before reporting completion; distinguish user visual acceptance.
   / 集成：Debug、ASan/UBSan 全套、关闭测试构建、授权原片数值与视觉诊断、验证报告
   及清单更新；检查图像，区分用户视觉验收。

P4 execution checkpoint: slices 1–3 engineering work completed, 58/58 Debug and
sanitizer tests pass; no-tests build and real-sample checks passed. User coarse visual
acceptance passed on 2026-09-19; see [P4 review](p4-verification-review.md).

P4 实施检查点：增量 1–3 工程工作完成，Debug/sanitizer 各 58/58，通过关闭测试
构建及真实样张检查；用户粗略视觉验收通过，详见 P4 审查报告。

## P5 Preparation / P5 准备 — 2026-09-19

Worktree: `build/p5-worktree`; branch `codex/p5-color-display-output`;
HEAD `d8cd328c16ab593b307c59524a9762d9120ee3c9`; clean starting tree.
See [P5 design preparation](../docs/p5-design-preparation.md).
工作区、分支和 HEAD 如上，起始干净；参见 P5 设计准备。

- [x] Verify isolated baseline and ignored build/learning paths / 核对独立基线及忽略边界。
- [x] Fresh Debug build and 58/58 CTest / 新建 Debug 构建，58/58 测试通过。
- [x] User confirms P5 not yet learned: prepare design only / 用户确认尚未学习 P5，仅准备设计。
- [x] Prepare capability map, source evidence and acceptance outline / 准备能力划分、来源依据和验收提纲。
- [x] Inspect authorized sample matrix metadata in place, 2026-09-21 / 原位核对三张授权样张矩阵，2026-09-21。
- [x] P5 learning completed, user-confirmed 2026-09-21 / P5 学习完成，用户于 2026-09-21 确认。
- [x] Review capability boundaries and output/dependency choice / 审阅能力边界及输出/依赖选择。
- [x] Complete module contracts, dependency review and ADR-007 / 完成模块契约、依赖审查及 ADR-007。
- [x] Plan TDD slices after design acceptance / 设计接受后规划 TDD 增量。

CMake 4.3.4, Apple Clang 21.0.0, LibRaw 0.22.1; cached GoogleTest 1.17.0.
Duplicate -lc++ warnings did not prevent build/tests. No new sanitizer, no-tests,
real-photo run, code change, commit or push at this preparation checkpoint.
基线版本如上；重复 -lc++ 警告未影响构建和测试。本准备检查点未新增 sanitizer、
关闭测试或真实照片运行，未修改代码、提交或推送。

Run from P5 worktree / 从 P5 工作区执行：

```sh
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake -S . -B build/p5-baseline -G Ninja -DCMAKE_BUILD_TYPE=Debug -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST='/Users/sgxz/Mini - Camera Raw/build/p2-verify/_deps/googletest-src'
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake --build build/p5-baseline
DEVELOPER_DIR=/Library/Developer/CommandLineTools ctest --test-dir build/p5-baseline --output-on-failure
```


P5 update 2026-09-21: learning confirmed; three sample matrices match and have zero
fourth columns. Focused libpng dependency/API review and proposed ADR-007 prepared.
Design and dependency approval remain pending; no P5 algorithm or publication.
P5 更新：学习已确认；三张矩阵相同且第四列为零。已补充 libpng 定向依赖/API 审查
和 ADR-007 提案；设计及依赖仍待确认，未实现 P5 算法或发布。


P5 design clarification, 2026-09-21: user accepted sRGB-first output flow with future
color-space extensibility. Prefer separate color-space metadata over sRGB-only state
combinations; preserve the working image and clip only its output derivative.
Dependency and exact API approval are not inferred. See updated ADR-007.
P5 设计澄清：用户确认首版 sRGB 输出流程，并要求未来扩展色彩空间。优先采用独立
色彩空间元数据，保留工作图像，仅裁剪输出副本。不推定依赖及精确 API 已获确认，
详见更新的 ADR-007。


## P5 implementation / P5 实施 — 2026-09-21

User authorized stage implementation including proposed libpng output; no publication.
用户授权实施本阶段（包含提案 libpng 输出），不含发布。

- [x] S1: metadata contract, tests then implementation; full suite / 元数据契约，先测试后实现并全套验证。
- [x] S2: matrix header/source/tests + two CMake files; directed and full tests / 矩阵五文件增量，定向及全套验证。
- [x] S3: sRGB output header/source/tests + CMake; same checks / 编码增量，同样验证。
- [x] S4: PNG writer and independent decoder tests; dependency/CI config / PNG 与独立解码测试及构建配置。
- [x] S5: Debug, ASan/UBSan, no-tests, real samples, review and evidence / 集成验证、样张、审查及记录。

Each slice depends on the previous contract checkpoint. Run cmake --build build/p5,
then ctest --test-dir build/p5 --output-on-failure. Keep all generated files ignored.
每增量依赖前一契约检查点，执行上述构建/测试；生成文件始终忽略。


### P5 local engineering checkpoint / P5 本地工程检查点

- [x] Debug 73/73 and ASan/UBSan 73/73; no-tests build / 两套测试各 73/73，关闭测试构建通过。
- [x] Three authorized samples, numeric reference and source preservation / 三张授权样张参考比较及原图保留验证。
- [x] Agent thumbnail inspection and highlight limitation recorded / Agent 缩略图检查及高光限制已记录。
- [x] User coarse acceptance: daylight and dark scene / 用户日光与暗景粗验收通过。
- [x] Backlit highlights: explicit output clipping implemented and revised previews accepted / 逆光显式高光输出裁剪已实现，新版粗验收通过。
- [ ] Remote Linux CI after separate publication authorization / 另获发布授权后的远端 Linux CI。

See [P5 verification](p5-verification-review.md). No commit/push.
详见 P5 验证，未提交或推送。

### Authorized highlight extension / 已授权高光扩展

- [x] User chose implementation in P5; explicit camera-domain output clip contract / 用户选择 P5 内实现，已定义相机域输出裁剪契约。
- [x] Red/green tests and immutable scalar reference / 红绿测试与不修改输入的标量参考。
- [x] Revised three-scene visual acceptance and self-review / 三场景新版粗验收与自审完成（独立审查覆盖限制见记录）。

- [x] Highlight extension: Debug and ASan/UBSan 76/76, no-tests build, real source immutability / 高光扩展两套 76/76，关闭测试构建及真实源数据不变验证。
- [x] Self-review complete; independent review returned no report and is not counted / 自审完成，独立审查未返回报告不计完成。

## Publication authorization / 发布授权 — 2026-09-21

User requested upload and merge. The earlier no-publication statements describe
the implementation checkpoint; publication is now authorized. Submit only P5
source, tests, build configuration and bilingual documentation; merge after remote
CI succeeds. Private images, learning and build artifacts remain local.
用户要求上传合并。此前未发布的表述为实施检查点历史状态，现已授权发布。只提交
P5 源码、测试、构建配置与双语文档，远端 CI 通过后合并；私人图像、学习及构建产物
保持本地。

P6 highlight follow-up completed locally: Debug/sanitizers 101/101, 36 real cases,
byte-identical approved backlit previews. See p6-highlight-regression-check.md.
P6 高光跟进本地完成：两套 101/101、36 个真实案例、逆光预览逐字节匹配已确认候选。

## P6 publication plan / P6 发布计划

1. Reconcile acceptance records and review complete P6 diff / 统一验收记录，审查完整差异。
2. Commit focused core, CLI and documentation changes on codex/p6-tone-cli / 在专用分支分组提交核心、CLI、文档。
3. Push and create PR; pass Linux CI, inspect review state, merge verified head / 推送并创建 PR，通过 Linux CI，检查审查状态并合并已验证提交。

Publication authorization covers P6 only. P7 and broader tone work are subsequent tasks.
本轮发布范围为 P6；P7 与更广影调工作为后续任务。
