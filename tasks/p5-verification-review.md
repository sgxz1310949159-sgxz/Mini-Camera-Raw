# P5 Verification and Review / P5 验证与审查

Date / 日期：2026-09-21
Status / 状态：P5 local implementation and coarse visual acceptance complete / P5 本地实现与粗略视觉验收完成。

## Scope / 范围

Base d8cd328c16ab593b307c59524a9762d9120ee3c9, isolated codex/p5-color-display-output.
Added explicit RGB identity/transfer, scalar camera-to-linear-sRGB transform,
independent clipped/encoded RGB16 output and an exclusive-create PNG writer.
User confirmed learning and authorized implementation. No outer/P3/P4 edits,
publication, P6 algorithms, GUI or processing CLI. Existing unspecified working
metadata remains valid but cannot silently enter the sRGB encoder.
基线及独立分支如上，增加明确色彩身份/传递、标量矩阵、独立裁剪编码 RGB16 和排他
创建 PNG 写入。学习与实施均获用户确认，未修改外层/P3/P4、发布或实现 P6/GUI/
处理 CLI。保留旧未知空间数据合法性，但不默认为 sRGB 编码输入。

## Automated evidence / 自动化证据

- Red/green cycles: metadata test failed on missing fields; matrix/display/PNG tests
  failed on missing headers before production implementation. Logs remain ignored.
- Fresh Debug suite: 73/73 passed. ASan/UBSan: 73/73, ASAN_OPTIONS=detect_leaks=0,
  UBSAN_OPTIONS=halt_on_error=1. Apple LeakSanitizer was not run.
- Separate BUILD_TESTING=OFF configure/build and CLI version smoke passed.
- Independent PNG test reader validates signature, chunk CRC, RGB16/IHDR, sRGB intent,
  zlib/filter-none rows and exact decoded integers, including width 11001 (66006
  pixel bytes per row), 1-pixel width and odd/padded storage.
- File tests cover existing originals, symlinks, directories, missing parents and
  resource-limit-induced write/flush failures. An incompressible fixture specifically
  reaches the libpng longjmp failure path and verifies cleanup.
- Matrix tests cover non-symmetric basis vectors, identity, generated signed values,
  padding, unknown destination, missing/nonfinite metadata, fourth-column policy,
  nonfinite samples and numeric overflow. Display tests cover known sRGB vectors,
  branch neighbors, inverse-generated ramp/monotonicity, half-step rounding and
  preservation of source highlights/negatives.
- New production sources pass -Wall -Wextra -Wpedantic syntax checks without diagnostics.
- git diff --check passed. All real PNGs and diagnostic sources/logs are ignored.

- 各增量先确认测试因缺少字段/头文件失败，再实现并转绿，日志保持本地忽略。
- Debug 与 ASan/UBSan 各 73/73，通过独立关闭测试构建及版本 smoke；未运行泄漏检查。
- PNG 独立测试验证签名、CRC、RGB16/IHDR、sRGB intent、zlib/filter-none 行和精确
  整数，包括 66006 字节宽行、单像素宽、奇数和 padding。
- 文件测试覆盖已有原文件、符号链接、目录、缺失父目录及资源限制导致的写入/刷新
  失败；不可压缩 fixture 专门触发 libpng longjmp 并验证清理。
- 矩阵测试覆盖方向、恒等、带符号生成值、padding、未知目标、非法元数据、第四列、
  非有限像素及溢出。编码覆盖已知向量、分段邻点、逆生成参考/单调性、舍入边界及
  源数据保留。新生产源码通过额外严格告警检查且无诊断；diff 检查通过，真实 PNG 与诊断源码/日志均已确认忽略。

An initial PNG CMake integration failed because a sibling test directory could not
see a scoped ZLIB::ZLIB target. Adding explicit find_package(ZLIB REQUIRED) in the
tests directory fixed discovery. An old 68-test binary run immediately after that
failed generation was excluded from PNG evidence; the rebuilt 73-test runs supersede it.
首次 PNG 构建因同级测试目录看不到 ZLIB::ZLIB 失败，在测试目录显式发现依赖后修复。
失败后运行过的旧 68 项测试不计为 PNG 证据，以重建后的 73 项结果为准。

