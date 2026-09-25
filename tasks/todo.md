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

## P6 Active Checkpoint / P6 当前检查点 — 2026-09-25

- [x] Clean isolated HEAD and live main both a4130b0 / 干净独立 HEAD 与实时 main 均为 a4130b0。
- [x] Fresh Debug configure/build and 76/76 tests / 新建 Debug 配置、编译及 76/76 测试。
- [x] Read P5 contracts, risk/boundary rules and exact P6 scope / 读取 P5 契约、风险/边界及 P6 精确范围。
- [x] User confirms P6 study; no fabricated exercise record / 用户确认已学习 P6，不补造习题记录。
- [x] Prepare bilingual design, API outline, proposed ADR and conditional slices / 双语设计、API 提纲、ADR 提案及条件性增量计划。
- [x] Review curve, processing state, output policy and module contracts / 审阅曲线、处理状态、输出策略及模块契约。
- [x] S1 exposure red/green, full suite and review / 曝光红绿、全套及审查。
- [x] S2 histogram red/green, full suite and review / 直方图红绿、全套及审查。
- [x] S3 tone-state validation and encoder compatibility / 影调状态校验及编码兼容。
- [x] S4 tone-curve red/green, full suite and review / 影调曲线红绿、全套及审查。
- [x] S5 two-branch integration and P5 neutral regression / 双分支集成及 P5 中性回归。
- [x] S6 minimal CLI and synthetic end-to-end checks / 最小 CLI 及合成端到端检查。
- [x] Fresh Debug/sanitizer/no-tests builds and final review / 新 Debug、sanitizer、关闭测试构建及终审。
- [x] Authorized in-place samples: numeric and agent visual checks / 授权样张原位数值及 Agent 视觉检查。
- [x] Agreed sample acceptance: daylight/dark and corrected backlit candidate / 约定样张验收：日光/暗景与逆光修正候选。

See [P6 preparation](../docs/p6-design-preparation.md) and the active section of
[plan](plan.md). This checkpoint supersedes old P6 not-yet-learned status, not
unrelated historical tasks. Local implementation verified; agreed sample acceptance is recorded below.
Publication is authorized; broader tone appearance remains deferred.
参见 P6 准备与当前计划。本检查点覆盖旧 P6 尚未学习状态，不覆盖其他历史任务。
本地实现已验证，约定样张验收见下方；已授权发布，更广影调观感延期。

---

Evidence / 证据：[P6 verification and review](p6-verification-review.md).

## P6 Visual Feedback Follow-up / P6 视觉反馈跟进

- [x] User reports no obvious daylight/dark issues / 用户确认日光、暗景无明显问题。
- [x] Reproduce backlit magenta and exposure-dependent clip bypass / 复现逆光粉紫及曝光造成的裁剪失效。
- [x] Verify a P5-baseline output candidate without changing production / 不改生产代码，验证 P5 基准输出候选。
- [x] Review and implement corrected output baseline and negative-EV regression / 审阅并实现修正输出基准与负 EV 回归。
- [x] Agent review and byte-exact match to user-approved candidate / Agent 复查，文件逐字节匹配用户已确认候选。

Broader tone appearance is deferred until after the middle stage by user request.
This is not visual acceptance. See [diagnosis](p6-highlight-regression-check.md).
更广影调观感按用户要求留到中期后，不计视觉通过；见诊断记录。

## P6 publication / P6 发布

- [x] User authorized Git operations through merge / 用户授权 Git 操作直到合并。
- [x] Reconcile current acceptance status / 统一当前验收状态。
- [x] Review full diff and public/private boundary / 审查完整差异与公开边界。
- [x] Commit, push and open PR #3 / 提交、推送并创建 PR #3。
- [x] Linux CI 102/102 and no unresolved review threads / Linux CI 102/102，无未解决审查讨论。

Final merge state is tracked by [PR #3](https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw/pull/3); every final head must pass CI before merge.
最终合并状态以 PR #3 为准；最终提交必须通过 CI 才合并。
