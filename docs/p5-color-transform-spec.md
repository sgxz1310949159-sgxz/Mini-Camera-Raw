# P5 color-transform / P5 色彩转换契约

Status / 状态：Accepted within implementation authorization, 2026-09-21 / 在用户实施授权内采用。

Input: float LinearCameraRgb, CFA None, three RGB channels, unspecified camera
primaries. Output: owned packed float LinearWorkingRgb, RgbColorSpace::kSrgb,
TransferFunction::kLinear, nominal [0,1] allowing excursions. Sizes preserved,
padding ignored, input immutable. No WB, clipping, rotation or auto brightness.
输入为三通道相机线性 float RGB、CFA None；输出拥有独立紧密存储的工作线性 float
RGB，空间 kSrgb、传递 kLinear，名义 [0,1] 允许越界。保持尺寸、忽略 padding、
不改输入，不做白平衡、裁剪、旋转或自动提亮。

`CameraToWorkingMatrix` holds row-major array<double,9> and destination space.
P5 supports only sRGB; unsupported/unspecified destinations are errors.
`camera_to_working_matrix(sensor)` requires present finite 3x4 metadata, zero fourth
column, nonzero 3x3. `transform_camera_rgb(image,matrix)` computes column-vector
out=M*in with ordered double dot products, checked before float conversion.
Explicit manual matrices may include zero/singular matrices for diagnostics;
no inversion or row normalization is performed. Underflow to zero is allowed.
矩阵类型保存逐行九个 double 和目标空间；P5 仅支持 sRGB。元数据 helper 要求有限、
存在、第四列为零且前三列不全零；手动矩阵允许零/奇异矩阵用于诊断。按列向量
out=M*in 做有序 double 点积，转 float 前检查，无求逆/重归一，允许下溢到零。

ImageMetadata gains trailing defaulted color-space/transfer fields. Old unspecified
images remain valid; explicitly sRGB working images require linear transfer. Bayer/
camera states cannot carry working-space tags. Encoded RGB requires uint16, three
channels, CFA None, exact nominal [0,65535], no excursions, sRGB space/transfer.
Unknown enums and inconsistent pairs are rejected; no legacy image is relabeled.
ImageMetadata 末尾新增带默认值字段，旧未知空间仍合法，但不能自动改标。明确 sRGB
工作图像必须为线性传递；Bayer/相机状态不能带工作空间标签。编码 RGB 要求 uint16、
三通道、CFA None、[0,65535] 不越界和 sRGB 空间/传递，拒绝未知枚举及冲突组合。

Errors: invalid_argument for contract/nonfinite input, overflow_error for arithmetic,
length_error for sizes; allocation failures propagate. All positive sizes supported.
Tests: basis/direction, identity, negatives/highlights, padding, NaN/Inf per component,
overflow, ownership, deterministic generated linearity/reference cases. Tolerance:
1e-6 absolute + 1e-6 relative; composed stages 1e-5 + 1e-4 relative.
错误分类沿用契约非法/非有限、数值溢出、尺寸溢出与分配异常，支持所有正尺寸。
测试覆盖基向量/方向、恒等、范围外、padding、非有限、溢出、所有权及确定性性质；
标量/组合容差如上。来源见 [preparation](p5-design-preparation.md)。

## Output highlight clipping / 输出高光裁剪

Accepted scope extension, 2026-09-21: user requested highlight handling and repeat
visual acceptance. `clip_camera_highlights(image)` creates a packed owned copy of
white-balanced float LinearCameraRgb, applying min(channel,1) before the output
branch's matrix transform. It leaves finite negative values unchanged, rejects
NaN/Inf and non-camera states, and ignores padding. Error/allocation conventions
match transform_camera_rgb. The fixed ceiling assumes the P3/P4 nominal [0,1]
scale and green-normalized camera WB; callers using other exposure/WB scales must
establish their rendering scale first. It is explicit and never called implicitly
by transform_camera_rgb or encode_srgb16.
用户授权扩大范围并重新验收。`clip_camera_highlights` 为白平衡后的 float 相机 RGB
创建紧密独立副本，各通道执行 min(channel,1)，然后在输出分支执行矩阵。保留有限
负值、拒绝非有限数及非相机状态、忽略 padding，错误/分配约定同矩阵。固定阈值基于
P3/P4 的标称 [0,1] 及绿通道归一化的相机白平衡；其他曝光/白平衡尺度须先明确渲染
尺度。本函数显式调用，矩阵及编码函数不会隐式调用它。

For a neutral-preserving matrix (row sums approximately 1), clipping all camera
channels to 1 maps a fully saturated core to approximately white. Partially clipped
pixels retain their remaining channel differences; this does not simply whiten any
out-of-gamut pixel. It discards highlight detail in this output branch and can shift
hue or create hard transitions. It is not highlight reconstruction or a guarantee
against all partial-saturation artifacts. Unclipped camera/working images remain
available for later tone/reconstruction methods. No optimization/performance claim.
矩阵各行和约为 1 时，三通道均裁至 1 的核心映射到近白。仅部分通道裁剪的像素仍有
通道差异，不将所有越界像素一律涂白。该输出策略牺牲高光层次，可能改变色相或形成
硬过渡，不是高光重建，也不保证消除所有局部饱和伪影。未裁剪相机/工作图像保留，
便于后续影调或重建。不作优化/性能结论。

Tests: known saturated-core and colored-highlight vectors; source immutability;
identity below ceiling, idempotence, monotonicity and upper bound on deterministic
odd/padded cases; NaN/Inf and state rejection. Visual acceptance requires new
three-scene previews. LibRaw's documented highlight modes motivate an explicit
policy, but this float reference is not claimed to reproduce its full pipeline.
测试包括饱和核心与有色高光参考、源数据不变、阈值以下恒等、幂等/单调/上界、奇数/
padding、非有限与状态拒绝；需重新验收三场景。LibRaw 高光模式支持策略显式化的
设计，本 float 参考不宣称复现其完整流水线。
Source / 来源：[LibRaw output parameters](https://www.libraw.org/docs/API-datastruct.html),
installed 0.22.1 documentation / 已核对本机 0.22.1 随库官方文档。
