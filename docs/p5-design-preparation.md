# P5 Design Preparation / P5 设计准备

Status / 状态：Historical preparation; implemented contract now in ADR-007 and module specs / 历史准备记录，当前实施契约见 ADR-007 及模块规格。
Date / 日期：2026-09-19

Learning completed, user-confirmed on 2026-09-21. Design/dependency approval is
still pending. Preserve P3/P4 acceptance; do not copy private learning notes.
用户于 2026-09-21 确认 P5 学习完成。设计/依赖仍待确认，保留 P3/P4 验收，不复制私人学习笔记。

## Capability map / 能力划分

| Module id | Responsibility / 职责 | Depends on / 依赖 |
|---|---|---|
| color-transform | Camera RGB to linear sRGB, no clipping / 相机 RGB 转线性 sRGB，不裁剪 | P4 |
| display-encode | sRGB output encoding on an independent output / 独立输出上的 sRGB 输出编码 | color-transform state / 色彩转换状态 |
| png-output | RGB16 PNG with sRGB identification / 带 sRGB 标识的 RGB16 PNG | display-encode state / 显示编码状态 |

Review this map and choices before module contracts and ADR-007, then plan TDD
slices. Each module is independently testable using synthetic images. Proposed file
stems: color_transform, display_encode, png_writer in existing include/src/tests.
先审阅能力划分和选择，再写模块契约及 ADR-007，之后规划 TDD 增量。各模块均用合成
图像独立测试。候选文件名如上，沿用 include/src/tests 目录。

## Color evidence and proposed boundary / 色彩依据与边界提案

P3 requires idata.colors == 3 and RGBG indexing. P4 returns packed float32 RGB,
CFA None, with caller-managed WB already applied. The LibRaw maintainer identifies
rgb_cam as camera to linear sRGB, D65, with WB applied before conversion [1].
Versions 0.21.4 and 0.22.1 use columns 0..2 in their three-color conversion loop;
the fourth column participates in the four-color branch [2,3]. This does not
justify summing green columns or reconstructing separate G1/G2 from P4 RGB.

P3 限制三颜色和 RGBG 编号；P4 在调用方完成白平衡后输出紧密 float32 RGB、CFA None。
LibRaw 维护者说明 rgb_cam 转到 D65 线性 sRGB，白平衡提前应用 [1]。0.21.4 与
0.22.1 的三颜色循环使用第 0..2 列，四颜色分支才使用第四列 [2,3]。不能据此相加
两个绿色列或从 P4 RGB 恢复独立 G1/G2。

Recommend explicit row-major double 3x3 coefficients; double dot products before
float output, preserving negative/above-one values. Keep P4's green-mean gain scale;
do not apply cam_mul/pre_mul twice, invert the matrix or normalize its rows. For
metadata adaptation, conservatively require present finite coefficients and zero
fourth column before extracting 3x3. Reject unexpected fourth-column content for
investigation. This stricter policy is a project proposal, not a LibRaw requirement.
On 2026-09-21, all three authorized samples were decoded in place with LibRaw
0.22.1: WB and matrices were present, matrices identical, fourth columns exactly zero.
This supports the proposed boundary for these samples only. No calibrated color-accuracy claim is made.

建议显式逐行 double 3x3 系数，double 点积后转 float，保留负值和超一值。保留 P4
绿色均值尺度，不重复应用 cam_mul/pre_mul、不求逆或重归一矩阵行。元数据适配建议
保守要求矩阵存在、有限且第四列为零，再提取 3x3；非零时拒绝并调查。这是更严格的
项目提案，不是 LibRaw 要求。2026-09-21 已使用 LibRaw 0.22.1 原位解码三张授权样张：均存在白平衡和矩阵，
矩阵相同且第四列精确为零。该结果仅支持这些样张的拟定边界。
不宣称标定色彩准确性。

## API and numeric candidates / API 与数值候选

