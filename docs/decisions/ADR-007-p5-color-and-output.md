# ADR-007: P5 Color and Output / P5 色彩与输出

Status / 状态：Accepted for implementation on 2026-09-21 / 2026-09-21 已接受实施。
Date / 日期：2026-09-21

## Context / 背景

P4 produces white-balanced camera-linear RGB. The user confirmed P5 learning,
sRGB-first output flow, future color-space extensibility and stage implementation.
P5 needs a named linear working space, a separate output boundary and one file path.
P4 输出白平衡后的相机线性 RGB。用户已确认 P5 学习完成、首版 sRGB 输出流程、
未来色彩空间扩展及阶段实施。P5 需要明确工作空间、独立输出边界和一种文件路径。

## Decision / 决定

Three modules: color-transform → display-encode → png-output. The stable module id
display-encode means sRGB output encoding, not a GUI operation. P5 does not implement
P6 exposure/tone or a processing CLI. Source files retain display_encode naming.
三个模块依次为色彩转换、输出编码、PNG 写出。稳定 id display-encode 表示 sRGB
输出编码，并非 GUI 操作，文件名保留 display_encode。P5 不实现 P6 影调/曝光或处理 CLI。

Use D65 linear sRGB as the first working space. A row-major double 3x3 matrix acts
on RGB column vectors. For the supported three-color LibRaw metadata, require zero
fourth column and use the first three columns; missing/invalid metadata is an error.
Do not repeat WB, invert/renormalize matrices or clip linear values.
首个工作空间为 D65 线性 sRGB，逐行 double 3x3 矩阵左乘 RGB 列向量。受支持三颜色
LibRaw 元数据要求第四列为零，取前三列；缺失/非法时报错。不重复白平衡、不求逆/
重归一矩阵，不裁剪线性值。

Extend ImageMetadata with trailing defaulted RgbColorSpace and TransferFunction
fields; add generic kEncodedRgb processing state. Old unspecified images remain
source-compatible, but output encoding requires explicit sRGB/linear metadata.
This is not a binary ABI compatibility promise. Other spaces require their own
implemented transforms/encoding and validation; changing tags never converts color.
ImageMetadata 末尾增量增加有默认值的空间和传递字段，处理状态增加通用 kEncodedRgb。
旧未知空间图像保留源码兼容，输出编码要求明确 sRGB/linear；不承诺二进制 ABI
兼容。其他空间需实现对应变换/编码及校验，改标签不构成转换。

Keep working pixels unmodified. sRGB output encoding validates finite samples,
clamps its own output to [0,1], applies the piecewise sRGB transfer and quantizes once
into uint16 [0,65535]. Future P6 operations precede this boundary; future output
spaces may need a linear conversion before gamut handling/encoding. Linear floating
export can bypass it. Working space, output space and transfer remain distinct.
保留工作像素不变，sRGB 输出编码校验有限值，在自身输出上裁剪到 [0,1]，分段编码
并量化一次到 uint16。未来 P6 在此边界之前，其他输出空间可能先需线性变换，再做
色域处理/编码；线性浮点导出可绕过此边界。工作空间、输出空间、传递分别表达。

Use installed libpng/zlib through PNG::PNG to write RGB16 PNG with sRGB relative
colorimetric identification, no alpha/interlace/private EXIF. Use conventional APIs,
explicit big-endian bytes and filter-none for the scalar reference. A private trivial
helper contains longjmp; C++ resource ownership remains outside it. POSIX exclusive
creation rejects existing targets. Normal write errors clean newly created partial
files when identity still matches. The caller keeps the output path stable; this
is not safe against concurrent pathname mutation, crash-atomic publication or a
cleanup filesystem failure. Failed identity inspection may leave a new file.
通过 PNG::PNG 使用系统 libpng/zlib，写 RGB16、sRGB 相对比色标识、无 alpha/交错/
私人 EXIF。常规 API、显式大端字节，标量参考使用 filter-none。longjmp 限于平凡
私有 helper，C++ 所有权在外层。POSIX 排他创建拒绝已有目标；普通写入失败且身份
匹配时清理新文件。调用方保持路径稳定，不保证并发路径修改、崩溃原子发布或文件
系统清理失败时的安全完成；身份检查失败可能保留新文件。

## Alternatives / 替代方案

Dedicated sRGB processing-state variants were rejected after the extensibility
request. A general ICC engine or other working spaces are premature; the metadata
boundary supports adding them later. RGB8 discards precision earlier; TIFF is useful
for future float export. Custom PNG serialization adds compression/checksum costs.
Simplified libpng APIs treat 16-bit data as linear, so cannot transparently write our
already encoded RGB16. Automatic overwriting risks existing files.
扩展要求提出后不采用 sRGB 专用处理状态。通用 ICC 引擎或其他工作空间暂不实现，
保留元数据边界。RGB8 更早丢精度，TIFF 可供未来浮点导出；自写 PNG 增加压缩/
校验成本。简化 libpng API 将 16 位输入视为线性，不适用于本已编码 RGB16；
自动覆盖危及已有文件。

## Consequences / 后果

Working values remain reusable; encoded output is clipped and irreversible. Current
writer platform baseline is macOS/Linux. Saturated sensor highlights can become
colored after WB/matrix and channel clipping; no highlight reconstruction is claimed.
Agent numeric/visual checks and user visual acceptance are separate. No publication
authorization is implied.
工作值可重用，编码输出裁剪后不可逆。写入平台基线为 macOS/Linux。传感器饱和
高光经白平衡/矩阵/逐通道裁剪可能偏色，不宣称高光重建。Agent 数值/视觉检查与
用户视觉验收分开，不外推发布授权。

Sources and contracts / 来源与契约：
- [Color transform](../p5-color-transform-spec.md)
- [sRGB output encoding](../p5-display-encode-spec.md)
- [PNG output](../p5-png-output-spec.md)
- [Preparation and source review](../p5-design-preparation.md)
- [Verification](../../tasks/p5-verification-review.md)

## Accepted highlight extension / 已接受的高光扩展 — 2026-09-21

User rejected magenta overexposed rendering and authorized addressing it in P5.
Keep the matrix/encoder pure and add explicit clip_camera_highlights on an owned
camera-RGB output copy, min(channel,1), before the output matrix. Preserve original
camera and linear-working data. This conservative hard-clip mode trades highlight
headroom for a neutral fully-clipped core with the supported neutral-preserving
matrix. It does not reconstruct lost detail or indiscriminately paint every
out-of-gamut pixel white. Alternatives: retain the original direct-encode behavior
(useful diagnostic but rejected as this sample's final rendering); saturation masks,
blending or reconstruction (future separately specified algorithms). Partial clips
may still shift hue; no universal highlight-quality claim. See the updated color
transform contract and repeat visual verification.
用户不接受过曝粉紫显示，授权本阶段处理。保持矩阵/编码原语纯粹，新增显式
clip_camera_highlights，在独立相机 RGB 输出副本上逐通道 min(channel,1)，再矩阵
转换，原相机及线性工作图像保留。对于支持的保中性矩阵，此保守硬裁剪以高光余量
换取完全裁剪核心的中性，不恢复细节、不把所有越界像素一律涂白。备选是原直接
编码（适合诊断，但该样张成片不接受），或饱和掩码/混合/重建（以后单独定义）。
局部裁剪仍可能偏色，不保证通用高光质量，详见更新契约及重新验收记录。
