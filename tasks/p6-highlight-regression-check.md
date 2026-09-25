# P6 Highlight Regression Check / P6 高光回归检查

Date / 日期：2026-09-25
Status / 状态：Candidate approved and implemented; see final verification below / 候选已确认并实现，见末尾验证记录。

The diagnosis below describes the pre-fix checkpoint. / 下述诊断描述修正前检查点。

## User feedback / 用户反馈

Daylight and dark-scene previews have no obvious problems. The user treated
preserve/tone-off as the original baseline and rejected recurring magenta in
backlit overexposure, including all four EV-2 previews. Tone appearance is deferred
until after the middle stage; this is not acceptance of its visual quality.
用户确认日光、暗景没有明显问题，将 preserve/影调关视为原图基准；指出逆光粉紫
过曝重现，且 EV-2 四张全部出现。影调观感留到中期后讨论，不计作影调视觉通过。

## Findings / 检查结论

1. apps/render_pipeline.cpp:47 returns the uncorrected working branch for preserve.
   P5's accepted correction was an explicit output branch, not part of the pure
   matrix or encoder. Thus preserve bypasses that correction completely. It is an
   unclipped diagnostic rendering, not the accepted P5 visual baseline.
2. apps/render_pipeline.cpp:54 applies EV before clip_camera_highlights's fixed
   nominal ceiling 1. Negative EV changes the scale; the previously saturated
   core falls entirely below the threshold and bypasses the correction.
3. tests/render_pipeline_test.cpp:27 checks P5 equivalence only at EV0/strength0.
   Its generated reference at line 44 reproduces the accepted but inadequate
   exposure-before-fixed-clip formula. Passing it proves arithmetic agreement,
   not preservation of P5's high-light behavior under exposure changes.
4. ADR-008's statement that clipping before exposure destroys editable headroom
   conflated the original editing buffer with an independent output derivative.
   Preserving original C and W allows an output-only policy without modifying them.

1. 输出协调器第 47 行的 preserve 返回未经高光修正的工作分支。P5 修正属于显式
   输出分支，不在纯矩阵/编码器中；故此模式完全绕过修正，不是 P5 已验收原图基准。
2. 第 54 行先做 EV，再按 clip_camera_highlights 的固定标称上界 1 裁剪。负 EV
   改变数值尺度，使原饱和核心全落到阈值以下，高光修正失效。
3. 测试只在 EV0/强度0 检查 P5 等价；生成参考复现了已接受但不足的公式。通过证明
   算术一致，不证明曝光变化后保持 P5 的高光表现。
4. ADR-008 关于“曝光前裁剪损失编辑余量”的表述混淆了原编辑数据与独立输出副本。
   保留原 C/W 后，可以只对输出副本应用策略，不改原始工作数据。

These are P6 integration/acceptance defects. They should not be dismissed as a
new limitation the user must accept. P5's clip implementation itself is unchanged
against a4130b0. No sRGB transfer or PNG defect was found in this check.
这些是 P6 衔接/验收缺陷，不应仅作为新限制要求用户接受。相对 a4130b0，P5 裁剪
函数本身未改变；本检查未发现 sRGB 传递或 PNG 问题。

## Reproduced real core / 真实核心复现

The authorized backlit RAW was reread in place. At the previously diagnosed core,
the normalized 2x2 tile remains (1,1,1,1); white-balanced camera RGB is
(2.37890625,1,1.5986328125). The sensor saturation does not disappear when software
exposure is reduced.
授权逆光 RAW 再次原位读取；此前诊断核心的归一化 2x2 仍为 (1,1,1,1)，白平衡后
相机 RGB 如上。软件降低曝光不会消除原先传感器已经饱和的事实。

| Path / 路径 | Linear RGB core / 线性 RGB 核心 |
|---|---|
| P5 corrected / P5 修正 | (1,0.999999940395,1) |
| Current preserve, EV-2, tone0 / 当前保留 | (0.7859416008,0.154356494546,0.447163105011) |
| Current clip, EV-2, tone0 / 当前裁剪 | Same as preserve / 与保留相同 |
| Either current mode, EV-2, tone1 / 当前任一模式、影调开 | (0.600049555302,0.11784786731,0.341399431229) |
| Candidate P5 baseline then EV-2, tone0 / 候选 P5 基准再曝光 | (0.25,0.249999985099,0.25) |

At EV-2 the camera values are (0.5947265625,0.25,0.399658203125), all below one.
Tone scales the three channels together, so it retains the unwanted imbalance.
This explains all four EV-2 panels without attributing the cause to tone mapping.
EV-2 的相机值如上，全部小于一，当前裁剪完全不起作用；影调同倍率缩放三通道，
保留失衡，因而解释了四张 EV-2 均粉紫，不把根因归为影调。

## Concrete correction candidate / 具体修正候选

Let g=2^EV. Current clipped output before encoding is T(M*min(g*C,1)). Candidate:
T(g*M*min(C,1)). Equivalently, in an exposed-camera formulation the ceiling must
track exposure as g: min(g*C,g), not remain 1. Keep original C and W=M*C intact;
apply EV to the linear output derivative, never to an encoded PNG or old preview.
Expose this P5-compatible rendering as the normal baseline; label uncorrected
preserve explicitly as diagnostic, not as an original-image reference. This changes
ADR-008's render policy and requires its replacement to be reviewed before landing.
当前裁剪输出公式如上，候选为先保持 P5 输出策略、再对线性输出副本做曝光；等价
表达是曝光后阈值随 g 变化，不能固定 1。原 C/W 保持完整，不编辑编码 PNG 或旧
预览。普通预览以 P5 兼容渲染为基准，未修正 preserve 明确标成诊断，不再作为原图
对照。该调整改变 ADR-008 输出策略，落地前需审阅替代决策。