- Keep processing state, color-space identity and transfer encoding distinct.
  Existing kLinearWorkingRgb does not name primaries. Following the user's extension
  requirement, prefer additive color-space metadata with explicit unknown handling;
  do not reinterpret existing working images as sRGB. Finalize compatible validation
  rules before modifying ImageBuffer; implement only sRGB in P5.
- Require immutable float RGB inputs with CFA None, respect stride, ignore padding
  including NaN padding, return independently owned packed output.
- Display: reject nonfinite active samples, explicitly clamp x to [0,1], then use
  12.92*x for x <= 0.0031308, otherwise 1.055*x^(1/2.4)-0.055 [4]. Quantize once:
  floor(65535*encoded+0.5). No exposure, tone curve, automatic brightness or dithering.
- Retain invalid_argument for invalid inputs, overflow_error for arithmetic,
  length_error for allocation-size overflow, runtime_error for I/O. No partial result.

- 处理状态、色彩空间身份与传递编码分别表达；现有 kLinearWorkingRgb 未命名原色。
  根据用户扩展要求，改为建议增量色彩空间元数据并明确未知值处理，不将已有工作图像
  重解释为 sRGB。修改 ImageBuffer 前确定兼容校验规则，P5 仅实现 sRGB。
- 要求不修改 float RGB 输入、CFA None、遵守 stride，忽略含 NaN 的 padding，输出
  独立拥有的紧密存储。
- 显示阶段先拒绝非有限有效像素，再显式裁剪到 [0,1]，使用上述标准分段公式 [4]，
  按上述公式仅量化一次；不加曝光、影调、自动提亮或抖动。
- 沿用上述异常分类，不返回部分结果。

## Output choice / 输出选择

Recommend RGB16 PNG, no alpha, sRGB chunk, explicit rounding and byte order [5].
Use a private libpng adapter; local pkg-config reports 1.6.58. No private EXIF or
source path in output; document unchanged orientation. The standard library cannot
encode PNG. libpng avoids a custom compression/checksum/format writer. TIFF/libtiff
is an alternative for future float output, outside this display-output scope.

建议 RGB16 PNG、无 alpha、sRGB chunk、明确舍入和字节序 [5]。采用私有 libpng
适配层，本机 pkg-config 报告 1.6.58；不写私人 EXIF/原路径，方向保持不变并记录。
标准库不提供 PNG 编码，libpng 可避免自写压缩/校验和/格式处理。未来浮点输出可
考虑 TIFF/libtiff，本轮显示输出不包含此需求。

Dependency approval remains required. Complete license, vulnerability, exact API,
error-lifetime and macOS/Linux/CI discovery review before adding it. No dependency
was installed or changed. Writer ownership, failure cleanup and overwrite policy
must be specified before implementation.

新增依赖仍需确认；加入前完成许可证、漏洞、精确 API、错误生命周期及跨平台/CI
发现审查。本轮未安装或改变依赖。写入器所有权、失败清理和覆盖策略须在实现前明确。

## Acceptance outline / 验收提纲

Color: identity, nonsymmetric matrix direction, basis vectors, negative/above-one
preservation, missing/nonfinite metadata, fourth-column policy, overflow, 1x1,
odd/padded layouts, immutability and independent deterministic reference.
Display: endpoints, threshold and adjacent values, clipping, NaN/Inf rejection,
monotonicity, repeated-encoding rejection, rounding half-steps.
PNG: independent decode of exact integer pixels, dimensions, bit depth, channel
order and sRGB chunk; failure cleanup and overwrite behavior. Do not require
identical compressed bytes across encoder versions.

色彩：恒等、非对称矩阵方向、基向量、范围外保留、缺失/非有限元数据、第四列策略、
溢出、1x1/奇数/padding、不修改输入及独立确定性参考。显示：端点、分段点及邻值、
裁剪、非有限拒绝、单调性、重复编码拒绝和半步舍入。PNG：独立解码验证精确整数
像素、尺寸、位深、通道和 sRGB 标识，验证失败清理和覆盖；不跨版本要求压缩字节相同。

