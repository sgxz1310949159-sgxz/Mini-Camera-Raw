# ADR-009: P6 highlight baseline / P6 高光基准

Date / 日期：2026-09-25
Status / 状态：Accepted by user / 用户确认

## Context / 背景

Negative EV disabled ADR-008's fixed post-exposure clipping threshold, restoring
magenta in the saturated core. Its headroom argument confused original editing
data with an output derivative. The user approved the verified right-column candidate.
负 EV 使 ADR-008 的曝光后固定裁剪阈值失效，饱和核心重新粉紫。此前余量论证混淆
原编辑数据与输出副本。用户确认已验证的右列候选。

## Decision / 决策

Supersede only ADR-008's camera-clip order and baseline interpretation:
T(g*M*min(C,1)). Clip an independent camera copy, transform, expose, then tone.
Use camera-clip for normal previews; preserve remains explicitly uncorrected
diagnostic output. Keep CLI spellings and mandatory selection. Retain original
camera/working data and existing tone formula, validation and histogram semantics.
仅替代 ADR-008 的 camera-clip 顺序及基准解释，公式如上：独立相机副本裁剪、矩阵、
曝光、影调。普通预览用 camera-clip；preserve 明确为未修正诊断。CLI 参数名及必选
要求保留，原相机/工作数据、影调公式、校验与直方图语义保留。

## Alternatives and consequences / 备选与后果

Exposure-relative ceiling min(g*C,g) is mathematically equivalent but requires
an unnecessary camera-exposure helper. Fixed ceiling after exposure is rejected
by the reproduced regression. Output clipping loses detail; this is not highlight
reconstruction. EV0/tone0 must match P5 byte-for-byte, negative EV must keep the
neutral saturated core, and all edits must leave original samples unchanged.
The obsolete camera-exposure intermediate overflow test is replaced: unclipped
working-path overflow still fails, while a finite input mapped to zero is valid.
曝光后相对阈值 min(g*C,g) 数学等价，但需要额外相机曝光函数；曝光后固定阈值因
已复现回归而弃用。输出硬裁剪损失细节，不是高光重建。EV0/影调0 必须逐字节匹配
P5；负 EV 保持饱和核心中性，所有编辑不改原数据。旧相机曝光中间态溢出测试替换：
未裁剪工作路径溢出仍报错，有限输入经零矩阵映射为零则合法。

## Verification / 验证

Failing negative-EV reproduction, deterministic exposure/tone cases, CLI PNG
checks, Debug/sanitizers, and three authorized local samples. Broader tone
appearance remains deferred by the user; publication is not authorized.
先负 EV 失败复现，再确定性曝光/影调、CLI PNG、Debug/sanitizer 及三张授权本地样张
验证。更广影调观感按用户要求延期，未授权发布。
