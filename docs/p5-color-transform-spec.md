# P5 色彩转换契约

状态：2026-09-21 在用户实施授权内采用。

输入为三通道相机线性 float RGB、CFA None；输出拥有独立紧密存储的工作线性 float
RGB，空间 kSrgb、传递 kLinear，名义 [0,1] 允许越界。保持尺寸、忽略 padding、
不改输入，不做白平衡、裁剪、旋转或自动提亮。

类型为 `CameraToWorkingMatrix`；元数据 helper 为 `camera_to_working_matrix(sensor)`，变换函数为 `transform_camera_rgb(image,matrix)`。
矩阵类型保存逐行九个 double 和目标空间；P5 仅支持 sRGB。元数据 helper 要求有限、
存在、第四列为零且前三列不全零；手动矩阵允许零/奇异矩阵用于诊断。按列向量
out=M*in 做有序 double 点积，转 float 前检查，无求逆/重归一，允许下溢到零。

ImageMetadata 末尾新增带默认值字段，旧未知空间仍合法，但不能自动改标。明确 sRGB
工作图像必须为线性传递；Bayer/相机状态不能带工作空间标签。编码 RGB 要求 uint16、
三通道、CFA None、[0,65535] 不越界和 sRGB 空间/传递，拒绝未知枚举及冲突组合。

错误分类沿用契约非法/非有限、数值溢出、尺寸溢出与分配异常，支持所有正尺寸。
测试覆盖基向量/方向、恒等、范围外、padding、非有限、溢出、所有权及确定性性质；
标量/组合容差如上。来源见 [preparation](p5-design-preparation.md)。

## 输出高光裁剪

函数调用为 `clip_camera_highlights(image)`。
用户授权扩大范围并重新验收。`clip_camera_highlights` 为白平衡后的 float 相机 RGB
创建紧密独立副本，各通道执行 min(channel,1)，然后在输出分支执行矩阵。保留有限
负值、拒绝非有限数及非相机状态、忽略 padding，错误/分配约定同矩阵。固定阈值基于
P3/P4 的标称 [0,1] 及绿通道归一化的相机白平衡；其他曝光/白平衡尺度须先明确渲染
尺度。本函数显式调用，矩阵及编码函数不会隐式调用它。

矩阵各行和约为 1 时，三通道均裁至 1 的核心映射到近白。仅部分通道裁剪的像素仍有
通道差异，不将所有越界像素一律涂白。该输出策略牺牲高光层次，可能改变色相或形成
硬过渡，不是高光重建，也不保证消除所有局部饱和伪影。未裁剪相机/工作图像保留，
便于后续影调或重建。不作优化/性能结论。

测试包括饱和核心与有色高光参考、源数据不变、阈值以下恒等、幂等/单调/上界、奇数/
padding、非有限与状态拒绝；需重新验收三场景。
LibRaw 高光模式支持策略显式化的
设计，本 float 参考不宣称复现其完整流水线。
来源：[LibRaw output parameters](https://www.libraw.org/docs/API-datastruct.html),
已核对本机 0.22.1 随库官方文档。

原记录数值容差：单阶段 `1e-6+1e-6*abs(reference)`；全图组合 `1e-5+1e-4*abs(reference)`。