Start scalar tolerances at 1e-6 absolute + 1e-6 relative and composed-image bounds
at 1e-5 + 1e-4 relative; integer round trips exact. Later require Debug, ASan/UBSan,
no-tests build, authorized sample numeric checks (max error/RMSE), and separate user
visual acceptance. No benchmark or color-accuracy claims without measurements.

标量初始容差 1e-6 绝对加 1e-6 相对，组合图像 1e-5 加 1e-4；整数往返精确。
后续需 Debug、ASan/UBSan、关闭测试构建、授权样张数值检查（最大误差/RMSE）及
独立用户视觉验收。没有测量不宣称性能或色彩精度。

## Sources / 来源

1. [LibRaw matrix and WB](https://www.libraw.org/node/2184)
2. [LibRaw 0.22.1 conversion](https://github.com/LibRaw/LibRaw/blob/0.22.1/src/postprocessing/postprocessing_utils.cpp#L73-L136)
3. [LibRaw 0.21.4 conversion](https://github.com/LibRaw/LibRaw/blob/0.21.4/src/postprocessing/postprocessing_utils.cpp#L73-L136)
4. [ICC sRGB registry](https://registry.color.org/rgb-registry/srgb)
5. [W3C PNG specification](https://www.w3.org/TR/png-3/)


## Review update / 审阅更新 — 2026-09-21

[ADR-007 proposal](decisions/ADR-007-p5-color-and-output.md) collects three choices
for review. Learning completion is user-confirmed; no private learning record was
copied or independently graded here. The metadata probe and its output stay in
ignored build/p5-baseline. It reused the unchanged existing decoder, not P5 code.

ADR-007 提案汇总三项待审选择。学习完成依据用户确认，本任务未复制或独立评分学习
记录。元数据探针及输出位于忽略的 build/p5-baseline，调用未修改的现有解码器。

### Dependency assessment / 依赖评估

Propose installed libpng 1.6.x and transitive zlib, with a patched maintained package.
Local libpng is 1.6.58. Its PNG Reference Library License v2 permits use/modification/
distribution subject to origin, change-marking and notice rules; retain attribution
in dependency documentation. No third-party source is vendored. Local library files
are about 203 KiB dynamic / 246 KiB static, excluding zlib; these are installed file
sizes, not application-size or performance measurements.

建议系统安装的 libpng 1.6.x 及传递依赖 zlib，使用持续维护并带安全补丁的包。本机
为 1.6.58。PNG Reference Library License v2 允许使用/修改/分发，须遵守来源、
修改标识与通知保留要求；依赖文档保留归属，不复制第三方源码。本机库文件约动态
203 KiB、静态 246 KiB（不含 zlib），仅为文件大小，不是应用增量或性能测量。

Use [CMake FindPNG](https://cmake.org/cmake/help/v3.24/module/FindPNG.html) and
PNG::PNG; macOS uses the installed package, Linux CI would add libpng-dev. Record
resolved PNG/zlib versions and check distribution security backports before accepting
the actual CI package; that remote environment has not been tested in this task.

使用上述 CMake FindPNG 和 PNG::PNG；macOS 使用已安装包，Linux CI 拟增加 libpng-dev。
记录实际 PNG/zlib 版本，并在接受 CI 包前核对发行版安全补丁；本任务未测试远端环境。

The [official advisory index](https://github.com/pnggroup/libpng/security/advisories)
and local 1.6.58 changelog were inspected. The simplified-write stride issue
[GHSA-vgjq-8cw5-ggw8](https://github.com/pnggroup/libpng/security/advisories/GHSA-vgjq-8cw5-ggw8)
was patched in 1.6.54; setter aliasing
[GHSA-6fr7-g8h7-v645](https://github.com/pnggroup/libpng/security/advisories/GHSA-6fr7-g8h7-v645)
in 1.6.57. Local 1.6.58 includes these fixes. The later
[APNG advisory](https://github.com/pnggroup/libpng/security/advisories/GHSA-c4v6-gxrq-6g2x)
concerns patched APNG/push parsing, not upstream 1.6 sequential PNG writing.
This is a focused dependency check, not a guarantee of absence of vulnerabilities.

已检查官方公告索引和本机变更记录：简化写入 stride 问题在 1.6.54 修复，setter
别名问题在 1.6.57 修复，本机 1.6.58 含这些修复。较新的 APNG 公告针对 APNG 补丁/
push 解析，不是上游 1.6 顺序 PNG 写入。此为定向依赖检查，不保证不存在漏洞。

The installed 1.6.58 png.h and man page show simplified 16-bit writing treats input
as linear. Use the conventional API for already sRGB-encoded RGB16: set IHDR to RGB,
16-bit, noninterlaced; set sRGB intent relative colorimetric; supply explicit
big-endian sample bytes with no gamma transform. Isolate setjmp/longjmp in a private
POD-only helper, with no C++ destructors crossed or exceptions through C callbacks;
perform C++ ownership and error translation outside that helper. Test injected write
failures and cleanup. API reference: [versioned png.h](https://github.com/pnggroup/libpng/blob/v1.6.58/png.h).

本机 1.6.58 头文件和手册说明简化 API 将 16 位输入视为线性。已编码 RGB16 应使用
常规 API：IHDR 为 RGB/16 位/无交错，sRGB intent 为相对比色，提供显式大端字节，
不做 gamma 变换。setjmp/longjmp 限于私有 POD helper，不跨越 C++ 析构、不让异常
穿过 C 回调，所有权及异常转换留在外层。须测试注入写入失败及清理。

Threat boundary: caller-supplied dimensions/state/path enter the writer. Validate
sizes before allocation/narrowing, expose no PNG reader in production, do not copy
EXIF or private path strings into chunks. Proposed path policy: refuse an existing
target using exclusive creation, delete only a partial file created by this call
on failure. No atomic publication guarantee in the first writer; a crash can leave
an incomplete new file. This is appropriate for a local CLI/library baseline and
must be documented and tested rather than implied transactional.

信任边界是调用方提供的尺寸/状态/路径。分配及窄化前校验尺寸，生产 API 不提供
PNG 读取，不复制 EXIF 或私人路径。建议排他创建并拒绝覆盖已有目标；失败只清理
本次创建的部分文件。首版不保证原子发布，进程崩溃可能留下不完整新文件；对此
本地 CLI/库基线应明确说明并测试，不能暗示事务保证。


### User-confirmed flow and extension / 用户确认流程与扩展 — 2026-09-21

Use sRGB first while preserving future color-space options. Working-space identity,
output-space selection and transfer encoding must not be conflated. Output conversion
and clipping never mutate the linear working image. P5 goes directly from linear
sRGB to an independently clipped/encoded output; future P6 processing precedes that
output branch. Future other-space output needs explicit conversion, gamut policy,
transfer encoding and matching file identification, not relabeling or a universal
sRGB curve. See ADR-007 for the revised model direction. Dependency approval remains
separate. Add acceptance cases for working-image preservation, unknown/mismatched
color identity rejection, and writer refusal to tag unsupported spaces as sRGB.

先使用 sRGB，同时保留未来色彩空间选项。工作空间身份、输出空间选择和传递编码
不可混为一谈；输出转换和裁剪不修改线性工作图像。P5 从线性 sRGB 直接生成独立
裁剪/编码输出，未来 P6 位于该输出分支之前。其他空间须明确变换、色域策略、
传递编码及对应文件标识，不靠改标签或通用套用 sRGB 曲线。模型方向更新见 ADR-007，
依赖确认保持独立。补充验收：工作图像保持不变、未知/不匹配色彩身份拒绝、写入器
拒绝把不支持的空间标记为 sRGB。
