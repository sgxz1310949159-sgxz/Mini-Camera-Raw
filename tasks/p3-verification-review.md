# P3 Verification and Review / P3 验证与审查

Date / 日期：2026-09-10

## Result / 结果

Pre-commit verification 2026-09-18: rebuilt `build/p3-clean` with the existing
Command Line Tools environment and reran all 42 CTest cases successfully.
Reviewed normalization/decoder tests first, then implementation and build changes;
no new blocking findings. Private sample paths and generated files are excluded
from the commit. Earlier sanitizer and real-image evidence remains dated as such.

2026-09-18 提交前复核：使用现有 Command Line Tools 环境重建 `build/p3-clean`，
全套 CTest 42/42 通过。先复核归一化/解码测试，再复核实现和构建修改，未发现新的
阻塞项。提交排除私人样张路径及生成文件；此前 sanitizer 和真实图片证据保留原日期。

Closeout 2026-09-18: after viewing the diagnostic material, the user reported
that a coarse inspection found no issue and authorized continuation. Record this
as user visual acceptance at that inspection depth, together with the earlier
user-confirmed learning completion. P3 local acceptance is complete. This does
not claim exhaustive pixel inspection, main-branch integration, publication,
or a new remote CI run. The dated pending items below are historical snapshots.
Next: [P4 entry and learning handoff](p4-handoff.md).

2026-09-18 收尾：用户查看诊断材料后反馈“粗略检查认为没问题，继续推进”。按实际
检查深度记录为人工视觉验收，加上此前用户确认的学习完成，P3 本地验收完成。
此结论不代表穷尽逐像素检查、主分支集成、发布或新一轮远端 CI；下方带日期的待办
状态是历史快照。下一步：[P4 启动与学习交接](p4-handoff.md)。

Update 2026-09-18: generated and inspected nine local diagnostic PNGs from the
project's decode/normalize APIs: before/after overviews with four corners,
centre/bright/dark mosaic and separated CFA crops, and full-range/near-black
four-position histograms. No obvious row discontinuities, introduced borders,
or large green-position mismatch were observed at the inspected scales.
Low-light shadow noise remains visible. Histogram comb structure is observed,
but its cause was not established by this inspection. This is not final color
validation or proof against subtle sensor artifacts. User visual confirmation
remains pending; the user has confirmed separate P3 learning completion.

2026-09-18 更新：从项目 decode/normalize API 生成并检查了九张本地诊断 PNG：
含四角的前后全图、中央/亮区/暗区马赛克与分离 CFA 局部、全范围及近黑四位置直方图。
在已检查尺度下未见明显行断裂、新增边框或显著双绿位置失配；低照度暗部噪声明显。
直方图可见梳状结构，本次未确定其成因。这不是最终色彩验收，也不能排除细微传感器
伪影。仍待用户确认视觉验收；用户已确认独立 P3 学习完成。

All private figures, metadata, exporter and rendering script stay ignored under
`build/p3-visual/`. Display uses sqrt(clamp(gain*x,0,1)); overview uses a 2x2 tile
mean with gains 1/1/16, dark crops use 64. No WB, demosaic, rotation or color matrix
is applied. Temporary full-resolution dumps were removed after rendering.
The bundled Python NumPy/Pillow runtime and existing Command Line Tools compiler
were used; production code was unchanged, so the earlier full-suite evidence
was not replaced by a new test claim.

私人图像、元数据、导出程序和绘图脚本均保留在被忽略的 `build/p3-visual/`。
显示映射为 sqrt(clamp(gain*x,0,1))；全图采用 2×2 tile 均值，三张显示增益依次为
1/1/16，暗部局部为 64。不执行白平衡、去马赛克、旋转或色彩矩阵；生成后已删除
临时全分辨率数据。使用内置 Python NumPy/Pillow 和现有 Command Line Tools 编译器。
生产代码未改动，本次未将此前全套测试记录冒充为新测试结果。

Update 2026-09-14: the user authorized the three existing daylight, high-dynamic-range
and low-light originals under the main project's ignored RAW directory. The new
folder's three copies were compared by size and SHA-256, then only matching copies
were removed. All originals remain. Real-file decode and normalization passed for
all three, with per-pixel comparison against a long-double reference and no
nonfinite output. Detailed private numeric results remain in ignored
`build/p3-real/results.txt`. Mosaic/visual inspection and separate learning
acceptance remain open. The dated implementation evidence below is preserved.

2026-09-14 更新：用户授权使用主项目被忽略 RAW 目录中已有的日光、大光比、低照度
三张原件。新文件夹中三张副本经大小和 SHA-256 核对一致后删除，所有原件保留。
三张真实文件均通过解码和归一化，并逐像素对照 long double 参考，输出无非有限值。
私人详细数值保留在被忽略的 `build/p3-real/results.txt`。马赛克/视觉检查及独立
学习验收仍待完成。以下保留原日期的实施证据。

