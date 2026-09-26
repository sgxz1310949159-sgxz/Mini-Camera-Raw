# Project Foundation / 项目基础说明

Date / 日期：2026-07-06

## Positioning / 项目定位

Mini-Camera Raw is a learning-first engineering project. Its value comes from
making the RAW processing pipeline visible, explainable, and testable rather
than from matching every feature of a commercial RAW editor.

Mini-Camera Raw 是一个学习优先的工程项目。它的价值不在于完整复刻商业 RAW 编辑器的所有功能，而在于把 RAW 处理流水线变得可观察、可解释、可测试。

The core learning goal is to understand bottom-level image principles:

核心学习目标是理解底层图像原理：

- RAW sensor data, Bayer/CFA layout, black level, white level, and dynamic range
- ISP pipeline order and why each stage exists
- linear-light image processing versus nonlinear display-referred processing
- color correction matrices, working color spaces, and gamma encoding
- tone curves, histograms, exposure values, and clipping
- later: local contrast, edge-preserving filters, multithreading, and GPU ideas

- RAW 传感器数据、Bayer/CFA 排列、黑电平、白电平和动态范围
- ISP 流水线顺序，以及每个阶段为什么存在
- 线性光图像处理与非线性显示空间处理的区别
- 色彩校正矩阵、工作色彩空间和 Gamma 编码
- 影调曲线、直方图、曝光值和裁剪
- 后续扩展：局部对比度、保边滤波、多线程和 GPU 思路

## Working Loop / 工作闭环

Each feature should follow the same loop:

每个功能都应该遵循同一个闭环：

1. Learn the principle.
2. Write a short design note: input, output, formula, data range, edge cases.
3. Implement the smallest correct CPU version.
4. Validate it with numeric tests and visual checks.
5. Record what changed, what was learned, and what still looks wrong.

1. 先学习原理。
2. 写一份简短设计说明：输入、输出、公式、数据范围、边界情况。
3. 实现最小但正确的 CPU 版本。
4. 用数值测试和视觉检查进行验证。
5. 记录改了什么、学到了什么、还有哪里看起来不对。

This loop keeps the project from becoming a pile of experiments. It also turns
the final repository into a portfolio artifact: code plus reasoning.

这个闭环能防止项目变成零散实验堆，同时也会让最终仓库成为更有展示价值的作品：不仅有代码，也有推理和解释。

## Roadmap / 路线图

Schedule note: the fixed 2026-10-01 acceptance covers the core baseline:
Stage 0, a complete but simple Stage 1 CPU path, and a reduced Stage 2 tone
engine. Stage 3 and Stage 4 remain part of the roadmap but move to the
2026-10-08 to 2026-11-06 extension window. This protects principle study, numeric validation,
and reproducibility from being traded for feature count.

进度说明：固定的 2026-10-01 验收覆盖核心基线，即阶段零、完整但简单的阶段一
CPU 路径，以及缩减后的阶段二影调引擎。阶段三和阶段四仍保留在路线图中，但移至
2026-10-08 至 2026-11-06扩展窗口。这样可以避免为了堆叠功能而牺牲原理学习、数值验证和可复现性。

### Stage 0: Engineering Baseline / 阶段零：工程底座

Goal: create a durable project base before implementing image algorithms.

目标：在实现图像算法之前，先创建一个能长期推进的工程基础。

Outputs:

产出：

- public repository structure
- MVP boundary and acceptance criteria
- initial CMake/testing plan
- initial image data model
- local-only learning workspace

- 公开仓库结构
- MVP 边界和验收标准
- 初始 CMake 与测试计划
- 初始图像数据模型
- 本地学习工作区

Learning focus:

学习重点：

- C++ project structure
- image memory layout
- bit depth and numeric ranges
- basic testing strategy

- C++ 项目结构
- 图像内存布局
- 位深和数值范围
- 基础测试策略

### Stage 1: RAW Ingestion and Basic ISP / 阶段一：RAW 读取与基础 ISP

