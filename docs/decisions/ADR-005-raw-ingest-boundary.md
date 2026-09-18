# ADR-005: RAW Ingest Boundary / RAW 读取边界

Status / 状态：Accepted within authorized P3 implementation / 在已授权 P3 实施范围内采用

Date / 日期：2026-09-10

## Context / 背景

LibRaw owns decoded storage and exposes camera-dependent metadata. Downstream
algorithms need stable active-area ownership and explicit code-value references.

LibRaw 持有解码存储并暴露相机相关元数据，下游算法需要稳定的有效区域所有权和显式码值参考。

## Decision / 决定

Adopt the [P3 contract](../p3-raw-normalization-spec.md). Keep LibRaw types private;
copy the active area once. Store sensor metadata separately from ImageMetadata.
Use four positional black/white references and double arithmetic before float
output. Reject unsupported spatial black tables and floating RAW. Preserve
unapplied optional WB/matrix metadata, orientation and reported bit depth.

采用 [P3 契约](../p3-raw-normalization-spec.md)。LibRaw 类型不进入公开头文件，有效
区域复制一次；传感器元数据与 ImageMetadata 分离。使用四个位置黑白参考，double
运算后输出 float。拒绝不支持的空间黑电平表和浮点 RAW；保存未应用的可选白平衡/
矩阵、方向和报告位深。

## Alternatives / 备选方案

Zero-copy views couple lifetime to LibRaw; dcraw_process hides ISP stages;
guessing metadata broadens unverified camera support. These are rejected.

零拷贝 view 将生命周期绑定到 LibRaw；dcraw_process 隐藏 ISP 阶段；猜测元数据会
扩大未经验证的相机支持，均不采用。

## Consequences / 影响

The copy costs one active-image allocation but permits safe decoder destruction.
Compatibility stays narrow and real-camera evidence remains pending until sample
authorization. Existing P2 types and error conventions remain intact. Sources and
exact numeric/layout rules are centralized in the P3 specification.

复制增加一次有效图像分配，但允许安全销毁解码器。兼容范围保持有限，真实相机证据
等待样张授权。保留 P2 类型及错误约定，来源和精确数值/布局规则集中在 P3 规格中。