The engineering implementation and synthetic verification are complete. P3 as
a whole remains open: selected private Sony sample authorization, real-file
integration/mosaic inspection and separate learning acceptance are pending.
No commit, push, merge, private RAW read/copy or learning-note edit was performed.

工程实现和合成验证已完成。P3 总体验收仍未完成：等待所选私人 Sony 样张读取授权、
真实文件集成/马赛克检查和独立学习验收。本次未 commit、push、merge，未读取/复制
私人 RAW，未编辑学习笔记。

## Verification / 验证

| Check / 检查 | Evidence / 证据 |
|---|---|
| Fresh Ninja Debug build / 全新 Ninja Debug 构建 | `build/p3-clean`, Apple Clang 21.0.0, LibRaw 0.22.1, CMake 4.3.4 |
| Compiler diagnostics / 编译器诊断 | `-Wall -Wextra -Wpedantic -Werror` passed / 通过 |
| Full CTest / 全套 CTest | 42/42 passed / 通过 |
| Tests disabled / 关闭测试 | Fresh `build/p3-no-tests` configure/build passed / 全新配置构建通过 |
| Address + UB sanitizers / 地址与未定义行为检查 | Fresh `build/p3-sanitized`, 42/42 passed / 全新构建、42/42 通过 |
| Hand-computable 2x2 normalization / 可手算 2×2 归一化 | max absolute error `1.49011611383e-09`, RMSE `7.45058056917e-10` |
| Whitespace and boundary / 空白与边界 | `git diff --check`; learning, ARW and build paths ignored / 忽略规则通过 |

Test builds reused the existing GoogleTest 1.17.0 source cache. Dependency
download was not revalidated. `P3_GTEST_SOURCE` below denotes that existing
absolute source directory, supplied through FETCHCONTENT_SOURCE_DIR_GOOGLETEST.
The Homebrew LibRaw pkg-config file adds a redundant standard-library link flag;
Apple ld emitted `ignoring duplicate libraries: '-lc++'`. Linking succeeded.

测试构建复用已有 GoogleTest 1.17.0 源码缓存，未重新验证依赖下载。下方
`P3_GTEST_SOURCE` 表示该缓存绝对目录，通过 CMake 指定。Homebrew LibRaw 的
pkg-config 文件添加重复标准库链接参数，Apple ld 报上述提示，链接成功。

```sh
cmake -S . -B build/p3-clean -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-Wall -Wextra -Wpedantic -Werror' \
  -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$P3_GTEST_SOURCE"
cmake --build build/p3-clean
ctest --test-dir build/p3-clean --output-on-failure
cmake -S . -B build/p3-no-tests -G Ninja -DBUILD_TESTING=OFF
cmake --build build/p3-no-tests
cmake -S . -B build/p3-sanitized -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined' \
  -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$P3_GTEST_SOURCE"
cmake --build build/p3-sanitized
ctest --test-dir build/p3-sanitized --output-on-failure
git diff --check
```

The numerical vector was raw `[0,200;700,1400]`, black `[100,200;300,400]`,
white `[1100,1200;1100,1200]`, expected `[-0.1,0;0.5,1.25]`. An ignored local
driver called the built library and computed the metrics above. All layout
copies use exact integer assertions. No benchmark or performance claim is made.

数值向量为上述 RAW、黑白参考及预期矩阵。仅本地、被忽略的验证程序调用已构建库，
计算表中误差。布局复制使用精确整数断言。本次无 benchmark 或性能结论。

## TDD and Findings Resolved / TDD 与已解决发现

- Normalization tests first failed to link the absent implementation, then all
  27 baseline/normalization tests passed. Decoder tests likewise failed on absent
  functions before implementation, then 36 tests passed.
