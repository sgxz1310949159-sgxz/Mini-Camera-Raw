# P6 设计准备

日期：2026-09-25
状态：P6 实现及约定样张检查完成，更广影调观感延期，已授权发布。

当前验收：日光/暗景无明显问题；用户确认逆光修正候选，生产 PNG 与其逐字节一致。
更广影调观感留至中期后，不计验收通过。用户已授权审查、提交、推送、PR 及合并。
下方早期检查点为历史记录；发布前审查前的最新测试为 101/101。

## 目标与范围

用户确认已学习 P6 并要求推进，不据此补造习题验收。tasks/plan.md 的 P6 工作包包含
曝光、直方图、一种单调影调曲线及最小处理 CLI。阶段二的更广愿望清单不扩入本阶段：
色温/色调、黑白场、GUI、局部对比度、高光重建、优化或新依赖。

## 已核验基线

基线 `a4130b0370332a2561207008261c7f5759fc93f2`。版本：CMake 4.3.4、Apple Clang 21.0.0、LibRaw 0.22.1、libpng 1.6.58、SDK zlib 1.2.12、缓存 GoogleTest 1.17.0。
独立工作区起始干净；HEAD 与实时远端 main 均为上述 P5 PR #2 合并提交，无需更新。
在新 build/p6-baseline 目录配置并编译 Debug/Ninja，76/76 CTest 通过；版本如上。
链接器仍报告重复 -lc++，未影响构建。未使用旧二进制证明新构建成功。

首次沙箱内远端读取无法连接配置的代理，获准只读重试后成功。本检查点未修改外层
工作区或旧学习笔记，也未读取私人 RAW。已检查 build/、learning/ 忽略规则。
本轮未新跑 sanitizer、Linux CI、真实样张或 benchmark，不宣称 P6 验收完成；
P5 的历史验收与本轮证据分开。

## 能力划分

用户于 2026-09-25 确认曲线、状态及输出模式；本划分为已确认边界，下述模块契约有效。

| ID | 职责 | 依赖 |
|---|---|---|
| exposure | 线性 EV 缩放 | P5 工作状态 |
| histogram | 只读 RGB 与 Y 统计 | P5 元数据 |
| tone | 一种全局亮度曲线 | 工作状态 |
| render-cli | 显式分支、次序及 PNG | 上述及 P3–P5 |

实施顺序：曝光 → 直方图 → 影调状态/曲线 → 输出分支 → CLI。
核心模块不调用 CLI，直方图不自动调曝光。

## 公共契约

输入为不可变 float32 三通道 RGB，正尺寸（含 1x1、奇数）、CFA None、明确 sRGB
原色和线性传递、名义 [0,1] 允许越界。校验有效像素，忽略行 padding；变换返回
独立紧密存储。未知空间、编码数据、NaN/Inf、状态错误抛 invalid_argument，算术
不可表示抛 overflow_error，尺寸溢出抛 length_error，分配异常上抛；double 运算、
检查后转 float，允许表示下溢到零，不静默裁剪。

API 风格提案：

```cpp
[[nodiscard]] ImageBuffer apply_exposure(const ImageBuffer& input, double ev);
[[nodiscard]] ImageBuffer apply_tone(const ImageBuffer& input, double strength);
[[nodiscard]] RgbLuminanceHistogram compute_histogram(const ImageBuffer& input);
```

公开声明放在 tone.h、histogram.h，实现与测试分别放 src/、tests/；沿用现有命名空间、
snake_case 函数、显式所有权及异常约定，不增加公开相机域曝光 API。

### 曝光

曝光公式 `out = in * exp2(ev)`；(-0.1,0.18,2) 加 +1 EV 得 (-0.2,0.36,4)。
公式如上，提议有限 EV 范围 [-20,20]、默认 0；范围外报错而非截断。输入输出均为
LinearWorkingRgb，保留带符号与超一值，原数据不变。EV=0 精确复制有效值；例子如上。

### 直方图

提议 R/G/B/相对亮度 Y 各 256 个 uint64 计数箱，分别另计小于零、大于一的数量。
0<=v<1 时 floor(256*v)，v=1 进入第 255 箱；越界值不塞进端点箱。每组箱与两项
越界计数之和为像素数，拒绝非有限有效值。Y 的权重为 (87098/409605,175762/245763,12673/175545)，使用 sRGB→XYZ 的 Y 行、double
计算；这是线性相对亮度，不是编码亮度，也不能据此判定传感器饱和。

报告曝光前、曝光后、影调后与实际输出分支统计，标注分支/状态/EV/强度。RGB 越界
数量是各通道数量，不是去重像素数。固定 [0,1] 箱不展示超一高光形状，另报极值；
不引入绘图库，不自动均衡直方图。

### 已接受影调 A

单一亮度肩部压缩，强度 s∈[0,1]、默认 0 关闭；Y=dot(weights,RGB)，scale=1/(1+s*max(Y,0))，output=RGB*scale。正 Y 时 T(Y)=Y/(1+sY)，导数 1/(1+sY)^2>0，
Y<=0 时 RGB 不变；s=0 精确复制。后续色域裁剪前保持 RGB 比例，负分量保留符号。
单调性针对 Y，不宣称 RGB 各分量独立单调；RGB 仍可超一，不是色域映射。

