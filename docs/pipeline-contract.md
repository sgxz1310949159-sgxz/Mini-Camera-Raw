# ISP Pipeline Contract / ISP 流水线契约

Status / 状态：Stage 0 baseline / 阶段零基线

Date / 日期：2026-07-09

## Purpose / 目的

Every processing stage must make its data meaning explicit. An implementation
may split or combine stages internally, but its public behavior must preserve
the state transitions below.

每个处理阶段都必须显式说明数据含义。实现内部可以拆分或合并阶段，但公开行为必须
保持以下状态转换。

## Common Rules / 通用规则

- Dimensions, row stride, pixel format, CFA pattern, color state, and transfer
  state are validated at stage boundaries.
- Linear and display-encoded values are never converted implicitly.
- NaN and infinity are errors at public boundaries.
- Working values are not automatically clamped to `[0, 1]`.
- A stage either returns a valid result or a descriptive error; partial output
  is not silently accepted.

- 在阶段边界验证尺寸、行跨度、像素格式、CFA 排列、色彩状态和传递函数状态。
- 线性值与显示编码值之间不得隐式转换。
- 在公开边界，NaN 和无穷值均视为错误。
- 工作值不会自动裁剪到 `[0, 1]`。
- 阶段要么返回有效结果，要么返回描述性错误；不得静默接受部分输出。

## Planned Stage Table / 计划阶段表

| Stage / 阶段 | Input / 输入 | Output / 输出 | Range and invariants / 范围与不变量 |
|---|---|---|---|
| RAW ingest / RAW 读取 | Sony A7C II `.ARW` path | `uint16_t` Bayer plus metadata / Bayer 与元数据 | LibRaw decoding only; no hidden white balance, demosaic, gamma, or auto-bright / 只解码，不启用隐藏白平衡、去马赛克、伽马或自动提亮 |
| Black/white normalization / 黑白电平归一化 | Bayer code values, per-channel black level, white level | Linear `float` Bayer | `(raw - black) / (white - black)`; denominator must be positive; values may leave `[0,1]` / 分母必须为正，结果可超出 `[0,1]` |
| Defect policy / 坏点策略 | Normalized linear Bayer | Normalized linear Bayer | Optional and explicit; no correction in the first reference path / 可选且显式；第一版参考路径不做校正 |
| White balance / 白平衡 | Linear Bayer plus positive finite channel gains | Gain-adjusted linear Bayer | CFA phase unchanged; values may exceed `1` / CFA 相位不变；结果可大于 `1` |
| Demosaic / 去马赛克 | Linear Bayer with known CFA pattern | Linear camera RGB `float` | Same dimensions; three interleaved channels; no implicit clipping / 尺寸不变、3 通道交错、不隐式裁剪 |
| Camera-to-working color / 相机到工作色彩 | Linear camera RGB plus documented matrix | Linear working RGB | Matrix direction and illuminant recorded; negative and above-one values may occur / 记录矩阵方向和光源；允许负值与超一值 |
| Exposure / 曝光 | Linear working RGB plus EV | Linear working RGB | Multiply by `2^EV`; no clipping / 乘以 `2^EV`；不裁剪 |
| Tone mapping / 影调映射 | Linear working RGB | Tone-mapped RGB with declared state | Curve must be monotonic for the baseline; output range documented / 基线曲线必须单调；记录输出范围 |
| Display encoding / 显示编码 | Display-ready linear RGB | sRGB-encoded RGB | Apply documented transfer function; clipping to `[0,1]` is explicit here / 应用明确传递函数；在此显式裁剪到 `[0,1]` |
| File output / 文件输出 | Encoded RGB | PNG or TIFF | Bit depth, color profile, and quantization policy recorded / 记录位深、色彩配置和量化策略 |

## Metadata Required From RAW Ingest / RAW 读取所需元数据

- image dimensions and active area
- CFA pattern and phase
- source bit depth
- per-channel or per-pattern black levels
- white level
- camera white-balance multipliers, when available
- camera color matrix and its documented direction, when available
- LibRaw version and relevant processing flags

- 图像尺寸与有效区域
- CFA 排列与相位
- 源位深
- 分通道或分 CFA 位置的黑电平
- 白电平
- 可用时记录相机白平衡增益
- 可用时记录相机颜色矩阵及其方向
- LibRaw 版本与相关处理参数

## Implementation Gate / 实现门槛

Before implementing a stage, add its exact formula, edge cases, error behavior,
and test oracle to the corresponding task or design note. This table is a
shared state contract, not a substitute for algorithm-specific derivation.

实现某个阶段前，必须在对应任务或设计笔记中补充精确公式、边界情况、错误行为和
测试 oracle。本表只定义共享状态契约，不能替代针对具体算法的推导。
