# P4 White Balance and Bilinear Demosaic / 白平衡与双线性去马赛克

Status / 状态：Accepted for implementation on 2026-09-19 / 2026-09-19 已确认实施契约。

Date / 日期：2026-09-18

## Scope / 范围

Prepare two scalar C++17 capabilities in roadmap order: white balance maps normalized
Bayer to gain-adjusted Bayer; bilinear demosaic maps it to camera-linear RGB. Both
are independently testable with synthetic input. The user confirmed learning completion and accepted all three design decisions on
2026-09-19; implementation is authorized. Learning stays in a separate task; no private
notes are copied. No P5, automatic WB, new dependency, optimization or publication.

按路线图顺序准备两个标量 C++17 能力：白平衡将归一化 Bayer 转为增益校正后的 Bayer，
双线性去马赛克将其转为相机线性 RGB。两者均可用合成输入独立测试。用户于 2026-09-19 确认学习完成并接受三项设计，授权实施；学习在独立任务进行，不复制私人笔记。不包含 P5、自动
白平衡、新依赖、优化或发布。

## API and State / API 与状态

The following signatures define the accepted API. Headers belong under
`include/mini_camera_raw/`, definitions under `src/`, and GoogleTest cases under
`tests/`: `white_balance.h/.cpp`, `demosaic.h`, `demosaic_bilinear.cpp`,
`white_balance_test.cpp`, `demosaic_test.cpp`. Follow existing snake_case functions,
PascalCase types and standard exceptions.

以下签名定义已接受的 API。公开头、实现、GoogleTest 测试分别放在上述目录
及文件，沿用 snake_case 函数、PascalCase 类型及标准异常。

```cpp
struct WhiteBalanceGains { std::array<double, 4> tile; };
WhiteBalanceGains normalize_camera_wb(
    const std::array<double, 4>& camera_tile, CfaPattern cfa);
ImageBuffer apply_white_balance(const ImageBuffer& linear,
                               const WhiteBalanceGains& gains);
ImageBuffer demosaic_bilinear(const ImageBuffer& linear);
```

Both stages require float32 single-channel LinearBayer with supported CFA. Inputs
are immutable; results own a new packed allocation. Respect input element stride;
padding, including nonfinite padding, is ignored. WB preserves dimensions and CFA.
Demosaic preserves dimensions and returns interleaved R,G,B, CFA=None,
LinearCameraRgb. Both declare nominal [0,1], allowing out-of-range values. Sensor
metadata and orientation remain with the caller; no rotation/crop is performed.
Current ColorState does not distinguish pre/post WB: applying gains exactly once is
an explicit caller obligation, not a type-enforced guarantee.

两阶段要求 float32 单通道 LinearBayer 及支持的 CFA。输入不修改，输出拥有独立紧密
存储。遵守输入元素 stride，忽略包括非有限值在内的 padding。白平衡保持尺寸和 CFA；
去马赛克保持尺寸，输出交错 R,G,B、CFA=None、LinearCameraRgb。两者名义范围均为
[0,1] 且允许超范围。传感器元数据及方向由调用方保留，不旋转或裁剪。现有 ColorState
不能区分白平衡前后，调用方保证只应用一次增益，不能宣称类型已强制此顺序。

## CFA and Gains / CFA 与增益

Spatial index `p=2*(y%2)+(x%2)` is relative to the active-image origin. This is not
LibRaw channel indexing; P3 already maps camera_wb_tile into spatial order.
空间索引相对于有效图像原点，公式如上；它不是 LibRaw 通道编号，P3 已将
camera_wb_tile 映射为空间顺序。

| CFA | 00 | 01 | 10 | 11 |
|---|---|---|---|---|
| RGGB | R | G | G | B |
| BGGR | B | G | G | R |
| GRBG | G | R | B | G |
| GBRG | G | B | R | G |

All four gains must be positive finite doubles, even for tiny images. Manual gains
are applied literally. For camera metadata, use an explicit helper: let a,b be
the two green positions; `gref=(m[a]+m[b])/2`, `gain[p]=m[p]/gref`.
Evaluate the mean as `lo+(hi-lo)/2` for positive finite inputs to avoid
overflow of the sum or premature underflow from halving tiny equal greens. Validate finite
positive reference and ratios. Preserve unequal green gains. This common scale
preserves all gain ratios while setting mean green gain to one. It is a project
exposure convention, not a LibRaw guarantee. Missing camera WB requires a caller
error or explicit manual gains; no silent identity/daylight fallback.

