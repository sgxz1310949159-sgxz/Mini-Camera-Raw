# P4 Entry and Learning Handoff / P4 启动与学习交接

Date / 日期：2026-09-18

Status: entry preparation; learning status and detailed design are not yet confirmed.

状态：启动准备；学习状态及详细设计尚未确认。

## Task Transfer / 任务转移

The user requested a fresh, independent P4 task on 2026-09-18, using GPT-6 Astra
with medium reasoning. Commit the accepted P3 baseline before creating P4.
Create a new task with this concise handoff, not a fork of the P3 conversation.
P4 depends on committed P3 code, not uncommitted work or inherited chat history.

用户于 2026-09-18 要求新建独立 P4 任务，使用 GPT-6 Astra、medium 推理强度。
先提交已验收的 P3 基线，再新建任务，仅交接必要信息，不分叉 P3 对话。
P4 依赖已提交的 P3 代码，不依赖未提交修改或继承的聊天记录。

The accepted P3 evidence includes 42/42 clean and sanitizer tests, a no-tests
build, three real-sample numeric checks, nine inspected diagnostic images, and
user-confirmed learning plus coarse visual acceptance. Reverify any evidence
affected by P4 changes; do not repeat P3 learning acceptance.

P3 既有验收证据包括普通及 sanitizer 测试 42/42、关闭测试的构建、三张真实样张
数值检查、九张诊断图检查，以及用户确认的学习完成和粗略视觉验收。P4 修改影响
的证据需要复验；不重复询问 P3 学习验收。

Read the three previously authorized originals in place; their private paths
are passed in the task message. Keep derived data ignored and do not recreate
removed duplicate copies. Diagnostic artifacts are in `build/p3-visual/`.

直接读取三张此前已授权原片，私人路径通过任务消息交接。派生数据保持忽略，不
重新创建已清理的副本。诊断材料位于 `build/p3-visual/`。

Tooling note: system git/Xcode may request license acceptance. The bundled
fallback git and `DEVELOPER_DIR=/Library/Developer/CommandLineTools` worked for
P3 diagnostics; do not accept licenses on behalf of the user. Local LibRaw is
0.22.1. Existing build evidence lives in `build/p3-clean`, `build/p3-sanitized`,
`build/p3-no-tests` and `build/p3-real`.

工具提示：系统 git/Xcode 可能提示接受许可。P3 诊断时内置 fallback git 和
`DEVELOPER_DIR=/Library/Developer/CommandLineTools` 可用，不代用户接受许可。
本地 LibRaw 为 0.22.1；既有构建证据目录如上。

## Starting Point / 起点

P3 returns owned float LinearBayer with an explicit CFA pattern and preserved
negative/above-one values. SensorMetadata can carry four spatial camera WB gains;
no gain, interpolation, rotation or color matrix has been applied. P3 local
numeric, diagnostic and user acceptance evidence is in `p3-verification-review.md`.
Main integration and remote CI are separate work and are not implied by local
acceptance or a local commit.

P3 输出拥有所有权的 float LinearBayer，显式保留 CFA、负值及大于一的值。
SensorMetadata 可带四个空间位置的相机白平衡增益；尚未应用增益、插值、旋转或色彩矩阵。
P3 本地数值、诊断和用户验收证据见 `p3-verification-review.md`。
主分支集成及远端 CI 是独立事项，不由本地验收或本地提交推定完成。

## Learning Sequence / 学习顺序

Use one continuous data flow: normalized CFA measurements → gain calibration →
missing-color interpolation → camera-linear RGB. Explain why a mosaic sample has
one measured color, why gain multiplication can exceed one, and why the result
is still not display-ready sRGB. Check a hand-solvable CFA patch before coding.
Use the independent learning task unless the user requests learning here; do not
copy private notes or infer P4 mastery from P3 completion.

按连续数据流学习：归一化 CFA 测量值 → 增益校正 → 缺失颜色插值 → 相机线性 RGB。
解释每个马赛克样本为什么只测量一种颜色、增益相乘为什么可能超过一，以及结果为什么
还不是可显示的 sRGB。实现前用可手算的 CFA 小块检验理解。沿用独立学习任务，除非用户
要求在此学习；不复制私人笔记，也不从 P3 完成推定 P4 已掌握。

## Design Before Implementation / 实现前设计

The approved roadmap already separates two capabilities: positive finite CFA
gain application, followed by scalar bilinear demosaic. Prepare the detailed P4
specification after learning status is established. Resolve explicitly:

已有路线图将 P4 分为两个能力：按 CFA 应用有限正增益，再执行标量双线性去马赛克。
确认学习状态后准备 P4 详细规格，显式确定：

- Gain source, normalization convention, missing-metadata behavior and the two
  green positions / 增益来源、归一约定、缺失元数据行为及两个绿色位置。
- Input/output color states, CFA mapping and gain placement relative to
  interpolation / 输入输出色彩状态、CFA 映射及增益相对插值的位置。
- Border policy, minimum supported dimensions and odd sizes / 边界策略、最小支持尺寸及奇数尺寸。
- Negative/above-one preservation, finite-value checks and overflow behavior /
  负值与超一值保留、有限性校验及溢出行为。
- Public API ownership, stride handling and tolerances / 公开 API 所有权、stride 处理及容差。

## Verification Targets / 验证目标

Test neutral-gain identity, known CFA hand vectors, constant per-channel fields,
channel impulses, ramps, all four Bayer phases, borders, odd/small dimensions,
invalid gains and nonfinite/overflow cases. Compare a clear scalar reference
before optimization. Reuse the three authorized local originals for relevant
visual checks after numeric correctness. P5 display/color processing, new
dependencies and performance optimization are outside this handoff.

验证中性增益恒等、已知 CFA 手算向量、分通道常量场、通道脉冲、渐变、四种 Bayer 相位、
边界、奇数/小尺寸、非法增益及非有限/溢出情况。先验证清晰的标量参考，不提前优化。
数值正确后复用三张已授权原片做适用视觉检查。P5 显示/色彩处理、新依赖及性能优化不在
本次交接范围。