Goal: turn a RAW file into a viewable RGB image.

目标：把 RAW 文件转换成一张可查看的 RGB 图像。

Target pipeline:

目标流水线：

```text
RAW file
-> LibRaw parse
-> Bayer/CFA buffer
-> black level correction
-> white balance
-> demosaicing
-> camera-to-working-space color transform
-> gamma/display transform
-> PNG/TIFF output
```

```text
RAW 文件
-> LibRaw 解析
-> Bayer/CFA 缓冲区
-> 黑电平校正
-> 白平衡
-> 去马赛克
-> 相机色彩空间到工作色彩空间转换
-> Gamma/显示转换
-> PNG/TIFF 输出
```

Learning focus:

学习重点：

- RAW versus JPEG
- Bayer pattern and demosaicing
- black/white level normalization
- white balance as channel gain
- color matrix conversion
- why most adjustments should happen in linear space

- RAW 与 JPEG 的区别
- Bayer 排列和去马赛克
- 黑电平/白电平归一化
- 作为通道增益的白平衡
- 色彩矩阵转换
- 为什么多数调整应发生在线性空间

### Stage 2: Tone Engine and Temporary UI / 阶段二：影调引擎与临时 UI

Goal: add basic Camera Raw-style controls while keeping the core engine clean.

目标：加入基础 Camera Raw 风格控制，同时保持核心引擎干净。

Priority controls:

优先控制项：

- Exposure
- Contrast
- Blacks / Whites
- Temperature / Tint
- basic tone curve or gamma control

- 曝光
- 对比度
- 黑场 / 白场
- 色温 / 色调
- 基础影调曲线或 Gamma 控制

OpenCV highgui can be used as a temporary validation UI, but UI code must stay
outside the core engine.

OpenCV highgui 可以作为临时验证 UI，但 UI 代码必须留在核心引擎之外。

Learning focus:

学习重点：

- EV exposure math
- S-curves and contrast
- histograms and clipping
- linear versus sRGB-space adjustment
- parameter range design

- EV 曝光数学
- S 曲线和对比度
- 直方图和裁剪
- 线性空间调整与 sRGB 空间调整的区别
- 参数范围设计

### Stage 3: Color and Local Light Control / 阶段三：色彩与局部光影控制

Goal: explore higher-level color and local tone algorithms without losing the
project's explainable structure.

目标：探索更高阶的色彩和局部影调算法，同时保持项目结构可解释。

Candidate controls:

候选控制项：

- Saturation
- Vibrance
- RGB <-> HSV/HSL conversion
- simple HSL color mixer
- simplified Shadows / Highlights

- 饱和度
- 鲜艳度
- RGB 与 HSV/HSL 双向转换
- 简化版 HSL 混色器
- 简化版阴影 / 高光

Learning focus:

学习重点：

- color model tradeoffs
- saturation versus vibrance
- luminance masks
- bilateral/guided filtering
- low-frequency illumination and high-frequency detail

- 色彩模型取舍
- 饱和度与鲜艳度的区别
- 亮度蒙版
- 双边滤波 / 导向滤波
- 低频光照结构与高频纹理细节

### Stage 4: Performance and API Export / 阶段四：性能优化与 API 导出

Goal: turn the correct CPU pipeline into a more serious engineering artifact.

目标：把正确的 CPU 流水线变成更完整的工程作品。

Priority order:

优先顺序：

1. stable single-threaded CPU version
2. benchmark tooling
3. OpenMP/multithreaded loops
4. memory-access optimization
5. dynamic library export
6. Metal compute as an optional advanced extension

1. 稳定的单线程 CPU 版本
2. benchmark 工具
3. OpenMP / 多线程循环
4. 内存访问优化
5. 动态库导出
6. Metal Compute 作为可选高级扩展

Learning focus:

学习重点：

- cache-friendly pixel traversal
- memory bandwidth limits
- CPU versus GPU execution models
- C API design and memory ownership
- dynamic library packaging