四个增益均须为有限正 double，即使输入很小也全部校验。手动增益按原值应用。
相机元数据采用独立显式 helper：a,b 为两个绿色位置，按上述公式归一，校验参考值
及各比值为有限正值。均值以 `lo+(hi-lo)/2` 计算，避免相加溢出或两个极小绿色
先除以二导致下溢。保留两个绿色增益的差异。统一缩放保持所有增益比例，使绿色
增益均值为一。这是项目曝光约定，并非 LibRaw 保证。相机白平衡缺失时由调用方报错
或明确提供手动增益，不静默使用单位/日光增益。

WB computes `out[y,x]=double(in[y,x])*gain[p]`, then converts to float. Preserve
negative and above-one results without clipping. Support all positive dimensions,
including 1x1. Unit gains are an explicit diagnostic choice.

白平衡按上述公式计算后转 float，保留负值和超一值，不裁剪。支持全部正尺寸，包括
1x1。单位增益可作为显式诊断选择。

## Bilinear Rule and Border / 双线性规则与边界

Keep the measured channel exactly. Average same-color neighbors for missing channels:
G at R/B uses axial offsets; B at R or R at B uses diagonal offsets; R/B at G uses
the horizontal or vertical pair selected by CFA. Use double sums, divide by neighbor
count, then convert to float. No cross-channel correction or edge-directed selection.

精确保留原测量通道。缺失通道按对应颜色邻居求均值：R/B 位置的 G 用上下左右，
R 位置的 B 或 B 位置的 R 用对角，G 位置的 R/B 按 CFA 选择水平或垂直邻居。
double 累加，除以邻居数后转 float；不加入跨通道校正或方向自适应。

Accepted border: omit out-of-image neighbors and divide by the remaining count.
This project boundary extension is not claimed equivalent to every library's
bilinear border rule. Do not clamp mosaic coordinates, which can change CFA color.
Require width>=2 and height>=2: accept 2x2, 2xN, Nx2 and odd sizes; reject 1xN/Nx1
because some colors are absent. All missing components have neighbors at accepted
sizes. Affine ramps reproduce the interior, not necessarily the border.

已接受的边界：忽略图外邻居，按剩余数量求均值。这是项目边界扩展，不宣称与所有库的
双线性边界一致。不钳制马赛克坐标，以免改变 CFA 颜色。要求宽高均至少为 2，支持
2x2、2xN、Nx2 及奇数尺寸；拒绝缺少某些颜色样本的 1xN/Nx1。支持尺寸的所有缺失
分量均有邻居。仿射渐变只要求内部恢复精确，不要求边缘同样成立。

## Errors and Numerics / 错误与数值

Reject invalid state/CFA, nonfinite active samples, nonpositive/nonfinite gains and
unsupported dimensions with invalid_argument. Reject unrepresentable arithmetic
results with overflow_error before float conversion, including unrepresentable gain
ratios. Checked allocation-size overflow uses length_error; allocation failures
propagate. Return no partial result; inputs remain unchanged on failure. Sample
underflow to zero is allowed; gain underflow to zero is rejected. Use double
intermediates, scalar single-thread traversal, one output allocation per stage,
no per-pixel allocation and no fast-math. No performance claim yet.

非法状态/CFA、非有限有效像素、非正/非有限增益及不支持尺寸抛 invalid_argument。
无法表达的运算结果在转 float 前抛 overflow_error，无法表达的增益比值亦如此。
分配尺寸溢出抛 length_error，分配失败向上传播。不返回部分结果，失败不改输入。
允许像素下溢到零，拒绝增益下溢到零。double 中间值，标量单线程逐行处理，每阶段
一次输出分配，无逐像素分配、不启用 fast-math；暂不宣称性能。

## Acceptance / 验收

Hand vectors / 手算向量：

- WB RGGB input `[-0.25,0.5;0.75,1.25]` with tile `[2,1;1,3]` gives
  `[-0.5,0.5;0.75,3.75]`; camera tile `[4,2;2,6]` normalizes to that gain tile.
  白平衡输入、增益、预期输出及相机增益归一结果如上。
- Demosaic RGGB `[2,4;6,8]` gives row-major RGB `(2,5,8), (2,4,8), (2,6,8),
  (2,5,8)` under the proposed border rule. 2x2 去马赛克按提案边界得到上述逐行 RGB。

