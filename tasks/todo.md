# Task Checklist / 任务清单

Status / 状态：Active / 进行中

## Preparation / 准备

- [x] Record project purpose and repository boundary.
- [x] Confirm D1-D6 decisions.
- [x] Verify three local Sony A7C II `.ARW` file headers.
- [x] Add MIT license with the approved author name.
- [x] Accept initial ADRs, pipeline contract, and validation policy.
- [x] Finish CMake and Ninja installation and record versions.
- [x] Install pkg-config and LibRaw and verify package discovery.
- [x] Record preview-publication permission and keep RAW sources private.
- [ ] Perform a final frame and metadata check on each derived preview before
  committing it.

- [x] 记录项目目的与仓库边界。
- [x] 确认 D1-D6 决策。
- [x] 验证 3 个本地 Sony A7C II `.ARW` 文件头。
- [x] 使用已确认署名添加 MIT 许可证。
- [x] 确认首批 ADR、流水线契约和验证策略。
- [x] 完成 CMake 与 Ninja 安装并记录版本。
- [x] 安装 pkg-config 与 LibRaw，并验证包发现。
- [x] 记录预览图公开许可，并保持 RAW 源文件私有。
- [ ] 每张派生预览提交前，执行最终画面与元数据检查。

## Stage 0 / 阶段零

- [x] Create CMake static-library, CLI, and test targets.
- [x] Integrate a pinned GoogleTest release.
- [x] Add the owned image-buffer model and invariant tests.
- [x] Add a CLI smoke path.
- [x] Add the Linux CI workflow.
- [x] Confirm Linux CI passes after the changes are published.
- [x] Pass Stage 0 acceptance and write the Chinese learning recap.

- [x] 创建 CMake 静态库、CLI 和测试 target。
- [x] 集成固定版本的 GoogleTest。
- [x] 添加拥有所有权的图像 buffer 模型与不变量测试。
- [x] 添加 CLI smoke 路径。
- [x] 添加 Linux CI 工作流。
- [x] 修改发布后确认 Linux CI 通过。
- [x] 通过阶段零验收并完成中文学习复盘。

Acceptance evidence: [Stage 0 closeout and P3 handoff](stage0-closeout.md), 2026-09-10.

验收证据：[阶段零收尾与 P3 交接](stage0-closeout.md)，2026-09-10。

## September–October Core Baseline / 9—10 月核心基线

- [x] P3 RAW ingest and sensor normalization (local acceptance, 2026-09-18).
- [x] P4 white balance and bilinear demosaic (local acceptance, 2026-09-19).
- [ ] P5 color transform, display encoding, and file output.
- [ ] P6 exposure, histogram, and one tone curve.
- [ ] P7 reproducibility, numeric, visual, and learning acceptance.

- [x] P3 RAW 读取与传感器归一化（本地验收，2026-09-18）。
- [x] P4 白平衡与双线性去马赛克（本地验收，2026-09-19）。
- [ ] P5 色彩转换、显示编码与文件输出。
- [ ] P6 曝光、直方图与一种影调曲线。
- [ ] P7 可复现性、数值、视觉与学习验收。

## P3 Checkpoints / P3 检查点

- [x] Define P3 contract and ADR-005 / 定义 P3 契约和 ADR-005。
- [x] Normalization TDD and synthetic properties / 归一化 TDD 与合成性质测试。
- [x] LibRaw adapter and boundary tests / LibRaw 适配层与边界测试。
- [x] Clean builds, full tests and code review / 干净构建、全套测试及代码审查。
- [x] Selected sample read authorization and real Sony numeric integration / 所选样张读取授权及真实 Sony 数值集成。
- [x] Generate and inspect local diagnostic previews / 生成并检查本地诊断预览。
- [x] User confirmation of real-sample visual acceptance (coarse inspection, 2026-09-18) / 用户确认真实样张视觉验收（粗略检查，2026-09-18）。
- [x] Separate P3 learning completion, confirmed by the user on 2026-09-18 / 独立 P3 学习完成，用户于 2026-09-18 确认。

Evidence / 证据：[P3 verification and review / P3 验证与审查](p3-verification-review.md).

## P4 Entry / P4 启动

- [x] Prepare the scope and learning handoff / 整理范围与学习交接。
- [x] P4 learning completed, user-confirmed 2026-09-19 / P4 学习完成，用户于 2026-09-19 确认。
- [x] Specify and review WB gains, CFA mapping, borders and numeric behavior / 定义并审阅白平衡增益、CFA 映射、边界与数值行为。
- [x] Implement and verify the scalar reference in small slices / 分增量实现并验证标量参考版本。

Handoff / 交接：[P4 entry and learning handoff / P4 启动与学习交接](p4-handoff.md).

## 2026-10-08 to 2026-11-06 Extensions / 2026-10-08 至 2026-11-06扩展

- [ ] Select Stage 3 controls from evidence and learning value.
- [ ] Add benchmarks before any parallel optimization.
- [ ] Decide whether OpenMP, dynamic-library export, or Metal earns its cost.

- [ ] 根据证据和学习价值选择阶段三控制项。
- [ ] 在任何并行优化前建立 benchmark。
- [ ] 决定 OpenMP、动态库导出或 Metal 是否值得投入。

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

- [x] User confirmed learning completion and all ADR choices / 用户确认学习完成及全部 ADR 选择。
- [x] WB TDD and review / 白平衡 TDD 及审查。
- [x] Demosaic TDD and review / 去马赛克 TDD 及审查。
- [x] Full/sanitizer/no-tests verification / 全套、sanitizer、关闭测试构建验证。
- [x] Real-sample numeric and agent visual checks / 真实样张数值及 Agent 视觉检查。
- [x] User coarse visual acceptance, 2026-09-19 / 用户粗略视觉验收，2026-09-19。

Evidence / 证据：[P4 verification and review / P4 验证与审查](p4-verification-review.md).