A private harness built against the current release libraries produced tone-off
candidate previews for EV {-2,0,+2}. It passed full-frame exposure comparison,
core neutrality at all three EVs, EV0 byte-exact P5 encoded equivalence, and camera/
working-buffer fingerprints. Agent inspected the 3x3 comparison: candidate removes
magenta core/reflection at EV-2; EV0 matches P5. Hard clipping still discards detail,
and neutral gray/colored transitions remain visible when dimming the saturated core.
This is not reconstruction or a claim that all highlight appearance is solved.
No tone formula or production code was changed. Existing test results remain the
previous 99/99 checkpoint; no new production suite was needed for this diagnosis.
本地 harness 链接当前 Release 库，生成 EV -2/0/+2、影调关闭的候选对照，通过全图
曝光参考、三曝光核心中性、EV0 与 P5 编码逐字节一致及相机/工作图指纹检查。Agent
查看 3x3 对照：候选消除 EV-2 粉紫核心/反光，EV0 匹配 P5；硬裁剪仍损失细节，压暗
饱和核心时仍可见中性灰和颜色过渡，不是重建或全部高光观感已解决。未改影调公式
或生产代码；99/99 为此前检查点，本诊断无生产变更，无需重跑该套件。

Evidence is ignored build/p6-highlight-check/{check.cpp,numeric.txt,
exposure-only-comparison.jpg,*.png}. Raw was not copied/uploaded. Existing previews
are retained. Future work: review the replacement render policy; add a failing
negative-EV saturated-core regression before implementation; rerun full/sanitizer
and all three scenes; keep broader tone appearance deferred as requested.
证据在上述忽略目录，原片未复制/上传，旧预览保留。后续：审阅替代输出策略，实现
前补负 EV 饱和核心失败回归，再复跑全套/sanitizer/三场景；更广影调观感按要求延期。

## Approved correction verification / 已确认修正验证 — 2026-09-25

The user approved implementation of the right-column candidate. Production now
uses T(g*M*min(C,1)) for camera-clip. Normal preview documentation recommends
camera-clip; preserve remains uncorrected diagnostic output. Original editing
buffers and tone formula are unchanged. ADR-009 supersedes the former order.
用户批准按右列候选实现。生产 camera-clip 现使用上述公式；普通预览推荐此模式，
preserve 明确为未修正诊断。原编辑数据及影调公式不变；ADR-009 替代旧顺序。

- Negative-EV saturated-core regression failed before the fix, then passed.
- Debug and ASan/UBSan each pass 101/101; Release BUILD_TESTING=OFF builds and
  --version succeeds. Leak detection remains disabled; Linux CI was not run.
- Three authorized RAWs read in place: all 36 full-frame oracle cases pass,
  tolerance 1e-5+1e-4*abs(reference), including source fingerprints, histogram
  conservation and EV0/tone0 byte-exact P5 encoding. Tone-off output also equals
  the exposed P5 linear baseline at every component for EV -2/0/+2.
- Backlit tone-off PNG files at all three EVs are byte-identical to the approved
  candidate. EV-2 core is (0.25,0.249999985099,0.25). Agent inspected all three
  scene sheets; corrected backlit core/reflection matches the approved output.
- Tests-first self-review found no remaining required finding in this correction:
  source ownership, order, overflow, stride and labeled histogram semantics checked.
  Replaced obsolete camera-intermediate overflow test with working overflow guard
  plus valid zero-matrix case; no skipped tests or tolerance relaxation.

- 负 EV 饱和核心回归修正前失败，修正后通过。
- Debug 与 ASan/UBSan 各 101/101；Release 关闭测试构建及 --version 通过。
  泄漏检测仍关闭，未运行 Linux CI。
- 三张授权 RAW 原位读取：36 个全图参考案例全部通过，容差如上，包含源指纹、直方图
  守恒、EV0/影调0 与 P5 编码逐字节一致；三个 EV 下影调关闭输出每分量均等于 P5
  线性基准乘曝光增益。
- 三个 EV 的逆光影调关闭 PNG 与已确认候选文件逐字节相同；EV-2 核心数值如上。
  Agent 查看三场景对照，逆光核心及反光与已确认结果一致。
- 先测试后实现的自审未发现本修正仍需处理的问题；检查源所有权、顺序、溢出、步幅
  及统计标注。旧相机中间态溢出测试改为工作路径溢出保护及合法零矩阵案例，无跳过
  测试或放宽容差。

Evidence: ignored build/p6-highlight-check/{red,green,sanitize,release-build}.log
and build/p6-highlight-fixed/{real_verify.cpp,daylight,backlit,dark}. Previous
outputs retained. Broader tone appearance remains deferred until after the middle
stage, not accepted. No commit or push; no independent reviewer or benchmark claim.
证据在上述忽略目录，旧输出保留。更广影调观感继续留至中期后，不计通过。
未提交或推送，未声称独立审查或性能验证。

The real CLI full-resolution EV-2/tone0 PNG passed independent chunk CRC,
RGB16/sRGB checks and every sampled pixel comparison with the verified preview
(build/p6-highlight-fixed/png-check.txt). The initial Python verifier's repeated
byte concatenation was interrupted for excessive runtime; joining chunk lists
completed the same checks. No production change or weakened check was needed.
正式 CLI 全尺寸 EV-2/影调0 PNG 通过独立块 CRC、RGB16/sRGB 及全部预览采样像素
核对。初版 Python 验证脚本因反复拼接字节运行过慢而中断，改为块列表合并后完成
相同检查，无生产变更或检查降级。