Cover four CFA phases, unequal greens, neutral-gain exact identity, exact measured
channel preservation, per-color constant fields, impulses, interior ramps, negative
and above-one values, odd/small sizes, padding invariance, ownership, repeatability,
invalid inputs, each-position NaN/Inf, extreme normalization and product overflow.
Use deterministic generated properties and a structurally independent stencil oracle;
check interpolation channel-range preservation and positive-gain WB monotonicity.
Float formula tolerance is `abs(error)<=1e-6+1e-6*abs(reference)`. Composed-image
comparison uses `1e-5+1e-4*abs(reference)` and reports maximum absolute error and RMSE.

覆盖四相位、不等绿色、单位增益精确恒等、原测量通道精确保留、分通道常量、脉冲、
内部渐变、负值/超一值、奇数/小尺寸、padding 不变量、所有权、确定性、非法输入、
各位置 NaN/Inf、极端归一及乘法溢出。采用确定性生成性质和结构独立的模板参考，
检查插值分通道值域保持及正增益白平衡单调性。公式及组合图像容差如上，图像比较
报告最大绝对误差及 RMSE。

After numeric correctness, read the three authorized originals in place; keep derived
outputs ignored. Record parameters, crop, orientation, viewer, diagnostic mapping and
zippering/false color. Diagnostic display mappings stay outside the production API
and must be labeled: camera RGB is not sRGB. P4 visual review cannot prove P5 color
accuracy. Learning completion and learner hand calculation remain separate gates.

数值正确后原位读取三张授权原片，派生输出保持忽略。记录参数、裁剪、方向、查看器、
诊断映射和拉链纹/伪色。诊断显示映射置于正式 API 外并须标注：相机 RGB 不是 sRGB。
P4 视觉检查不能证明 P5 色彩准确。学习完成及学习者手算仍为独立门槛。

## Verification and Boundaries / 验证与边界

Run from the P4 worktree. Existing GoogleTest 1.17.0 source may be supplied via
`-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/absolute/cache/path`; no version change.
On this Mac prefix build commands with `DEVELOPER_DIR=/Library/Developer/CommandLineTools`.
从 P4 工作区执行，现有 GoogleTest 1.17.0 源码缓存可通过上述变量指定，不修改版本。
本机构建命令加上上述 DEVELOPER_DIR 前缀。

```sh
cmake -S . -B build/p4 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/p4
ctest --test-dir build/p4 --output-on-failure
ctest --test-dir build/p4 -R 'WhiteBalance|Demosaic' --output-on-failure
cmake -S . -B build/p4-no-tests -G Ninja -DBUILD_TESTING=OFF
cmake --build build/p4-no-tests
git diff --check
```

Later implementation also requires full ASan/UBSan, tests-first review and real-sample
numeric/visual checks. Preparation baseline tests only exercise existing P3 code.
Always preserve private boundaries and validate; new scope/dependencies require
user authorization; never silently clip, copy private notes or publish this work.

后续实现还须完成全套 ASan/UBSan、先测试后代码的审查及真实样张数值/视觉检查。
准备期基线仅验证已有 P3 代码。始终验证并保护私有边界，新范围/依赖须用户授权；
不静默裁剪、复制私人笔记或发布本轮内容。

## Sources and Review Points / 来源与审阅点

- [LibRaw data structures](https://www.libraw.org/docs/API-datastruct.html): cam_mul
  is as-shot WB metadata. Installed 0.22.1 and the P3 adapter were inspected.
  cam_mul 为拍摄时白平衡元数据，已核对本机 0.22.1 及 P3 适配层。
- [Getreuer, IPOL 2011, Algorithm](https://www.ipol.im/pub/art/2011/g_mhcd/revisions/2011-08-14/g_mhcd.htm):
  interior bilinear neighbor averages and edge artifacts. No MHC correction or source
  code adopted. 内部双线性邻域及边缘伪影参考，不采用 MHC 校正或源码。
- [CMake 4.3 FetchContent](https://cmake.org/cmake/help/v4.3/module/FetchContent.html):
  existing-source override / 现有源码覆盖方式。

Accepted review points: green-mean exposure convention, omitted-neighbor borders,
minimum size and caller-managed WB ordering. See accepted ADR-006.
已接受的审阅点：绿色均值曝光约定、有效邻居边界、最小尺寸及调用方管理的白平衡顺序。
见已接受的 ADR-006。

Acceptance update / 验收更新：2026-09-19, user confirmed separate learning completed and all design choices accepted. No duplicate lesson is required here; learner hand-calculation evidence remains in the separate learning task and was not copied or independently inspected. / 用户确认独立学习完成并接受全部设计，本任务不重复授课；手算证据保留在独立学习任务，未复制或独立查阅。

Current implementation evidence / 当前实现证据：[P4 verification](../tasks/p4-verification-review.md).