s=1 时中性值 0.18→0.152542、1→0.5、4→0.8。
s=1 的中性值示例如上，会压暗中间调、使白点变灰，不固定中灰，也不重建传感器
饱和高光。强度与负值延拓是由基础 Reinhard 肩部推导的项目选择，不复现完整论文。
备选 B：以 0.18 为固定点的单调分段线性对比度曲线，明确节点与线性尾部；能固定
所选中灰，但需另外审阅节点、斜率与颜色比例行为。首版只实现一种。

提议增量增加空间无关的 kToneMappedWorkingRgb，传递仍为 linear：非线性影调不等于
sRGB 编码。曝光/影调只接受 LinearWorkingRgb；直方图/编码器额外接受新状态。
s=0 也标记阶段完成，防止重复影调或影调后曝光；保留旧 P5 编码行为与元数据字段。

## 高光分支选择

设 C 为原始白平衡后相机 RGB，M 为矩阵，g=2^EV，T 为所选影调；始终保留 C、W=M*C。

| 模式 | 编码前实际输出 | 代价 |
|---|---|---|
| preserve | T(g*W) | 仅未修正诊断 |
| camera-clip | T(g*M*min(C,1)) | 普通 P5 兼容输出，硬裁剪损失细节 |

用户于 2026-09-25 确认右列候选；ADR-009 替代 ADR-008 的输出顺序。
--highlights 仍须显式指定，普通预览推荐 camera-clip，preserve 明确仅作诊断。
只裁剪独立相机副本至标称上界 1，再做矩阵、一次曝光及一次影调；原 C 和未裁剪 W
不变。不用编码预览或裁剪结果作为下次编辑源。EV0/影调0 必须精确匹配 P5；负 EV
必须保持修正后的饱和核心中性。更广影调观感留到中期后讨论。

## CLI 提案

命令为 `mini-camera-raw --input INPUT --output OUTPUT --ev 0 --tone-strength 0 --highlights preserve|camera-clip [--histogram]`。
命令提案如上，保留 --version、增加 --help。必填输入/输出路径及高光策略，只接受
完整解析的有限数值；解码前拒绝重复/未知选项和缺值。沿用 P5 排他创建、不覆盖
原文件；不加覆盖开关、自动白平衡/曝光、旋转或额外机型。统计明确标注分支/状态。

## 验收与验证

各增量用 GoogleTest 记录红绿过程。曝光测带符号/高值、零恒等、组合/逆变换、溢出
与非法 EV；影调测上述中性参考、带符号/零 Y、Y 单调、比例、强度端点、错误状态
及有限极值；直方图测精确边界/相邻 float、守恒、越界与 padding 不变。通用覆盖
原图不变、1x1、奇数带 padding、NaN/Inf 和确定性生成值。单阶段容差 `1e-6+1e-6*abs(reference)`，全图容差 `1e-5+1e-4*abs(reference)`；计数精确相等。

集成必须证明两模式均响应曝光/影调、中性 camera-clip 匹配 P5、统计对应所标 buffer、
PNG 解码值正确、CLI 错误不覆盖文件、--version 兼容。新建全套 Debug、ASan/UBSan
及关闭测试构建；授权样张仅原位读取，用上述 EV/强度/两模式对照数值参考并进行
Agent 视觉检查，用户视觉验收单列。公开合成测试不依赖私图，无测量不作性能声明。

本工作区命令：

```sh
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake -S . -B build/p6-baseline -G Ninja -DCMAKE_BUILD_TYPE=Debug -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST='/Users/sgxz/Mini - Camera Raw/build/p2-verify/_deps/googletest-src'
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake --build build/p6-baseline
DEVELOPER_DIR=/Library/Developer/CommandLineTools ctest --test-dir build/p6-baseline --output-on-failure
git diff --check
```

始终保留原工作数据并校验契约；改变已接受曲线、处理状态、输出策略或新增依赖须另审。
不得改其他工作区、伪造学习/视觉验收、公开私图或未经授权提交/推送。

## 来源

- 仅基础亮度肩部，不采用自动 key 或局部算子。

- 参考性 sRGB XYZ Y 系数，草案而非 ISP 标准。

- 色彩身份与传递独立于影调。

- 源码覆盖后重新编译缓存 GoogleTest，在线系列页为 4.3.5、本机帮助为 4.3.4。

- P5 已接受决策及证据。

## 实施细化

输出协调器放在 apps/render_pipeline.*，属于应用内部，接受原相机 RGB 与矩阵；返回
所选影调后图像及可选阶段统计，不暴露裁剪后的编辑源。直方图保存 R/G/B/Y 四组箱、
越界计数与极值；影调即使强度为零也标记完成。确定性生成测试不引入新依赖。沿用
上述命令及 API 来源，P5 原有函数契约保持。

最终本地证据与视觉限制：[P6 verification](../tasks/p6-verification-review.md).

相关资料与证据链接（保留原记录来源）：

- [Reinhard et al., 2002, equation 3](https://web.tecgraf.puc-rio.br/~scuri/inf1378/pub/reinhard.pdf)

- [W3C CSS Color 4, conversion sample](https://www.w3.org/TR/2026/CRD-css-color-4-20260913/#color-conversion-code)

- [ICC sRGB registry](https://registry.color.org/rgb-registry/srgb)

- [CMake 4.3 FetchContent](https://cmake.org/cmake/help/v4.3/module/FetchContent.html)

- [P5 accepted decisions](decisions/ADR-007-p5-color-and-output.md)

- [evidence](../tasks/p5-verification-review.md)
