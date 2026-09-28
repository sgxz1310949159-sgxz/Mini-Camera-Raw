# ISP 流水线契约

状态：阶段零基线

日期：2026-07-09

## 目的

每个处理阶段都必须显式说明数据含义。实现内部可以拆分或合并阶段，但公开行为必须
保持以下状态转换。

## 通用规则

- 在阶段边界验证尺寸、行跨度、像素格式、CFA 排列、色彩状态和传递函数状态。

- 线性值与显示编码值之间不得隐式转换。

- 在公开边界，NaN 和无穷值均视为错误。

- 工作值不会自动裁剪到 `[0, 1]`。

- 阶段要么返回有效结果，要么返回描述性错误；不得静默接受部分输出。

## 计划阶段表

| 阶段 | 输入 | 输出 | 范围与不变量 |
|---|---|---|---|
| RAW 读取 | Sony A7C II `.ARW` path | `uint16_t` Bayer 与元数据 | 只解码，不启用隐藏白平衡、去马赛克、伽马或自动提亮 |
| 黑白电平归一化 | Bayer code values, per-channel black level, white level | Linear `float` Bayer | `(raw - black) / (white - black)`；分母必须为正，结果可超出 `[0,1]` |
| 坏点策略 | Normalized linear Bayer | Normalized linear Bayer | 可选且显式；第一版参考路径不做校正 |
| 白平衡 | Linear Bayer plus positive finite channel gains | Gain-adjusted linear Bayer | CFA 相位不变；结果可大于 `1` |
| 去马赛克 | Linear Bayer with known CFA pattern | Linear camera RGB `float` | 尺寸不变、3 通道交错、不隐式裁剪 |
| 相机到工作色彩 | Linear camera RGB plus documented matrix | Linear working RGB | 记录矩阵方向和光源；允许负值与超一值 |
| 曝光 | Linear working RGB plus EV | Linear working RGB | 乘以 `2^EV`；不裁剪 |
| 影调映射 | Linear working RGB | Tone-mapped RGB with declared state | 基线曲线必须单调；记录输出范围 |
| 显示编码 | Display-ready linear RGB | sRGB-encoded RGB | 应用明确传递函数；在此显式裁剪到 `[0,1]` |
| 文件输出 | Encoded RGB | PNG or TIFF | 记录位深、色彩配置和量化策略 |

## RAW 读取所需元数据

- 图像尺寸与有效区域

- CFA 排列与相位

- 源位深

- 分通道或分 CFA 位置的黑电平

- 白电平

- 可用时记录相机白平衡增益

- 可用时记录相机颜色矩阵及其方向

- LibRaw 版本与相关处理参数

## 实现门槛

实现某个阶段前，必须在对应任务或设计笔记中补充精确公式、边界情况、错误行为和
测试 oracle。本表只定义共享状态契约，不能替代针对具体算法的推导。
