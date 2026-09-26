# P8 Entry Audit / P8 入口检查

Date / 日期：2026-09-26
Verdict / 结论：Ready to prepare the P8 purpose/scope/learning briefing; not yet
approved to begin P8 learning or implementation / 可以准备 P8 目的、范围与学习汇报，
尚未获准开始 P8 学习或实施。

## Baseline / 基线

HEAD and freshly queried remote main both equal
`158647c1417b7a1397e3e3e09e7e39d7e221ff06`. P6 PR #3 and P5 PR #2 are in local
history. Production, tests and build/CI files have no local diff. Saved P7 JUnit
contains 102 tests with zero failures; the exact-commit Linux evidence and Release
no-tests result are unchanged. No unnecessary rebuild was run for this document audit.
HEAD 与本轮实时远端 main 均为上述提交；本地历史包含 P6 PR #3 及 P5 PR #2。
生产、测试、构建/CI 无本地差异；既存 P7 JUnit 为 102 项零失败，精确提交 Linux
证据及 Release 关闭测试结果未变。本轮文档检查未无理由重新构建。

## Prior stages / 前序阶段

| Stage / 阶段 | Current evidence / 当前证据 | Remaining boundary / 剩余边界 |
|---|---|---|
| Stage 0 / 底座 | stage0-closeout.md records completion / 收尾记录已完成 | No newly found required item / 未发现新增必做缺项 |
| P3 | Numeric/decoder checks, coarse visual and user-confirmed study / 数值解码、粗视觉及用户学习确认 | Limited Sony scope, no general RAW/security claim / 限定 Sony，不宣称通用 RAW/全面安全 |
| P4 | WB/demosaic verification and user coarse acceptance/study / 白平衡去马赛克验证、用户粗验收与学习 | Known bilinear limitations / 保留双线性局限 |
| P5 | Implementation, output checks and agreed sample acceptance; merged via PR #2 / 实现输出与约定样张验收，PR #2 已合并 | Hard clipping is not reconstruction / 硬裁剪非重建 |
| P6 | Final 102/102 evidence and PR #3 merge; agreed corrected backlit result / 最终验证及合并，逆光修正结果已确认 | Existing shoulder excluded from future formal path by provisional user decision; no code removal claimed / 当前肩部暂不进入未来正式路径，不宣称已删除代码 |
| P7 | Fresh-build/CI evidence, per-stage metrics, category records and actual learner recap / 干净构建、CI、逐阶段数值、分类视觉及本人复盘 | Acceptance complete within recorded scope; publication authorized and CI-gated / 已记录范围内验收完成，已授权发布并须通过 CI |

Sources / 来源：stage0-closeout.md, p3-verification-review.md,
p4-verification-review.md, p5-verification-review.md, p6-verification-review.md,
p6-highlight-regression-check.md, p7-verification-review.md and current Git evidence.
以上来源为对应阶段报告及本轮 Git 证据，历史未完成段落按其后最终记录解释。

P5/P6 summary checkboxes and the P7 response checkbox were stale and are reconciled.
P7 recap requires documented formulas/ranges/failures/questions, not elimination of
all future research questions. The user answered; the third answer received an
explicit correction. No later successful re-test of that explanation is invented.
P5/P6 总清单和 P7 回答勾选滞后，已同步。P7 复盘要求记录公式/范围/失效/疑问，不要求
消除所有未来研究问题。用户已回答，第三题有明确纠正；不补造纠正后的再次核验通过。

## What is still unfinished / 尚未完成事项

1. The user authorized P7 commit/push/PR/merge before the P8 briefing on 2026-09-26.
   Complete CI-gated publication and a verified final backup. [P7 PR #4](https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw/pull/4) is the final
   publication authority; an independent P8 implementation must start from that
   merged baseline and receive local-only learning material separately.
   用户于 2026-09-26 授权先提交/推送/PR/合并 P7，再汇报 P8。须完成 CI 关口及最终
   验证备份；最终发布状态以 PR 为准，独立 P8 实施应从该合并基线出发，另行交接本地学习资料。
2. Highlight/output policy remains a P10 review requirement in the newly accepted
   order. Record it in P8 contracts; do not infer recovered detail from the accepted
   preview. No unapproved reconstruction is required to close P7's limited scope.
   高光/输出策略按已采用顺序留 P10 复核，P8 契约须标明；已确认预览不等于恢复细节，
   不要求为关闭 P7 有限范围而擅自实现重建。
3. Full-resolution/calibrated visual evaluation, Windows, LSan, comprehensive
   untrusted-file audit and performance optimization are not claimed complete.
   Preview latency/memory targets must be defined in P8 and measured from P9.
   全尺寸/标定视觉、Windows、LSan、全面文件安全及性能优化未宣称完成。P8 须定义
   预览延迟/内存目标，自 P9 实测；不把这些缺口伪装为实时能力已通过。

## Backup and next gate / 备份与下一关口

Existing local receipts confirm the initial external archive (3874 entries) and
completion supplement (10 files) passed restore/hash checks; Git restoration and
source stability passed for the base snapshot. This audit does not rerun the entire
archive restore. The entry-audit supplement also passed restoration/hash checks. Publication changes
require a further final supplement including fresh Git history; its receipt stays local.
既存本地凭据显示主归档 3874 项及收尾补充包 10 文件恢复/哈希通过，主快照 Git 恢复
及源稳定性通过。本次不重做整包恢复；入口检查补充包也已恢复/哈希通过。
发布产生的变化须另补最终快照及最新 Git 历史，凭据位于忽略的备份控制目录。

User provisionally accepted P8 → P9 → P10 → P11 → P12 → P13. Next deliverable is
P8's detailed briefing explaining every item's purpose and concrete learning topics,
with decisions separated into pre-learning and post-learning confirmation points.
Do not start lessons, choose unapproved algorithms/UI/dependencies, create a new task,
or implement merely because this audit permits preparation.
用户暂定采用上述顺序。下一产出是 P8 详细汇报，逐项说明目的及具体学习内容，区分
学习前、学习后确认点。允许准备不等于开始教学、选定未批算法/界面/依赖、创建新任务
或实施。
