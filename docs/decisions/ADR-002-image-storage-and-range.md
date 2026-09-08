# ADR-002: Image Storage and Numeric Range / 图像存储与数值范围

Status / 状态：Accepted

Date / 日期：2026-07-09

Updated / 更新：2026-07-16 (P2 contract / P2 契约)

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

## P2 Image-Model Contract / P2 图像模型契约

P2 refines the accepted storage direction into the following public contract:

P2 将已确认的存储方向细化为以下公开契约：

- `ImageMetadata` records width, height, channel count, row stride in
  **elements**, pixel format, CFA pattern, color state, and a declared nominal
  numeric interval. The declaration never clips or converts sample values.
- Width and height must be non-zero. The packed row size is
  `width * channels`; `row_stride_elements` may equal or exceed that value, so
  explicit row-end padding is supported while the owned allocation remains one
  contiguous block.
- Raw Bayer data is one-channel `uint16_t`, requires a concrete CFA pattern,
  declares a nominal code-value interval `[0, maximum]` within the storage
  type, and marks out-of-range samples as disallowed by contract.
- Linear Bayer data is one-channel `float`; linear camera RGB and linear
  working RGB are three-channel interleaved `float`. Linear states declare the
  nominal `[0, 1]` interval while allowing values outside it.
- RGB states require `CFA=None`; Bayer states require one of RGGB, BGGR, GRBG,
  or GBRG. P2 does not add display-encoded storage or external-memory views.
- Construction validates every enum value, state combination, packed-row
  multiplication, stride-times-height multiplication, element count, byte
  count, and supplied storage size before accepting the object.
- Invalid metadata or storage size throws `std::invalid_argument`; arithmetic
  overflow throws `std::length_error`; pixel or row bounds errors throw
  `std::out_of_range`; asking for a storage type that does not match the pixel
  format throws `std::logic_error`.
- Construction validates the storage model and declared state but does not
  scan or modify individual samples. Code-value and finite-value requirements
  remain the responsibility of the processing-stage boundary that consumes
  the buffer.

- `ImageMetadata` 记录宽、高、通道数、以**元素数**计的行跨度、像素格式、
  CFA 排列、色彩状态和声明的名义数值区间。该声明不会裁剪或转换样本值。
- 宽和高必须非零。紧密行大小为 `width * channels`；
  `row_stride_elements` 可以等于或大于该值，因此允许显式行尾 padding，
  同时整个 owned allocation 仍然是一个连续内存块。
- RAW Bayer 数据为单通道 `uint16_t`，必须带具体 CFA 排列，声明位于存储
  类型范围内的名义码值区间 `[0, maximum]`，并把超范围样本标记为契约不允许。
- 线性 Bayer 数据为单通道 `float`；线性相机 RGB 与线性工作 RGB 为三通道
  交错 `float`。线性状态声明名义 `[0, 1]` 区间，同时允许超范围值。
- RGB 状态要求 `CFA=None`；Bayer 状态要求 RGGB、BGGR、GRBG 或 GBRG。
  P2 不加入显示编码存储或外部内存 view。
- 构造时先校验所有枚举值、状态组合、紧密行乘法、stride 与 height 乘法、
  元素数、字节数和传入 storage 大小，全部通过后才接受对象。
- 非法元数据或 storage 大小抛出 `std::invalid_argument`；算术溢出抛出
  `std::length_error`；像素或行访问越界抛出 `std::out_of_range`；请求与
  像素格式不匹配的存储类型抛出 `std::logic_error`。
- 构造只验证存储模型和声明状态，不扫描或修改单个样本。码值范围与有限值要求
  仍由实际消费该 buffer 的处理阶段边界负责。

## Sources / 资料依据

- The C++ working draft specifies `std::vector` as a contiguous container:
  <https://eel.is/c++draft/vector.overview>.
- CMake 3.24 documents static-library source registration with
  `add_library()`:
  <https://cmake.org/cmake/help/v3.24/command/add_library.html>.
- CMake 3.24 documents executable-based GoogleTest discovery with
  `gtest_discover_tests()`:
  <https://cmake.org/cmake/help/v3.24/module/GoogleTest.html>.

- C++ 工作草案规定 `std::vector` 是连续容器。
- CMake 3.24 官方文档说明了使用 `add_library()` 注册静态库源文件。
- CMake 3.24 官方文档说明了 `gtest_discover_tests()` 基于测试可执行文件发现
  GoogleTest 用例的行为。
