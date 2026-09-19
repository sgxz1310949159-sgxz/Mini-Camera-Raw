# P4 Verification and Review / P4 验证与审查

Date / 日期：2026-09-19

Status / 状态：Implementation and local engineering checks complete; user coarse visual acceptance passed / 实现及本地工程检查完成，用户粗略视觉验收通过。

## Result / 结果

Implemented three APIs: normalize_camera_wb, apply_white_balance and demosaic_bilinear.
The user confirmed separate learning completion and accepted ADR-006 on 2026-09-19.
No copied learning notes, new dependency, P5 implementation, commit or push.

已实现相机增益归一、白平衡及双线性去马赛克三个 API。用户于 2026-09-19 确认独立
学习完成并接受 ADR-006。未复制学习笔记，未新增依赖、实现 P5、提交或推送。

| Check / 检查 | Evidence / 证据 |
|---|---|
| Baseline / 基线 | 9950d2ad09327a7089ffd31e07a459b6c8f58c79, isolated codex/p4-white-balance-demosaic; existing draft edits preserved / 独立分支，保留既有草案修改 |
| Debug | build/p4: 58/58 CTest passed / 58/58 通过 |
| AddressSanitizer + UndefinedBehaviorSanitizer | build/p4-sanitized: 58/58 passed / 58/58 通过 |
| BUILD_TESTING=OFF | build/p4-no-tests: Release library and CLI built / Release 库及 CLI 构建通过 |
| Real samples / 真实样张 | Three authorized Sony inputs, all 7028x4688, RGGB; all active pixels checked / 三张授权 Sony 输入，全像素复核 |
| Visual / 视觉 | Three diagnostic sheets, WB comparison and histograms inspected by agent / Agent 已检查三张诊断图、白平衡对照及直方图 |
| Source review / 源码审查 | No unresolved critical or required findings / 无未解决的严重或必须修改项 |

## TDD and Coverage / TDD 与覆盖

WB tests first reached undefined-symbol linkage without implementation; adding the
implementation passed 7 new tests and 49 total. Demosaic similarly failed linkage
before implementation, then passed 8 new tests and 57 total. One additional WB
property test covers common scaling and tiny equal green gains, giving 58 total.
These were missing-definition red phases, not intentional runtime-failure stubs.

白平衡先写测试，未实现时出现未定义符号链接失败；实现后新增 7 项、共 49 项通过。
去马赛克同样先确认未定义符号失败，再实现并通过新增 8 项、共 57 项测试。补充的
共同缩放及极小相等绿色增益性质测试使总数达到 58。红灯来自缺失实现，不是故意
返回错误值的临时桩。

Deterministic tests cover four phases, 36 WB layout combinations, 192 demosaic
size/phase combinations (width 2..9, height 2..7), 100 phase/impulse placements,
constant channels, interior affine ramps, neutral identity, exact measured samples,
padding invariance, independent storage, repeated outputs, NaN/Inf, wrong states,
invalid gains, float extrema, sample underflow and gain/product overflow.
The demosaic oracle uses explicit directional stencils while production pools
same-color neighbors in a clipped 3x3 window. No tolerances were relaxed.

确定性测试覆盖四相位、36 种白平衡布局组合、192 种去马赛克尺寸/相位组合、100 种
相位/脉冲位置、分通道常量、内部仿射渐变、单位恒等、原测量值精确保留、padding
不变量、独立存储、重复输出、NaN/Inf、错误状态、非法增益、float 极值、像素下溢
及增益/乘法溢出。去马赛克测试参考显式列出方向模板，生产代码在截取的 3x3 邻域
按颜色汇总。没有放宽任何容差。

## Real-Image Numeric Evidence / 真实图像数值证据

Each original was read in place. The local C++ harness ran decode -> normalize ->
WB -> demosaic. An independent NumPy directional-stencil check visited all active
pixels, preserving double reference values until comparison. Padding/phase cases
not represented by the real camera remain covered by synthetic tests.

原片均原位读取，本地 C++ 诊断程序执行上述链条，独立 NumPy 方向模板逐一检查全部
有效像素，参考值保持 double 至比较。真实相机不包含的 padding/相位由合成测试覆盖。

| Input category / 类别 | WB max error / 白平衡最大误差 | WB RMSE | Demosaic max error / 去马赛克最大误差 | Demosaic RMSE |
|---|---:|---:|---:|---:|
| Daylight / 日光 | 5.95464e-8 | 3.11500e-9 | 5.96046e-8 | 3.97504e-9 |
| High contrast / 大光比 | 1.18976e-7 | 4.81154e-9 | 1.19209e-7 | 6.03631e-9 |
| Low light / 低照度 | 1.16415e-7 | 3.03062e-10 | 1.19209e-7 | 3.81358e-10 |

