# Stage gates and backup / 阶段关口与备份

Status / 状态：User-required workflow, effective 2026-09-26 / 用户要求的流程，自该日起生效。

## Stage briefing template / 阶段汇报模板

Before learning, supply the following and wait for explicit confirmation:
开始学习前提供以下内容，等待明确确认：

1. Stage objective, prior verified baseline, scope and exclusions / 阶段目标、前序已验基线、范围及排除项。
2. For every item: who needs it, in which scene, what problem it solves, desired
   image/interaction result, and why simpler alternatives are insufficient /
   逐项解释谁在什么场景需要、解决什么问题、期望图像/交互效果及更简单方案为何不足。
3. Proposed data flow, methods/options, costs and known limits; do not present
   unchosen options as agreed implementation / 拟议数据流、方法/备选、代价和已知限制，不把未选方案写成已定实现。
4. Specific learning topics and prerequisites; examples/checks and the decisions
   the learner will be able to make afterward / 具体学习内容与前置知识、例子/核验、学习后能够作出的决定。
5. Acceptance cases: numeric, visual, interaction/performance where applicable;
   distinguish automated evidence from user observation / 数值、视觉及适用的交互/性能案例，区分自动检查与用户观察。
6. Decision table: confirmed now, pending learning, and pending external evidence;
   each pending item has an explicit later confirmation point / 决策表区分当前已定、待学习、待外部证据，未决项均有后续明确确认点。

Flow / 流程：briefing → user confirms scope/learning → learning and verification →
post-learning choices confirmed → implementation → acceptance → verified backup →
stage closeout / 详细汇报→用户确认范围与学习→学习核验→学习后决策确认→实施→验收→
备份验证→阶段收尾。Do not begin the next stage automatically / 不自动启动下一阶段。

The user provisionally accepted the P8–P13 order on 2026-09-26; each stage still independently follows these gates. Current
P6 shoulder remains provisionally outside the formal photo path; no code deletion
or rerouting follows merely from documenting this decision.
用户于 2026-09-26 暂定采用 P8–P13 顺序，各阶段仍独立遵守这些关口。P6 当前肩部暂不进入正式照片路径，
记录决定本身不代表已删除或改接代码。

## Backup procedure / 备份流程

Adapt the verified FirstLight procedure: external timestamped archive, SHA-256
manifest, actual temporary restore and per-file comparison. Its private storage
paths belong only in local receipts, not public documents.
参照 FirstLight 已验证流程：外部时间戳归档、SHA-256 清单、实际临时恢复及逐文件核对。
私人存储路径只进本地凭据，不进公开文档。

- Inventory main checkout, shared Git history and independent worktrees; include
  current edits and ignored learning/evidence. Preserve source trees read-only.
  清点主目录、共享 Git 历史与独立工作区，覆盖当前修改及忽略学习/证据；源目录只读。
- Confirm medium and scope, especially privately restricted originals. A RAW
  exclusion means the backup cannot restore those originals; say so explicitly.
  确认介质和范围，尤其有复制限制的私人原片；排除 RAW 就不能恢复原片，须明确声明。
- Use a unique directory; never overwrite existing archives. Preserve symlinks
  as links rather than silently following them into unrelated data.
  新建唯一目录，不覆盖既有归档；符号链接按链接保存，不静默跟随到无关数据。
- Hash files, archive, restore, compare every included file and symlink, and
  recheck source hashes for changes during the run. Verify Git history can be
  restored independently rather than relying on old absolute .git pointers.
  文件哈希、归档、恢复、逐文件/链接比较，并复核备份期间源哈希未变；验证 Git 历史
  可独立恢复，不依赖原机器绝对 .git 指针。
- Record file counts, bytes, exclusions, source revisions, archive SHA-256,
  restore/Git result and time in local and external receipts. Keep previous backups.
  本地和外部凭据记录数量、容量、排除项、源版本、归档哈希、恢复/Git 结果及时间，保留旧备份。

Trigger: initial backup now; every independent stage after final records are saved,
before closure. If stage material changes afterward, make another verified snapshot.
Receipt written after archival describes that snapshot; it need not recursively
include itself. Backup failure is an open closeout item, not permission to skip it.
触发：当前首次备份；以后每个独立阶段最终记录保存后、关闭前备份。之后阶段材料若
变化，需再做验证快照。归档后凭据描述该快照，无需递归包含自身。备份失败保持收尾
缺项，不构成跳过许可。No recurring timer or automatic publication is implied / 不推定定时任务或自动发布。
