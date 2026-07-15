# ADR-002: Image Storage and Numeric Range / 图像存储与数值范围

Status / 状态：Accepted

Date / 日期：2026-07-09

## Context / 背景

The same numeric array can represent raw mosaic samples, linear camera RGB,
working RGB, or display-encoded RGB. Hidden layout or range assumptions would
make correct-looking but incorrect processing easy.

同一数值数组可能表示 RAW 马赛克、线性相机 RGB、工作 RGB 或显示编码 RGB。
隐含的布局和范围假设很容易产生“看起来正确、实际错误”的处理结果。

## Decision / 决定

- Source Bayer storage is owned, contiguous, row-major `uint16_t`.
- Working storage is owned, contiguous, row-major `float`.
- Initial RGB storage is interleaved RGB for implementation clarity.
- Width, height, channel count, pixel format, CFA pattern, color state, and
  row stride are explicit metadata.
- Black and white references define a nominal `[0, 1]` interval.
- Working values may be below `0` or above `1` when a stage contract permits
  it. White balance, matrix conversion, and exposure never clip implicitly.
- Clipping and display encoding are explicitly named stages.

- Bayer 源数据使用拥有所有权、连续、行优先的 `uint16_t` 存储。
- 工作数据使用拥有所有权、连续、行优先的 `float` 存储。
- 第一版 RGB 使用交错布局，以保持实现清晰。
- 宽、高、通道数、像素格式、CFA 排列、色彩状态和行跨度均为显式元数据。
- 黑白电平定义名义上的 `[0, 1]` 区间。
- 阶段契约允许时，工作值可以小于 `0` 或大于 `1`。白平衡、矩阵转换和
  曝光不得隐式裁剪。
- 裁剪和显示编码必须是显式命名的阶段。

## Consequences / 影响

The first implementation copies data instead of exposing zero-copy views and
uses a simple RGB layout instead of early SIMD-oriented storage. In exchange,
ownership, interpretation, and test expectations remain clear.

第一版会复制数据，而不是直接设计零拷贝 view；也会使用简单 RGB 布局，而不是过早采用
面向 SIMD 的存储。作为交换，数据所有权、含义和测试预期保持清晰。
