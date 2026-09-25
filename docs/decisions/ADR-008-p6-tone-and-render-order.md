# ADR-008: P6 Tone and Render Order / P6 影调与渲染次序

Status / 状态：Accepted for implementation, 2026-09-25 / 2026-09-25 已确认实施。
Date / 日期：2026-09-25

## Context / 背景

P5 preserves unclipped working RGB but its accepted highlight preview derives from
a separate camera-clipped branch. P6 edits must not be bypassed by that branch.
P5 保留未裁剪工作 RGB，但已验收高光预览来自另一个相机裁剪分支；该分支不能绕过 P6 编辑。

## Proposed decision / 提议决策

Use the [P6 proposal](../p6-design-preparation.md): linear exposure, read-only RGB/Y
histograms, a strength-controlled luminance shoulder (off by default), and explicit
preserve/camera-clip render modes. Retain original camera and working images.
Camera-clip applies EV to an output derivative before its hard ceiling, then matrix
and the same tone parameters. Add a space-independent tone-mapped processing state
with linear transfer; reject applying exposure/tone again to that state.
采用 P6 提案：线性曝光、只读 RGB/Y 统计、强度可控的亮度肩部（默认关闭）、显式
preserve/camera-clip 输出模式。保留原相机与工作图像；camera-clip 对输出副本先曝光、
再硬裁剪、矩阵和同一影调。增加空间无关影调后状态、线性传递，拒绝再次曝光/影调。

## Alternatives / 备选

An anchored piecewise-linear contrast curve better preserves selected midgray but
needs knot/tail decisions. Preserve-only rendering is simpler but does not retain
P5's accepted highlight appearance. Clipping before exposure permanently destroys
editable headroom; swapping in P5 output after tone loses edits. Matrix inversion
would add an unjustified conditioning contract. Neither is recommended.
固定中灰的分段线性曲线更适合保持所选中灰，但需确定节点/尾部；仅 preserve 更简单，
但不保留 P5 已验收高光外观。曝光前裁剪会永久破坏可编辑余量，影调后换入 P5 输出
会丢失编辑；矩阵求逆会引入无必要的条件数契约，均不推荐。

## Consequences / 后果

No high-quality highlight reconstruction or gamut mapping. Negative EV can reveal
saturated color; the shoulder darkens midgray and a clipped white core. Two branch
statistics must be labeled, and both branches need new visual review. The proposed
state extends ADR-007 without changing its primaries/transfer separation. ADR-007 remains valid; the user accepted this additive P6 decision on 2026-09-25.
不提供高质量高光重建或色域映射；负 EV 可能暴露饱和偏色，肩部会压暗中灰及裁白
核心。两分支统计必须标注，均需新视觉验收。新状态扩展 ADR-007、不改变原色/传递
分离。ADR-007 保持有效；用户于 2026-09-25 确认本 P6 增量决策。

Output order superseded by [ADR-009](ADR-009-p6-highlight-baseline.md).
输出顺序已由 ADR-009 替代；其余决策保留。