## Real samples / 真实样张

Three authorized originals were read in place, without copying or uploading. Pipeline:
P3 normalization → P4 camera WB (green mean) → bilinear → camera matrix → sRGB RGB16
→ PNG. No tone/exposure adjustment. LibRaw 0.22.1, PNG runtime 1.6.58. All matrices
had zero fourth columns. Each image is 7028x4688, orientation metadata zero.
三张授权原片原位读取，未复制/上传。流水线如上，不加曝光/影调。矩阵第四列均为零，
尺寸均为 7028x4688、方向值零，库版本如上。

Long-double dot-product oracle compared every working component, with tolerance
1e-6+1e-6*abs(reference). All components passed. A bit-pattern fingerprint of the
working buffer before/after output encoding matched for every sample.
以 long-double 点积参考检查每个工作分量，全部满足上述容差；编码前后工作 buffer
逐位指纹一致，证明本次运行未修改内部图像。

| Category / 类别 | Linear range / 线性范围 | Max error / 最大误差 | RMSE |
|---|---|---:|---:|
| Daylight / 日光 | -0.037247 .. 1.820690 | 5.96041e-8 | 4.32781e-9 |
| Backlit / 逆光 | -0.107198 .. 3.624131 | 1.19209e-7 | 1.05173e-8 |
| Dark scene / 暗景 | -0.035956 .. 3.308233 | 1.19168e-7 | 5.33831e-10 |

Detailed private evidence is in ignored build/p5/real-20260921: numeric.txt,
highlight.txt, sample1..3.png and preview1..3.png. System sips independently identified
all full-size outputs as 16-bit RGB with sRGB IEC61966-2.1 interpretation. Thumbnails
were made with sips -Z 1400 solely for viewing; numeric tests use full-size data.
详细证据位于上述忽略目录。系统 sips 独立识别三张完整输出为 16 位 RGB、sRGB。
缩略图只用于查看，数值验证使用原尺寸数据。

Agent inspected all three thumbnails via image viewer: daylight scene has plausible
colors and dark shadows; backlit scene has a conspicuous pink solar core; dark scene
remains very dark. This is not calibrated color accuracy or full-resolution artifact
acceptance. At the sampled solar-core location, the four normalized sensor samples
are all 1; WB/demosaic camera RGB is about (2.37891,1,1.59863), working RGB about
(3.14377,0.617426,1.78865). Independent channel clipping explains the pink result.
No highlight reconstruction is implemented; exposure/tone alone cannot guarantee
recovery of sensor-saturated color. Record the limitation rather than disguise it.

Agent 已查看三张缩略图：日光颜色整体合理但暗部较深；逆光太阳核心明显粉紫；暗景
仍较暗。此不等于标定色彩精度或全尺寸伪影验收。太阳核心抽样的四个归一化传感器
值均为 1，白平衡/插值相机 RGB 及工作 RGB 如上，逐通道裁剪能解释粉紫结果。
本阶段未实现高光重建，曝光/影调本身不保证恢复传感器饱和颜色，应明确记录限制。

## Review scope and remaining gates / 审查范围与剩余项

Tests-first review covers new numeric paths, additive metadata validation and writer
ownership/longjmp/stride/size/error behavior. Differential scope includes all changed
public headers, image_buffer.cpp, dependency and CI changes, plus untracked sources.
Baseline history of image_buffer.cpp was inspected; existing validation is retained,
new identity restrictions are additive. Existing aggregate initializers remain tested.
No full untrusted-RAW parser audit, performance claim or Windows support is implied.
先测试后实现审查覆盖新数值路径、元数据增量校验及写入所有权/longjmp/stride/尺寸/
错误行为。差异范围包括公开头、图像模型、依赖/CI 及未跟踪新源码，已检查模型基线
历史；原校验保留，身份约束增量增加，旧聚合初始化仍有测试。不代表完整不可信 RAW
解析器审计、性能结论或 Windows 支持。

