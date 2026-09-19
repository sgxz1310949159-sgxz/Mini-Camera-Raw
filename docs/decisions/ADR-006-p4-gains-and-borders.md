# ADR-006: P4 Gains and Borders / P4 增益与边界

Status / 状态：Accepted on 2026-09-19 / 2026-09-19 已接受。

Date / 日期：2026-09-18

## Context / 背景

P3 preserves four spatial WB values and out-of-range linear Bayer samples. P4 must
make gain scaling, green identity and missing boundary neighbors reproducible.
P3 保留四个空间位置的白平衡值及超范围线性 Bayer 样本。P4 必须使增益缩放、两个
绿色位置及边界缺失邻居处理可复现。

## Proposal / 提案

Use literal manual spatial gains, with a separate green-mean normalization helper
for camera gains. Apply before interpolation and preserve both greens independently.
Average in-bounds neighbors at borders; demosaic requires at least 2x2. Keep existing
ImageBuffer states; document caller responsibility to apply WB once. Detailed API,
formulas and sources: [P4 contract](../p4-white-balance-demosaic-spec.md).

手动空间增益按原值应用，相机增益另用绿色均值归一 helper。插值前应用，独立保留
两个绿色。边界仅平均图内邻居，去马赛克最小 2x2。保留现有 ImageBuffer 状态，明确
调用方保证白平衡只应用一次。详细 API、公式及来源见上述 P4 契约。

## Alternatives / 替代方案

First-green or minimum-gain references change global exposure convention. Forcing
both greens to one loses their calibration ratio. Automatic WB adds out-of-scope
estimation. CFA-preserving reflection is valid but weights borders differently;
mosaic coordinate clamping can use the wrong color. A 1D fallback invents unobserved
colors. A new WB state enforces ordering but expands the image-model change.

第一个绿色或最小增益参考会改变整体曝光约定；强制两绿色均为一会丢失校准比例。
自动白平衡引入范围外估计。保持 CFA 的反射可行，但边缘权重不同；马赛克坐标钳制
可能使用错误颜色。单行/列回退会虚构未测量颜色。新增白平衡状态可强制顺序，但
扩大图像模型改动。

## Consequences / 后果

Small scalar stages remain hand-checkable. Borders need not reproduce ramps exactly.
Current types cannot reject repeated WB. Camera-linear output is not display-ready;
P5 must use a consistent gain convention. The user accepted all three choices on 2026-09-19.

标量阶段可手算核对，边界不保证精确恢复渐变。现有类型无法拒绝重复白平衡。
相机线性输出不可直接视为显示图像；P5 须沿用一致增益约定。用户于 2026-09-19 接受全部三项选择。