All comparisons satisfy `abs(error)<=1e-6+1e-6*abs(reference)`, with zero failed
samples, zero nonfinite RGB and zero measured-channel error. Negative and above-one
samples survive. This proves agreement with the specified reference, not agreement
with an unknown ground-truth scene or a commercial RAW renderer.

全部满足上述容差，无失败像素、无非有限 RGB，原测量通道误差为零。负值和超一值
得以保留。这证明与指定参考一致，不代表与未知真实场景或商业 RAW 渲染结果一致。

## Visual Observations / 视觉观察

Outputs are local PNG diagnostics, with no ICC claim or camera-to-working transform.
Display uses `sqrt(clip(scale*cameraRGB,0,1))`; scale is 0.75 divided by the sampled
99.5th-percentile green value. Scales: daylight 0.85634794, high contrast 0.75,
low light 11.41191410. The same scale is used before/after WB. Before-WB RGB was
actually demosaicked from normalized Bayer by the C++ API, not inferred by division.
No orientation transform: all three input orientation fields are zero.

输出为本地 PNG 诊断，无 ICC 正确性声明或相机到工作空间变换。显示使用上述映射，
scale 为 0.75 除以抽样绿色 99.5% 分位值；三张尺度如上，白平衡前后使用同一尺度。
白平衡前 RGB 由 C++ API 对归一化 Bayer 实际去马赛克获得，非事后除增益推算。
不旋转图像，三张输入方向字段均为零。

Agent inspection through the image viewer covered overviews, native 320x320 center,
top-left and bottom-right crops, before/after comparison and 256-bin full-resolution
unclipped histograms. No obvious CFA checkerboard, channel swap or border discontinuity
was seen in inspected regions. This is sampled visual coverage, not every pixel.

Agent 通过图像查看器检查总览、原比例 320x320 中心/左上/右下裁剪、前后对照及
256 桶全分辨率未裁剪直方图。已看区域未见明显 CFA 棋盘、通道交换或边界断裂。
视觉检查为区域抽样，并非逐像素肉眼验收。

- Daylight: green cast reduced; geometry coherent; fine detail remains limited by
  bilinear interpolation. / 日光：绿色偏色减弱，几何连贯；细节受双线性插值限制。
- High contrast: sun center appears pink in the diagnostic. At (4527,1169), linear
  RGB=(2.37890625,1,1.5986328125), mapped to (1,0.8660254,1). This localized example
  explains the hue from unequal saturation plus diagnostic clipping; P4 retains
  the linear values. No highlight recovery or final color-quality claim.
  / 大光比：太阳中心在诊断中偏粉，该坐标及映射数值如上；这个局部实例说明通道
  饱和值不等及诊断裁剪如何产生粉色，P4 线性值仍保留。不宣称高光恢复或最终色彩质量。
- Low light: visible shadow/chroma noise after diagnostic brightening; negative
  float values remain in data. No denoising is part of P4.
  / 低照度：诊断提亮后可见暗部和彩色噪声，数据中负值仍在；P4 不包含降噪。

Local artifacts are under build/p4-visual: diagnostic.cpp, before_wb.cpp,
check_and_render.py, compare.py, numeric-report.json, histogram-data.json,
wb-comparison.png, histograms.png and three category diagnostic sheets. All are ignored.
Private filenames and source images are absent from this public report.

本地诊断代码、JSON、前后对照、直方图及三类诊断图位于上述忽略目录。此公开报告
不包含私人文件名或原片。

## Differential and Code Review / 差异及代码审查

Scope: six new header/source/test files, two CMake registrations, accepted spec/ADR,
README and task documentation; baseline 9950d2a. The new APIs are medium review
risk; the CMake additions do not alter dependency versions or existing targets.
P3 source, image storage, validation, decoder and CLI logic are unchanged. Existing
history was inspected; no security checks or prior fixes are removed.

范围：六个新头/源/测试文件、两处 CMake 注册、规格/ADR、README 及任务文档，基线
9950d2a。新 API 按中等审查风险处理；CMake 增量不修改依赖版本或既有 target。
P3 源码、图像存储、校验、解码器及 CLI 逻辑保持不变。已查历史，无移除安全校验或
历史修复。

Review sequence: tests first, then public contract and implementation. Checked CFA
origin/parity, distinction between two green positions, immutable inputs, row padding,
validated output allocation, bounded neighbor counts, double arithmetic, float overflow,
nonfinite rejection and no per-pixel allocation. The 2x2 minimum guarantees each
missing channel has support. At most nine float terms fit in double, and missing-color
averages are convex, so finite float inputs cannot overflow demosaic output.
Only tests and local diagnostic harnesses call the three new APIs; zero existing
production callers change behavior. No external I/O is introduced by the new APIs.

