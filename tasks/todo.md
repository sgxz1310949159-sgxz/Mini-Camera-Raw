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
- [ ] Add the owned image-buffer model and invariant tests.
- [x] Add a CLI smoke path.
- [x] Add the Linux CI workflow.
- [x] Confirm Linux CI passes after the changes are published.
- [ ] Pass Stage 0 acceptance and write the Chinese learning recap.

- [x] 创建 CMake 静态库、CLI 和测试 target。
- [x] 集成固定版本的 GoogleTest。
- [ ] 添加拥有所有权的图像 buffer 模型与不变量测试。
- [x] 添加 CLI smoke 路径。
- [x] 添加 Linux CI 工作流。
- [x] 修改发布后确认 Linux CI 通过。
- [ ] 通过阶段零验收并完成中文学习复盘。

## August Core Baseline / 8 月核心基线

- [ ] P3 RAW ingest and sensor normalization.
- [ ] P4 white balance and bilinear demosaic.
- [ ] P5 color transform, display encoding, and file output.
- [ ] P6 exposure, histogram, and one tone curve.
- [ ] P7 reproducibility, numeric, visual, and learning acceptance.

- [ ] P3 RAW 读取与传感器归一化。
- [ ] P4 白平衡与双线性去马赛克。
- [ ] P5 色彩转换、显示编码与文件输出。
- [ ] P6 曝光、直方图与一种影调曲线。
- [ ] P7 可复现性、数值、视觉与学习验收。

## September Extensions / 9 月扩展

- [ ] Select Stage 3 controls from evidence and learning value.
- [ ] Add benchmarks before any parallel optimization.
- [ ] Decide whether OpenMP, dynamic-library export, or Metal earns its cost.

- [ ] 根据证据和学习价值选择阶段三控制项。
- [ ] 在任何并行优化前建立 benchmark。
- [ ] 决定 OpenMP、动态库导出或 Metal 是否值得投入。