Coverage limits: independent review returned no report; Linux remote CI has not run
(publication is not authorized). No benchmark/optimization performed.
覆盖限制：独立审查未返回报告；Linux 远端 CI 未运行（未获发布授权）；未优化或 benchmark。

## Reproduction / 复现

Run from the isolated P5 worktree; on this Mac use
DEVELOPER_DIR=/Library/Developer/CommandLineTools. Existing GoogleTest source may be
provided with FETCHCONTENT_SOURCE_DIR_GOOGLETEST as recorded in tasks/plan.md.
从 P5 独立工作区运行，本机用上述 DEVELOPER_DIR；GoogleTest 缓存覆盖见计划记录。

```sh
cmake -S . -B build/p5 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/p5
ctest --test-dir build/p5 --output-on-failure
cmake -S . -B build/p5-sanitize -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build/p5-sanitize
ASAN_OPTIONS=detect_leaks=0 UBSAN_OPTIONS=halt_on_error=1 \
  ctest --test-dir build/p5-sanitize --output-on-failure
cmake -S . -B build/p5-no-tests -G Ninja -DBUILD_TESTING=OFF
cmake --build build/p5-no-tests
```

## User visual feedback and highlight diagnosis / 用户视觉反馈与高光定位

The user accepted daylight and dark-scene previews at a coarse level, but flagged
pink/magenta overexposed areas in the backlit image. This is not an intentional
warning overlay or creative effect, and is not accepted as normal final rendering.
The backlit visual gate remains open; numeric correctness does not close it.
用户粗看日光与暗景没有明显问题，但指出逆光太阳及其他过曝部分粉紫色异常。这不是
刻意的过曝警示或创作效果，不作为正常成片显示接受。逆光视觉验收仍未通过，数值
正确性不能代替该验收。

A separate local LibRaw 0.22.1 diagnostic used camera WB, bilinear demosaicing,
sRGB output, linear gamma, 16-bit output, disabled auto brightness and disabled
maximum adjustment. At the same solar-core coordinate, highlight=0 (clip) gave
approximately (1,0.999985,1); highlight=1 (unclip) gave (1,0.259525,0.751858).
The different scaling/internal integer path prevents treating these as a pixelwise
oracle for our float pipeline. It does independently demonstrate that highlight
policy changes this saturated core, without changing PNG or sRGB encoding.
独立本地 LibRaw 0.22.1 诊断采用相机白平衡、双线性、sRGB、线性 gamma、16 位输出，
关闭自动亮度及最大值自动调整。同一太阳核心坐标在 highlight=0（裁剪）时接近白色，
highlight=1（保留）时仍偏色，具体数值如上。其缩放及内部整数路径不同，不能作为
本 float 流水线的逐像素参考，但独立说明高光策略影响饱和核心，不涉及 PNG 或
sRGB 编码的修改。

Evidence: ignored build/p5/reference_highlight.cpp and
build/p5/real-20260921/libraw-highlight.txt. API semantics verified against installed
official LibRaw API-datastruct.html (highlight modes, no_auto_bright,
adjust_maximum_thr); online retrieval was unavailable during this check.
证据保留在上述忽略路径。API 语义依据当前安装版本的官方 API-datastruct.html；
本轮在线读取失败，以本地随库官方文档核验。

Initial design decision (now resolved below): add an explicit highlight rendering stage to P5, or carry it
as a required next-stage item. Preserve unclipped working data and do not whiten
all out-of-gamut colors indiscriminately. No highlight behavior has been changed
pending this scope decision; exposure/tone alone is not promised to recover lost
sensor information.
最初待决范围（现已按下文解决）：本 P5 补充明确的高光渲染阶段，或列为下一阶段必修项。应保留未裁剪
工作数据，不得将所有越界高饱和颜色一律抹白。范围决定前未修改高光行为，也不承诺
曝光/影调本身能恢复已经丢失的传感器信息。

## Authorized highlight implementation and recheck / 已授权高光实施与复验

