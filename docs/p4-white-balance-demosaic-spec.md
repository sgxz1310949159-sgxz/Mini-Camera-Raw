# P4 白平衡与双线性去马赛克

状态：2026-09-19 已确认实施契约。

日期：2026-09-18

## 范围

按路线图顺序准备两个标量 C++17 能力：白平衡将归一化 Bayer 转为增益校正后的 Bayer，
双线性去马赛克将其转为相机线性 RGB。两者均可用合成输入独立测试。用户于 2026-09-19 确认学习完成并接受三项设计，授权实施；学习在独立任务进行，不复制私人笔记。不包含 P5、自动
白平衡、新依赖、优化或发布。

## API 与状态

公开目录为 `include/mini_camera_raw/`、`src/`、`tests/`，对应文件为 `white_balance.h/.cpp`、`demosaic.h`、`demosaic_bilinear.cpp`、`white_balance_test.cpp`、`demosaic_test.cpp`。
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

两阶段要求 float32 单通道 LinearBayer 及支持的 CFA。输入不修改，输出拥有独立紧密
存储。遵守输入元素 stride，忽略包括非有限值在内的 padding。白平衡保持尺寸和 CFA；
去马赛克保持尺寸，输出交错 R,G,B、CFA=None、LinearCameraRgb。两者名义范围均为
[0,1] 且允许超范围。传感器元数据及方向由调用方保留，不旋转或裁剪。现有 ColorState
不能区分白平衡前后，调用方保证只应用一次增益，不能宣称类型已强制此顺序。

## CFA 与增益

空间位置公式为 `p=2*(y%2)+(x%2)`。
空间索引相对于有效图像原点，公式如上；它不是 LibRaw 通道编号，P3 已将
camera_wb_tile 映射为空间顺序。

| CFA | 00 | 01 | 10 | 11 |
|---|---|---|---|---|
| RGGB | R | G | G | B |
| BGGR | B | G | G | R |
| GRBG | G | R | B | G |
| GBRG | G | B | R | G |

归一化公式为 `gref=(m[a]+m[b])/2`、`gain[p]=m[p]/gref`。
四个增益均须为有限正 double，即使输入很小也全部校验。手动增益按原值应用。
相机元数据采用独立显式 helper：a,b 为两个绿色位置，按上述公式归一，校验参考值
及各比值为有限正值。均值以 `lo+(hi-lo)/2` 计算，避免相加溢出或两个极小绿色
先除以二导致下溢。保留两个绿色增益的差异。统一缩放保持所有增益比例，使绿色
增益均值为一。这是项目曝光约定，并非 LibRaw 保证。相机白平衡缺失时由调用方报错
或明确提供手动增益，不静默使用单位/日光增益。

白平衡公式为 `out[y,x]=double(in[y,x])*gain[p]`。
白平衡按上述公式计算后转 float，保留负值和超一值，不裁剪。支持全部正尺寸，包括
1x1。单位增益可作为显式诊断选择。

## 双线性规则与边界

精确保留原测量通道。缺失通道按对应颜色邻居求均值：R/B 位置的 G 用上下左右，
R 位置的 B 或 B 位置的 R 用对角，G 位置的 R/B 按 CFA 选择水平或垂直邻居。
double 累加，除以邻居数后转 float；不加入跨通道校正或方向自适应。

已接受的边界：忽略图外邻居，按剩余数量求均值。这是项目边界扩展，不宣称与所有库的
双线性边界一致。不钳制马赛克坐标，以免改变 CFA 颜色。要求宽高均至少为 2，支持
2x2、2xN、Nx2 及奇数尺寸；拒绝缺少某些颜色样本的 1xN/Nx1。支持尺寸的所有缺失
分量均有邻居。仿射渐变只要求内部恢复精确，不要求边缘同样成立。

## 错误与数值

非法状态/CFA、非有限有效像素、非正/非有限增益及不支持尺寸抛 invalid_argument。
无法表达的运算结果在转 float 前抛 overflow_error，无法表达的增益比值亦如此。
分配尺寸溢出抛 length_error，分配失败向上传播。不返回部分结果，失败不改输入。
允许像素下溢到零，拒绝增益下溢到零。double 中间值，标量单线程逐行处理，每阶段
一次输出分配，无逐像素分配、不启用 fast-math；暂不宣称性能。

## 验收

手算向量：

  RGGB 输入 `[-0.25,0.5;0.75,1.25]`，增益 `[2,1;1,3]`，输出 `[-0.5,0.5;0.75,3.75]`；相机 tile `[4,2;2,6]` 归一到上述增益。
白平衡输入、增益、预期输出及相机增益归一结果如上。

- RGGB 输入 `[2,4;6,8]` 的逐行 RGB 为 `(2,5,8), (2,4,8), (2,6,8), (2,5,8)`。
2x2 去马赛克按提案边界得到上述逐行 RGB。

公式容差 `abs(error)<=1e-6+1e-6*abs(reference)`；组合全图容差 `1e-5+1e-4*abs(reference)`。
覆盖四相位、不等绿色、单位增益精确恒等、原测量通道精确保留、分通道常量、脉冲、
内部渐变、负值/超一值、奇数/小尺寸、padding 不变量、所有权、确定性、非法输入、
各位置 NaN/Inf、极端归一及乘法溢出。采用确定性生成性质和结构独立的模板参考，
检查插值分通道值域保持及正增益白平衡单调性。公式及组合图像容差如上，图像比较
报告最大绝对误差及 RMSE。

数值正确后原位读取三张授权原片，派生输出保持忽略。记录参数、裁剪、方向、查看器、
诊断映射和拉链纹/伪色。诊断显示映射置于正式 API 外并须标注：相机 RGB 不是 sRGB。
P4 视觉检查不能证明 P5 色彩准确。学习完成及学习者手算仍为独立门槛。

## 验证与边界

使用 `-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/absolute/cache/path` 指定缓存；本机命令前缀为 `DEVELOPER_DIR=/Library/Developer/CommandLineTools`。
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

后续实现还须完成全套 ASan/UBSan、先测试后代码的审查及真实样张数值/视觉检查。
准备期基线仅验证已有 P3 代码。始终验证并保护私有边界，新范围/依赖须用户授权；
不静默裁剪、复制私人笔记或发布本轮内容。

## 来源与审阅点

  cam_mul 为拍摄时白平衡元数据，已核对本机 0.22.1 及 P3 适配层。

- 内部双线性邻域及边缘伪影参考，不采用 MHC 校正或源码。

- 现有源码覆盖方式。

已接受的审阅点：绿色均值曝光约定、有效邻居边界、最小尺寸及调用方管理的白平衡顺序。
见已接受的 ADR-006。

验收更新：2026-09-19，用户确认独立学习完成并接受全部设计，本任务不重复授课；手算证据保留在独立学习任务，未复制或独立查阅。

当前实现证据：[P4 verification](../tasks/p4-verification-review.md).

相关资料与证据链接（保留原记录来源）：

- [LibRaw data structures](https://www.libraw.org/docs/API-datastruct.html)

- [Getreuer, IPOL 2011, Algorithm](https://www.ipol.im/pub/art/2011/g_mhcd/revisions/2011-08-14/g_mhcd.htm)

- [CMake 4.3 FetchContent](https://cmake.org/cmake/help/v4.3/module/FetchContent.html)