先审测试，再审公开契约及实现。检查 CFA 原点/相位、两个绿色位置、输入不变、行
padding、输出分配校验、邻居数边界、double 运算、float 溢出、非有限拒绝及无逐像素
分配。最小 2x2 保证所有缺失通道有邻居；至多九个 float 项可安全累加到 double，
缺失颜色均值为凸组合，有限 float 输入不会让去马赛克输出溢出。三个新 API 目前仅
被测试及本地诊断程序调用，零个既有生产调用方改变行为；新 API 不引入外部 I/O。

Findings: 0 critical, 0 high, 0 medium, 0 low security findings; no unresolved required
code changes. Engineering recommendation: locally ready for user visual acceptance,
not a publication approval. This is a focused review, not a full LibRaw security audit.

发现：严重/高/中/低安全问题均为零，无未解决的必须代码修改。工程结论：可进入
用户视觉验收，不构成发布批准。这是定向审查，不是 LibRaw 全库安全审计。

## Reproduction and Limits / 复现与限制

Use the commands in the P4 specification; supply the authorized existing GoogleTest
source cache if offline. Recorded versions: CMake 4.3.4, GoogleTest 1.17.0, Apple
Clang 21.0.0, LibRaw 0.22.1. On this Mac use CommandLineTools via DEVELOPER_DIR.
Sanitizer configuration adds `-fsanitize=address,undefined -fno-omit-frame-pointer`
to CXX flags and `-fsanitize=address,undefined` to executable linker flags.
Run with `ASAN_OPTIONS=halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`.

按 P4 规格命令复现，离线时指定现有授权 GoogleTest 源码缓存。记录的工具版本如上，
本机通过 DEVELOPER_DIR 使用 CommandLineTools。sanitizer 编译/链接标志及运行选项如上。

Initial `detect_leaks=1` aborted before tests because this Apple ASan runtime does
not support LeakSanitizer. Removed only that unsupported option; ASan/UBSan remained
active. The baseline duplicate `-lc++` linker warning persists. An initial WB test
initializer mixed double/float constants and was corrected before the real missing-API
red phase. The optional diagnostic plotter lacked matplotlib, so existing NumPy/Pillow
produced the histograms without installing a dependency.

首次 detect_leaks=1 因 Apple ASan 不支持 LeakSanitizer 而在测试前退出，仅移除该
不支持选项，地址/未定义行为检查仍开启。基线重复 -lc++ 链接警告仍存在。初始白平衡
测试常量列表混用了 double/float，已在正式缺失 API 红灯前修正。诊断绘图环境无
matplotlib，已使用现有 NumPy/Pillow 生成直方图，未安装依赖。

Limits: no LeakSanitizer, new Linux CI run, benchmark/speed claim, fuzzing or instrumented
LibRaw binary. Real samples cover RGGB and equal green gains only; synthetic tests
cover other phases and unequal greens. Allocation-failure injection and enormous
image allocations are not performed; existing ImageBuffer overflow tests and source
review cover the shared checked allocator. WB ordering remains caller-managed per
accepted ADR. Separate learner exercises were not re-examined here. P5 and final
color acceptance remain outside P4.

限制：未执行 LeakSanitizer、新 Linux CI、benchmark/速度声明、fuzzing 或 LibRaw
二进制插桩。真实样张仅覆盖 RGGB 及相等绿色，其他相位及不等绿色由合成测试覆盖。
未注入分配失败或实际分配超大图像，共享分配器由原 ImageBuffer 溢出测试及源码
审查覆盖。白平衡顺序按已接受 ADR 由调用方管理，未重新查阅独立学习练习。
P5 与最终色彩验收仍在 P4 之外。

## User Acceptance / 用户验收 — 2026-09-19

The user reported no obvious issues after a simple visual check. Record this as
coarse P4 visual acceptance; local P4 acceptance is complete. This does not claim
exhaustive native-pixel inspection, calibrated color accuracy, or P5 acceptance.
No additional mandatory visual procedure is introduced. No commit or push.

用户简单检查后认为没有明显问题，记为 P4 粗略视觉验收通过，P4 本地验收完成。
不宣称已逐像素详查、通过校准色彩准确性检查或完成 P5 验收。不额外增加强制视觉
检查流程，未提交或推送。

## Commit Authorization / 提交授权 — 2026-09-19

After local acceptance, the user requested a local commit. Earlier statements about
not committing describe the implementation checkpoint before this authorization.
The commit contains P4 source, tests and bilingual documentation only; ignored
private diagnostics and RAW inputs are excluded. Push and main integration remain
separate, unauthorized actions.

本地验收后，用户要求创建本地提交。前文未提交的描述对应本次授权前的实施检查点。
提交仅包含 P4 源码、测试及中英双语文档，排除忽略的私人诊断材料和 RAW 输入。
推送及主分支集成仍是独立事项，尚未授权。