User chose to add highlight handling in P5. Added explicit clip_camera_highlights:
a white-balanced camera-RGB output copy with each finite channel limited above at 1,
then matrix and encoding. Matrix and encoder contracts remain unchanged. The input
camera and original unbounded working image are preserved. This is a conservative
hard clip, not reconstruction; partially saturated regions may still change hue.
用户选择 P5 内补充高光。新增显式 clip_camera_highlights，对白平衡相机 RGB 输出
副本逐通道仅裁上界 1，再矩阵与编码；矩阵及编码原语契约不变。保留原相机数据及
未限幅工作图像。这是保守硬裁剪而非重建，局部饱和仍可能改变色相。

Three tests first failed because the API did not exist, then passed after the scalar
implementation. Fresh Debug and ASan/UBSan suites each passed 76/76; no-tests build
and strict production syntax warnings also passed. Logs: ignored
build/p5/highlight-{red,build,test,sanitize-build,sanitize-test,no-tests}.log.
No performance benchmark or LSan claim. Self-review covered state validation,
finite checks, packed/padded stride, independent ownership, linear/matrix ordering,
fixed nominal scale, arithmetic bounds and the documented detail-loss tradeoff;
no required findings remain in that review. The separately requested independent
review did not return a report and was stopped; do not count it as completed coverage.
三个测试先因 API 不存在而失败，实现后通过。新版 Debug 与 ASan/UBSan 各 76/76，
关闭测试构建及严格生产源码告警检查通过，日志见上述本地忽略路径。不作性能或
LSan 声明。自审覆盖状态/非有限、紧密与 padding、独立所有权、线性/矩阵次序、
固定标称尺度、算术边界及细节损失约定，无待修必需项。另请求的独立审查未返回
报告，已停止，不能计为完成覆盖。

All three originals were reprocessed into ignored build/p5/highlights-20260921.
The harness compared every clipped component to min(original,1), verified camera
and original-working fingerprints unchanged, and repeated the original long-double
matrix comparison. All passed. Clipped component counts were 123854, 2121781, 4049
for daylight/backlit/dark; these are channel-component counts, not pixel counts.
The numeric.txt ranges describe preserved unbounded working data, not the clipped
output. Full PNGs and 1400-wide previews are separate from the original outputs.
三原片重新处理至上述忽略目录。对每个裁剪分量与 min(original,1) 逐一比较，检查原
相机/工作图像指纹不变，并重复 long-double 矩阵参考，全部通过。三图被裁剪分量
数量依次如上（不是像素数）；numeric.txt 范围是保留的未限幅工作图像范围，不是
裁剪输出范围。完整 PNG 与 1400 宽预览均与旧输出分开保留。

Agent viewed all new previews: the obvious magenta solar core/reflections are gone,
the sun has a white core with yellow surroundings, and no obvious new issue was
seen in daylight/dark thumbnails. This does not establish calibrated color fidelity
or full-resolution artifact acceptance. The user subsequently accepted all revised previews at a coarse visual level.
Agent 已查看三张新预览：明显粉紫核心/反光消失，太阳白色核心周围呈黄色；日光与
暗景缩略图未发现明显新问题。这不是标定色彩精度或全尺寸伪影验收，用户随后确认新版通过粗略视觉验收。

Final user acceptance / 最终用户验收：2026-09-21, “新版通过粗略视觉验收”.
Local completion does not claim independent-review coverage, remote Linux CI,
full-resolution/calibrated acceptance, or high-quality highlight reconstruction.
No commit or push was made.
本地完成不代表独立审查、远端 Linux CI、全尺寸/标定验收或高质量高光重建。未提交或推送。

## Publication authorization / 发布授权 — 2026-09-21

User requested upload and merge. The earlier no-publication statements describe
the implementation checkpoint; publication is now authorized. Submit only P5
source, tests, build configuration and bilingual documentation; merge after remote
CI succeeds. Private images, learning and build artifacts remain local.
用户要求上传合并。此前未发布的表述为实施检查点历史状态，现已授权发布。只提交
P5 源码、测试、构建配置与双语文档，远端 CI 通过后合并；私人图像、学习及构建产物
保持本地。
