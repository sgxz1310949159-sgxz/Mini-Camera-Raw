# P6 Verification and Review / P6 验证与审查

Date / 日期：2026-09-25
Status / 状态：P6 implementation and agreed sample checks complete; broader tone appearance deferred; publication authorized / P6 实现及约定样张检查完成，更广影调观感延期，已授权发布。

Current acceptance: daylight/dark have no obvious issues; the user approved the
corrected backlit candidate and production PNGs match it byte-for-byte. Broader
tone appearance is deferred until after the middle stage, not marked accepted.
The user authorized review, commit, push, PR and merge. Earlier checkpoints below
are historical; current checks are 101/101 before publication review.
当前验收：日光/暗景无明显问题；用户确认逆光修正候选，生产 PNG 与其逐字节一致。
更广影调观感留至中期后，不计验收通过。用户已授权审查、提交、推送、PR 及合并。
下方早期检查点为历史记录；发布前审查前的最新测试为 101/101。


## Scope and baseline / 范围与基线

Started in the isolated worktree at a4130b0370332a2561207008261c7f5759fc93f2,
independently matched to live main; fresh P5 baseline 76/76. The user accepted
ADR-008's curve, state and explicit two-mode rendering. Added linear EV exposure,
RGB/Y histograms, one luminance shoulder, tone-completion state and minimal CLI.
Original camera data and unbounded editing path remain unchanged. No GUI, extra
camera/space support, reconstruction, parallel optimization or new dependency.
独立工作区从上述提交启动，已独立核对实时 main，新建 P5 基线 76/76。用户接受
ADR-008 曲线、状态及显式双模式输出。增加线性 EV、RGB/Y 直方图、单一亮度肩部、
影调完成状态及最小 CLI，保留原相机数据与未限幅编辑路径。不加 GUI、额外机型/
空间、高光重建、并行优化或新依赖。

Prior-turn preparation documents were preserved and advanced. No outer worktree or
old learning notes were modified. Study was user-confirmed; no exercise record was
invented. No commit, push, remote CI or merge was performed.
保留并推进上一轮准备文档；未修改外层工作区或旧学习笔记。学习由用户确认，不补造
习题记录；未提交、推送、运行远端 CI 或合并。

## Automated evidence / 自动化证据

- Exposure, histogram, state and tone tests first failed on missing APIs/state;
  render tests failed on the absent coordinator. CLI argument test failed on
  missing processing help. Incremental suites: 79,83,85,90,93,94,96 tests passed.
- Final Debug and ASan/UBSan: 99/99 each. New directories build/p6-debug and
  build/p6-sanitize; sanitizer flags -fsanitize=address,undefined and
  -fno-omit-frame-pointer; ASAN_OPTIONS=detect_leaks=0, UBSAN_OPTIONS=halt_on_error=1.
- Separate Release BUILD_TESTING=OFF configure/build and --version passed.
- New production sources pass -Wall -Wextra -Wpedantic syntax checks.
- CMake 4.3.4, Apple Clang 21.0.0, LibRaw 0.22.1, libpng 1.6.58, zlib 1.2.12,
  cached GoogleTest 1.17.0. Existing duplicate -lc++ linker warnings remain.
- git diff --check passed; private outputs and build evidence remain ignored.

- 曝光、统计、状态、影调先因 API/状态缺失失败；输出分支因协调器缺失失败；CLI 参数
  测试先因缺少处理帮助失败。各增量全套通过数量如上。
- 最终新目录 Debug、ASan/UBSan 各 99/99；sanitizer 参数如上，未跑 Apple 泄漏检测。
- 独立 Release 关闭测试构建及版本 smoke 通过，新生产源码严格告警检查无诊断。
- 工具/库版本如上，仍有既有重复 -lc++ 链接警告；diff 检查通过，私有输出与证据忽略。

