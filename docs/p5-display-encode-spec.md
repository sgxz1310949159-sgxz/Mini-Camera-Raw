# P5 display-encode: sRGB output encoding / sRGB 输出编码

Status / 状态：Accepted within implementation authorization, 2026-09-21 / 在用户实施授权内采用。

`encode_srgb16(image)` takes explicitly sRGB/linear float LinearWorkingRgb only.
It returns packed uint16 EncodedRgb with sRGB space/transfer and [0,65535] without
excursions. Validate finite active samples, clamp independent output to [0,1], use
12.92*x at x<=0.0031308, else 1.055*pow(x,1/2.4)-0.055; round via floor(65535*s+0.5).
No float intermediate encoding buffer or second quantization. Preserve source,
size and row order; padding ignored. Unknown space and repeated encoding rejected.
`encode_srgb16` 只接受明确 sRGB/线性的工作 float 图像，返回紧密 uint16 编码 RGB，
空间/传递均为 sRGB，值域 [0,65535]。有效像素须有限，独立输出裁剪后按上述公式
编码/舍入；没有中间 float 编码缓存或二次量化。不改原图、尺寸和行序，忽略 padding，
拒绝未知空间及重复编码。

P6 exposure/tone operations will precede this output boundary. Future other output
spaces need actual transforms and matching transfer/profile support; metadata alone
cannot convert colors. Float export is a separate future path.
未来 P6 在本输出边界之前，其他空间需真实变换和匹配编码/标识，浮点导出另走独立路径。

Tests: fixed endpoints/threshold neighbors, inverse-transfer generated oracle within
one integer code at representational boundaries, monotonicity, source preservation,
NaN/Inf, odd/padded inputs and invalid states. Errors follow color-transform.
测试固定端点/分段邻值、逆传递生成参考（表示边界允许一个整数码差）、单调性、原图
保留、非有限、奇数/padding 和错误状态；异常沿用色彩转换契约。
Source / 来源：[ICC sRGB](https://registry.color.org/rgb-registry/srgb).

## Highlight rendering boundary / 高光渲染边界

Per-channel output clipping is a numeric encoding boundary, not a complete
sensor-saturation rendering policy. Real-sample validation exposed magenta in
saturated backlit highlights in the original direct-encoding preview. The revised
explicit camera-clip output passed coarse user visual acceptance. Any highlight
rendering stage must precede encoding and preserve the original linear working
image. The authorized explicit camera-domain clip is specified in the color-transform contract; see
[P5 verification](../tasks/p5-verification-review.md).
逐通道输出裁剪是数值编码边界，并非完整的传感器饱和渲染策略。真实逆光样张暴露
高光粉紫；补充相机域显式裁剪后的新版已通过用户粗略视觉验收。高光渲染应位于编码之前并保留原线性工作图像，已授权的相机域显式裁剪详见色彩矩阵契约与上述验证记录。