- A real LibRaw `open_bayer`/`unpack` synthetic test exposed equivalent green
  index 1/3 aliasing in the final encoded row. Reproduced with an 8x8 generated
  buffer; checked the [0.22.1 source](https://github.com/LibRaw/LibRaw/blob/0.22.1/src/utils/open.cpp).
  The adapter now accepts aliases only if color, black correction and WB agree.
  A regression rejects aliases with unequal calibration. No numeric tolerance
  was weakened. This synthetic test explicitly overrides camera identity to
  exercise the narrow adapter; it is not evidence of Sony file decoding.
- Review found that alternate channel descriptions could make retained matrix
  semantics incorrect. `RejectsNonRgbgMatrixBasis` first failed, then passed after
  the supported-camera gate explicitly required RGBG channel indexing.

- 归一化测试先因缺少实现链接失败，再通过 27 个基线/归一化测试；解码测试同样先因
  缺函数失败，补实现后 36 个测试通过。
- 真实 LibRaw 合成解包测试暴露最后编码行等价绿色索引 1/3 混用；用生成的 8×8
  buffer 复现并核对上述源码。适配层只在颜色、黑电平校正和白平衡均一致时接受，
  新增回归测试拒绝不等校准，未放宽数值容差。该测试明确覆盖合成相机身份以进入
  有限适配层，不是 Sony 文件解码证据。
- 审查发现其他通道描述可能使保存的矩阵语义错误；对应测试先失败，加入 RGBG
  通道编号要求后通过。

## Security and Code Review / 安全与代码审查

Self-review used code-review-and-quality and differential-review: tests first,
then all new headers/sources, CMake/CI changes and bilingual documents. Baseline
was `eba0f64`; its P2 ImageBuffer validation is unchanged. Git history and blame
show no removed security checks. The new file parser boundary is high risk;
normalization/API changes are medium risk; documentation/build registration is
low risk. No unresolved critical or required code finding remains within the
synthetic scope. Recommendation: retain the real-file gate; do not claim full P3
acceptance or hardened arbitrary-file support.

使用 code-review-and-quality 和 differential-review 自审：先审测试，再审全部新
头/源、CMake/CI 改动和双语文档。基线 eba0f64 的 P2 校验未变，历史和 blame 未见
安全校验被移除。新文件解析边界风险高，归一化/API 中等，文档/构建注册较低。合成
范围内无遗留严重或必须修改代码发现；结论是保留真实文件门槛，不宣称完整 P3 验收
或任意不可信文件的加固支持。

Call graph / 调用关系：

```text
decode_raw -> LibRaw open_file/unpack -> copy_libraw_result -> copy_active_mosaic
normalize -> existing ImageBuffer allocation and row access
```

Each private adapter has one production caller; both public APIs currently
have zero production call sites outside their definitions (CLI integration is
later scope). The changed parser does not weaken any old parser: P2 had none.
Concrete adversarial cases reviewed include overflowed black addition,
pitch*height overflow, out-of-bounds crop, short/null storage, malformed channel
basis, nonfinite levels and false metadata. Tests reject these before affected
sample reads or arithmetic. RAII destroys LibRaw on success/failure and the
returned image owns a distinct allocation. No per-pixel allocation or implicit
clipping was found.

每个私有适配函数有一个生产调用点；两个公开 API 尚无定义以外的生产调用点，CLI
集成属于后续范围。P2 原本没有解析器，因此未弱化旧解析校验。审查了黑电平加法
溢出、pitch×height 溢出、裁剪越界、空/短 storage、错误通道基底、非有限参考及
伪造元数据；相关测试在受影响像素读取/运算前拒绝。RAII 在成功/异常时销毁 LibRaw，
返回图像拥有独立分配。未发现逐像素分配或隐式裁剪。

Limits: no full LibRaw security audit/fuzzing, no instrumented third-party LibRaw
binary, no real Sony file success/corruption path, no Windows/Linux execution,
no new remote CI, no human visual or learning acceptance. LibRaw allocation
capacity is not exposed; the adapter trusts successful unpack's buffer extent
contract after validating geometry. Optional WB is absent unless all used gains
are positive; a nonzero finite matrix is retained without claiming calibration
quality. These limitations remain visible in the specification and checklist.

限制：未完成 LibRaw 全库安全审计/fuzzing，第三方二进制未加 sanitizer，未验证真实
Sony 文件成功/损坏路径，未在 Windows/Linux 运行，未新跑远端 CI，也无人工视觉/
学习验收。LibRaw 不公开分配容量；适配层校验几何后仍信任成功 unpack 的长度契约。
白平衡仅在所用增益全为正时存在；非零有限矩阵会保存，但不保证校准质量。规格和
清单保留这些限制。

## Learning Handoff / 学习同步要点

The separate learning task can use: code values versus saturation reference;
four spatial CFA positions versus RGB channel indices; visible-origin phase
versus sensor margins; byte pitch versus element stride; unsigned subtraction
and double intermediates; preserving negative/above-one values; decoder-owned
versus returned-owned storage; and synthetic evidence versus camera integration.
Use the hand vector above for prediction before inspecting the output. This is
a topic handoff, not a record of learner mastery.

独立学习任务可同步：码值与饱和参考的区别；CFA 四个空间位置与 RGB 索引；有效区域
相位与传感器 margins；字节 pitch 与元素 stride；无符号减法与 double 中间值；
保留负值/超一值；解码器所有权与返回值所有权；合成证据与相机集成证据。可先预测
上述手算向量再看结果。这只是主题交接，不代表学习掌握记录。