Coverage includes analytic signed/high values, EV and strength endpoints, invalid
parameters/states, all-component NaN/Inf, arithmetic overflow, representational
underflow/signed zero, independent ownership, odd/padded layouts, generated exposure
composition/inverse and Y monotonicity, channel ratios, exact histogram edges,
conservation and finite extremes. A separate test-only CLI executable substitutes
only decode_raw with synthetic decoded data; production has no test input switch.
It exercises argument-to-PNG downstream flow for both modes and EV/tone settings,
independent chunk/CRC/zlib/sample checks and nonoverwrite. Missing WB/matrix and
decode errors create no output. Actual LibRaw file decoding is verified below.
覆盖解析带符号/高值、EV/强度端点、非法参数/状态、各分量 NaN/Inf、溢出、表示下溢/
负零、独立所有权、奇数/padding、生成曝光组合/逆变换、Y 单调、通道比例、精确分箱
边界、计数守恒及有限极值。单独测试 CLI 可执行文件仅替换 decode_raw 为合成已解码
数据，生产无测试开关；覆盖参数到 PNG 的下游全流程、两模式/曝光/影调、独立
chunk/CRC/zlib/像素验证及不覆盖。缺失 WB/矩阵和解码错误不创建输出；真实解码另验。

Logs and local harness: ignored build/p6-baseline/*-{red,build,test}.log and
build/p6-evidence/{debug,sanitize,release}-*.log, warnings.log, real_verify.cpp.
No old binary following a failed build was counted as passing evidence.
日志与本地 harness 位于上述忽略目录；失败构建后的旧二进制未计作通过证据。

## Real samples / 真实样张

Three authorized originals were read in place, never copied or uploaded. Full
7028x4688 processing used EV {-2,0,+2}, strength {0,1}, both highlight modes:
36 combinations. Every output component was compared with an independently
computed long-double matrix/exposure/tone reference; every component passed
1e-5 + 1e-4*abs(reference). All stage/channel histogram totals equaled pixel count.
Camera fingerprints and pre/post-encoding render fingerprints matched. Neutral
camera-clip PNG sample buffers matched P5 byte-for-byte across all three images.
三张授权原片仅原位读取、不复制或上传；完整 7028x4688 图像采用上述 EV、强度及
两模式，共 36 组。每个输出分量与独立 long-double 矩阵/曝光/影调参考比较，全部
满足组合容差；所有阶段/通道统计总数等于像素数。相机数据指纹及编码前后渲染图
指纹一致；中性 camera-clip 的 PNG 像素 buffer 在三图中均与 P5 逐字节一致。

| Scene / 场景 | Cases / 组合 | Largest absolute error / 最大绝对误差 |
|---|---:|---:|
| Daylight / 日光 | 12/12 | 2.38416292042e-7 |
| Backlit / 逆光 | 12/12 | 4.76836904184e-7 |
| Dark / 暗景 | 12/12 | 4.76671488059e-7 |

The real production CLI additionally wrote three full-size PNGs: daylight
camera-clip/EV0/s0, backlit preserve/EV-2/s1, dark camera-clip/EV+2/s1. An independent
Python struct/zlib reader checked chunk CRCs, RGB16/IHDR, sRGB intent and filter-none
rows, then matched every preview sample to its corresponding full-image pixel.
This confirms CLI/harness agreement without treating thumbnails as numeric proof.
真实生产 CLI 另写三张全尺寸 PNG，参数如上。独立 Python struct/zlib 读取器检查
CRC、RGB16/IHDR、sRGB intent 及 filter-none 行，再逐一核对预览采样与全尺寸对应
像素，确认 CLI/harness 一致；不将缩略图当作数值证明。

Private evidence: build/p6-evidence/{daylight,backlit,dark}/ contains numeric.txt,
12 diagnostic previews, comparison.jpg, cli-full.png and cli.txt. Previews select
source pixels at 1200px width, comparison sheets scale those previews; they are
not full-resolution artifact or calibrated-color acceptance. No performance claim.
私有证据位于上述目录，包括数值表、12 张诊断预览、对照图、完整 CLI PNG 及日志。
预览以 1200 宽采样，对照图再缩放；不是全尺寸伪影或标定色彩验收，不作性能声明。

## Agent visual inspection / Agent 视觉检查

Viewed all 36 thumbnails in three 4-column by 3-row contact sheets. Rows are EV
-2,0,+2; columns preserve/s0, camera-clip/s0, preserve/s1, camera-clip/s1.
Daylight and dark scenes respond consistently to EV; stronger tone reduces bright
regions while shadows stay similar. Bright daylight camera-clip/s1 can leave a
flat gray sky. Backlit preserve retains an obvious magenta solar core/reflection;
at EV-2 camera-clip also reveals it, as predicted. At EV0/s0 camera-clip restores
P5's white core. With strength1, clipped cores turn gray with conspicuous colored
transitions; EV+2 makes the flat region larger. These are visible limits of the
accepted hard-clip/shoulder model, not evidence of highlight reconstruction.
已查看三张四列三行对照图中的全部 36 张缩略图，行列含义如上。日光及暗景随 EV
一致变化，强影调压低亮区、暗部变化较小；高曝光日光 camera-clip/s1 可形成平灰
天空。逆光 preserve 保留明显粉紫太阳核心/反光，EV-2 的 camera-clip 也如预期重现；
EV0/s0 的 camera-clip 恢复 P5 白色核心。强度 1 会使裁剪核心变灰并形成明显彩色
过渡，EV+2 时平坦区域更大。这是已接受硬裁剪/肩部模型的可见限制，不是高光重建。

User P6 visual acceptance remains OPEN. P5 coarse acceptance does not carry over.
Improving these effects would require a separately reviewed rendering algorithm;
this implementation does not silently alter the accepted formula to hide them.
用户 P6 视觉验收仍未完成，不继承 P5 粗验收。改善上述效果需另行审阅渲染算法，
本实现未为掩盖现象而静默更改已确认公式。

## Code and differential review / 代码及差异审查

Focused self-review against a4130b0 covers all changed/new production, tests and
CMake files, with tests read first. Existing validation history in eba0f64,
a30f888 and 4829feb was inspected. The new enum is appended, preserving previous
values; state checks extend accepted inputs only to explicitly tagged tone-mapped
float sRGB. Raw/Bayer/camera, range, stride, overflow and encoded-state guards stay.
以 a4130b0 为基线，先看测试，定向自审所有变更/新增生产、测试及 CMake 文件；已读
上述提交的校验历史。新枚举追加、旧值不变；只增量接受明确标记的影调后 float
sRGB。原 RAW/Bayer/相机、范围、行距、溢出及编码状态校验保持。

Risk is medium for public numeric APIs/state extension and app orchestration, low
for docs/test-support movement. Runtime production call sites: apply_exposure 1,
apply_tone 2, compute_histogram 4, render_camera 1, encode_srgb16 1; image construction
validation affects the existing core globally. Reviewed ownership, finite arithmetic,
per-row bounds, no per-pixel allocation, state order and branch bypass. Attack-shaped
cases include NaN/huge EV, encoded input, extreme signed pixels, duplicate/missing
CLI options and existing output paths; tests reject or handle these explicitly.
公开数值 API/状态扩展及应用编排为中风险，文档/测试辅助提取为低风险。生产调用点
数量如上；图像构造校验影响现有核心。审查所有权、有限算术、行边界、无逐像素分配、
状态次序与分支绕过；NaN/巨大 EV、编码输入、极端带符号像素、重复/缺失参数及
已有输出路径均有显式处理或拒绝测试。

Unresolved code findings: critical 0, high 0, medium 0, low 0 in this self-review.
No removed security guard or new dependency. The old independent PNG test decoder
was moved verbatim (inline linkage/formatting only) into a shared test header;
its original tests still run. No measured coverage percentage or independent
reviewer is claimed. Recommendation: ready for user visual review, not full stage
acceptance or publication. No full LibRaw/untrusted-file security audit, Windows,
Linux remote CI, LSan, calibrated/full-resolution acceptance or benchmark coverage.
本次自审未发现待修代码问题（各级 0），未删除安全校验或增加依赖。原独立 PNG 测试
解码器仅提取到共享测试头（inline/格式变化），原测试继续运行。不宣称覆盖率百分比
或独立审查。结论为可交用户视觉审阅，不等于阶段完全验收或发布；不覆盖完整 LibRaw/
不可信文件安全审计、Windows、远端 Linux CI、LSan、标定/全尺寸验收及 benchmark。

## Reproduction / 复现

Use the configure command in [P6 contract](../docs/p6-design-preparation.md), changing
-B to the desired fresh build directory. On this Mac use the documented developer
path and cached GoogleTest source. Then build and run CTest. For sanitizer add
-DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' and
-DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined', and the runtime options above.
For no-tests use -DBUILD_TESTING=OFF. CLI syntax is in [cli.md](../docs/cli.md).
用 P6 契约的配置命令，将 -B 改为所需新目录；本机采用已记录的开发者路径与
GoogleTest 源码缓存，然后构建及运行 CTest。sanitizer 增加上述编译/链接及运行
参数，关闭测试用 -DBUILD_TESTING=OFF；CLI 语法见文档。

## User feedback follow-up / 用户反馈跟进 — 2026-09-25

Daylight/dark: no obvious problems, user-confirmed. Backlit magenta remains a
required integration correction; broader tone appearance is deferred until after
the middle stage, not accepted. The prior numeric passes remain valid for the
implemented formula but do not close this regression. See [root-cause check](p6-highlight-regression-check.md).
用户确认日光/暗景无明显问题；逆光粉紫为需修正衔接问题，更广影调观感留到中期后，
不计通过。原数值通过仍证明当时公式实现一致，不关闭此回归；见根因检查。

## Approved correction verification / 已确认修正验证 — 2026-09-25

The user approved implementation of the right-column candidate. Production now
uses T(g*M*min(C,1)) for camera-clip. Normal preview documentation recommends
camera-clip; preserve remains uncorrected diagnostic output. Original editing
buffers and tone formula are unchanged. ADR-009 supersedes the former order.
用户批准按右列候选实现。生产 camera-clip 现使用上述公式；普通预览推荐此模式，
preserve 明确为未修正诊断。原编辑数据及影调公式不变；ADR-009 替代旧顺序。

- Negative-EV saturated-core regression failed before the fix, then passed.
- Debug and ASan/UBSan each pass 101/101; Release BUILD_TESTING=OFF builds and
  --version succeeds. Leak detection remains disabled; Linux CI was not run.
- Three authorized RAWs read in place: all 36 full-frame oracle cases pass,
  tolerance 1e-5+1e-4*abs(reference), including source fingerprints, histogram
  conservation and EV0/tone0 byte-exact P5 encoding. Tone-off output also equals
  the exposed P5 linear baseline at every component for EV -2/0/+2.
- Backlit tone-off PNG files at all three EVs are byte-identical to the approved
  candidate. EV-2 core is (0.25,0.249999985099,0.25). Agent inspected all three
  scene sheets; corrected backlit core/reflection matches the approved output.
- Tests-first self-review found no remaining required finding in this correction:
  source ownership, order, overflow, stride and labeled histogram semantics checked.
  Replaced obsolete camera-intermediate overflow test with working overflow guard
  plus valid zero-matrix case; no skipped tests or tolerance relaxation.

- 负 EV 饱和核心回归修正前失败，修正后通过。
- Debug 与 ASan/UBSan 各 101/101；Release 关闭测试构建及 --version 通过。
  泄漏检测仍关闭，未运行 Linux CI。
- 三张授权 RAW 原位读取：36 个全图参考案例全部通过，容差如上，包含源指纹、直方图
  守恒、EV0/影调0 与 P5 编码逐字节一致；三个 EV 下影调关闭输出每分量均等于 P5
  线性基准乘曝光增益。
- 三个 EV 的逆光影调关闭 PNG 与已确认候选文件逐字节相同；EV-2 核心数值如上。
  Agent 查看三场景对照，逆光核心及反光与已确认结果一致。
- 先测试后实现的自审未发现本修正仍需处理的问题；检查源所有权、顺序、溢出、步幅
  及统计标注。旧相机中间态溢出测试改为工作路径溢出保护及合法零矩阵案例，无跳过
  测试或放宽容差。

Evidence: ignored build/p6-highlight-check/{red,green,sanitize,release-build}.log
and build/p6-highlight-fixed/{real_verify.cpp,daylight,backlit,dark}. Previous
outputs retained. Broader tone appearance remains deferred until after the middle
stage, not accepted. No commit or push; no independent reviewer or benchmark claim.
证据在上述忽略目录，旧输出保留。更广影调观感继续留至中期后，不计通过。
未提交或推送，未声称独立审查或性能验证。

## Publication review / 发布前审查

User authorized all Git operations through merge. Full P6 review covered tests
first, then metadata/encoder compatibility, exposure/tone/histogram arithmetic,
render ordering, CLI parsing and labeled diagnostics, PNG test-helper extraction,
CMake integration and documentation. No unresolved critical or required finding.
用户授权全部 Git 操作直到合并。完整 P6 审查先看测试，再检查元数据/编码兼容、
曝光/影调/直方图算术、渲染顺序、CLI 解析与统计标注、PNG 测试辅助提取、CMake
集成及文档；无未解决严重或必改项。

One coverage gap was addressed: the original CLI synthetic fixture stayed below
camera saturation. Added a saturated fixture and an independent PNG assertion for
camera-clip at EV-2, proving CLI selection reaches the corrected branch. The test
failed before fixture support and passes afterwards. Production code is unchanged.
Latest Debug and ASan/UBSan each pass 102/102. Existing release and 36-case real
sample evidence remains applicable. No tolerance weakened or tests skipped.
补齐一项覆盖缺口：旧 CLI 合成数据未饱和。增加饱和输入及 EV-2 camera-clip 独立
PNG 断言，验证 CLI 实际选择修正分支。补齐 fixture 前失败，之后通过；生产代码
不变。最新 Debug、ASan/UBSan 各 102/102；原 Release 与 36 案例样张证据仍适用。
未放宽容差或跳过测试。

Public boundary inspection found only intended source, tests, build configuration
and bilingual documents. Private RAW, generated images, build artifacts and
learning notes remain ignored. This is self-review, not an independent review or
security audit. Broader tone appearance remains deferred; Linux CI is a merge gate.
公开边界检查仅包含预期源码、测试、构建配置及双语文档；私人 RAW、生成图像、
构建产物与学习笔记保持忽略。本次为自审，不声称独立审查或安全审计。更广影调
观感继续延期，Linux CI 为合并条件。

## Remote verification / 远端验证

[PR #3](https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw/pull/3) publishes
P6. Both push CI (run 36152429142) and PR CI (run 36152482403) passed on
3968c95; Linux logs confirm 102/102. Review submissions and unresolved threads
were empty at the pre-merge check. The final documentation-only head must also
pass CI before merging; the PR is the authoritative record of the merge result.
P6 通过 PR #3 发布。3968c95 的 push 与 PR 两次 CI 均成功，运行编号如上，Linux
日志确认 102/102。合并前检查无审查提交或未解决讨论。最终仅文档提交也须通过 CI
后合并；最终合并结果以 PR 记录为准。