- 缓存友好的像素遍历
- 内存带宽限制
- CPU 与 GPU 执行模型差异
- C API 设计和内存所有权
- 动态库打包

## Acceptance Categories / 验收类别

Every stage should be checked with five categories:

每个阶段都应该用五类标准检查：

- Functional: the feature runs end-to-end.
- Numeric: formulas match expected values within a documented tolerance.
- Visual: output has no obvious color cast, clipping, halo, or banding issue.
- Performance: timing and memory measurements are recorded for fixed inputs.
- Learning: the principle can be explained in the project notes.

- 功能验收：功能能端到端跑通。
- 数值验收：公式结果在记录好的容差范围内符合预期。
- 视觉验收：输出没有明显偏色、裁剪、光晕或断层问题。
- 性能验收：对固定输入记录耗时和内存测量结果。
- 学习验收：能在项目笔记中解释对应原理。

## Product requirements update / 产品要求更新 — 2026-09-26

The intended product must support photographers' varied editing choices, not just
convert RAW files into viewable PNGs. The user has confirmed three mandatory future
capabilities. These supersede any earlier wording that treats them as optional:
项目最终需要支持摄影师多元的编辑选择，不止将 RAW 转成可显示 PNG。用户已确认以下
三项后续必做能力，覆盖此前将其视为可选项的表述：

| Capability / 能力 | Confirmed requirement / 已确认要求 |
|---|---|
| Tonal-region adjustments / 亮度分区调整 | Adjust different brightness regions independently of global exposure / 能分别调整不同亮度区域，不限于全局曝光 |
| Color adjustments / 颜色调整 | Provide user-controlled color editing beyond the fixed camera-to-working transform / 提供用户可控的颜色编辑，不限于固定相机到工作空间转换 |
| Continuous preview / 连续预览 | Show updated image results continuously while editing, without requiring a separate CLI export for each change / 编辑过程中连续观看变化，无需每次修改后单独执行 CLI 导出 |

Specific controls, region definitions, formulas, parameter ranges, color semantics,
UI technology, preview/export consistency, and measurable latency/memory targets
remain to be specified. The provisional P8–P13 order is recorded in tasks/plan.md;
no new deadline is set here.
具体控件、区域定义、公式、参数范围、颜色语义、界面技术、预览/导出一致性及可测量的
延迟/内存目标尚待定义；暂定 P8–P13 顺序见 tasks/plan.md，不新增截止日期。

The current P6 luminance shoulder is a verified simple formula, not a permanent
product requirement. The provisional exclusion decision below is now authoritative;
existing implementation/tests remain available as reference.
当前 P6 亮度肩部是已验证的简单公式，不是永久产品要求；以下暂定排除决定为当前
依据，现有实现/测试保留作参考。

This update records future requirements only. It does not authorize GUI development,
new dependencies, algorithm changes, optimization, publication, or expansion of P7's
already-run checks into proof that the final editor is complete.
本更新仅记录后续要求，不启动 GUI、新依赖、算法修改、优化或发布，也不把 P7 已执行
检查扩大解释为最终编辑器已经完成。

### Provisional tone decision / 影调暂定决定 — 2026-09-26

The user provisionally excludes the current P6 shoulder curve from the formal photo
processing path. This supersedes the earlier unresolved product-role wording above.
Keep the existing implementation/tests as a reference for now; no code was removed
or rerouted in this documentation update. Future formal preview/export design must
omit this operator unless the user reopens the decision. This decision does not
exclude all tone curves, tonal-region tools or future output mapping.
用户暂定当前 P6 肩部曲线不进入正式照片处理路径，覆盖上方产品角色未定的状态。
暂保留现有实现和测试作为参考；本次文档更新未删除或改接代码。后续正式预览/导出设计
应排除此算子，除非用户重新讨论决定。本决定不排除所有曲线、亮度分区工具或未来输出映射。
