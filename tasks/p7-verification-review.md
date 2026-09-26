# P7 Core Baseline Acceptance / P7 核心基线验收

Date / 日期：2026-09-25
Status: agreed engineering checks and actual learner recap recorded; P7 publication through merge authorized on 2026-09-26; CI-gated release in progress.
状态：约定范围工程检查及本人实际复盘已记录；2026-09-26 已授权 P7 发布至合并，按 CI 关口执行。Broader tone appearance remains deferred, not accepted / 更广影调观感延期，不计通过。

## Baseline and reproducibility / 基线与复现

Initial worktree was clean and detached at `158647c1417b7a1397e3e3e09e7e39d7e221ff06`.
Live `git ls-remote origin refs/heads/main` returned the same SHA; ancestry check
passed. This newly created worktree is separate from earlier P3–P6 workspaces.
No production/test/build/CI diff exists against final P6 head `875d9d0`.
初始独立工作区干净、游离于上述提交；实时 main 返回同一 SHA，祖先检查通过。
本工作区与旧 P3–P6 工作区独立，与最终 P6 head 的生产/测试/构建/CI 文件无差异。

Fresh directories `build/p7-debug` and `build/p7-release` were configured on the
existing macOS host. This proves clean build reproducibility, not a fresh OS or
hermetic dependency installation. Versions: CMake 4.3.4, Apple Clang 21.0.0,
LibRaw 0.22.1, libpng 1.6.58, SDK zlib 1.2.12, GoogleTest 1.17.0.
在既有 macOS 主机的新目录执行配置，证明干净构建可复现，不等于全新系统或完全封闭
依赖安装；版本如上。GoogleTest was freshly downloaded using the committed URL/SHA256;
no source override or old build cache was used / GoogleTest 使用仓库固定 URL/SHA256 新下载，未指定旧源码或复用构建缓存。

```sh
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake -S . -B build/p7-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake --build build/p7-debug
DEVELOPER_DIR=/Library/Developer/CommandLineTools ctest --test-dir build/p7-debug --output-on-failure --output-junit ../p7-evidence/ctest.xml
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake -S . -B build/p7-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
DEVELOPER_DIR=/Library/Developer/CommandLineTools cmake --build build/p7-release
build/p7-release/apps/mini-camera-raw --version
```

All commands passed: Debug 102/102, no failures/skips; Release reports 0.1.0.
Apple ld reports duplicate `-lc++` and, in the test target, core-library flags;
linking succeeds. No warning suppression or tolerance change was made.
全部通过：Debug 102/102，无失败/跳过；Release 版本为 0.1.0。链接器提示重复库参数，
链接成功；未屏蔽警告、未调整容差。Logs and JUnit: ignored `build/p7-evidence/` / 日志与 JUnit 位于该忽略目录。

Official source checked: installed CMake 4.3.4 `--help-module FetchContent`,
including URL/hash population and source-override semantics; see
[CMake FetchContent](https://cmake.org/cmake/help/v4.3/module/FetchContent.html).
已核对安装版本自带官方帮助中的下载/哈希与源码覆盖语义，来源链接如上。

## Exact-commit Linux evidence / 精确提交 Linux 证据

[CI run 36152769497](https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw/actions/runs/36152769497)
is a successful push run on the exact merge SHA above. P7 retrieved its live state
and full logs; it did not trigger another run. Ubuntu 24.04.5, GNU 13.3.0,
LibRaw 0.21.2, libpng 1.6.43, zlib 1.3; 102/102 tests passed.
该 push CI 精确对应上述合并 SHA。本轮读取实时状态与完整日志，未重新触发。
Linux 环境版本如上，102/102 通过；原始日志保存于忽略目录。
This tests synthetic fixtures on Linux, not private Sony RAW decoding on Linux.
这验证 Linux 合成测试，不等于 Linux 私人 Sony 原片验收。

## Stage numeric matrix / 逐阶段数值矩阵

Counts below come from this run's JUnit; all pass. Formula tests and deterministic
properties are production tests, not learner hand-calculation records.
下表数量来自本轮 JUnit，全部通过。公式与确定性性质测试不代表学习者完成过手算。

| Stage / 阶段 | Current tests / 本轮测试 | Contract and evidence / 契约与证据 |
|---|---:|---|
| Stage 0 + metadata / 底座与元数据 | 22 | ImageBuffer 18 + P5 metadata 2 + P6 metadata 1 + About 1; ownership, layout, state and overflow / 所有权、布局、状态及溢出 |
| P3 decode / 解码 | 15 | RawLayout 4 + RawDecoder 9 + RawFile 2; exact crop/copy, synthetic unpack, rejection boundaries / 精确裁剪复制、合成解包及拒绝边界 |
| P3 normalize / 归一化 | 7 | `(raw-black)/(white-black)`; signed/headroom, four CFA phases, padding; `1e-6 + 1e-6*abs(ref)` / 负值余量、四相位、padding 及该容差 |
| P4 WB / 白平衡 | 8 | green-mean-normalized gains; neutral identity, overflow, source preservation / 绿色均值归一增益、中性恒等、溢出及源保留 |
| P4 demosaic / 去马赛克 | 8 | same-color neighbor mean; measured channel exact, independent stencil, small/odd sizes / 同色邻居均值、实测通道精确保留、独立模板及小/奇尺寸 |
| P5 matrix and clip / 矩阵与裁剪 | 7 | `M*C`, independent `min(C,1)` output copy; basis, signed range, linearity / 矩阵方向、带符号范围、线性与独立输出副本 |
| P5 encode / 编码 | 5 | clamp output only; piecewise sRGB and round to RGB16; knots and monotonic ramp / 只裁剪输出、分段 sRGB 与 RGB16 舍入、接点及单调渐变 |
| P5 PNG | 5 | independent zlib decoder, CRC/chunks, exact RGB16 sample round trip and path failures / 独立解码、CRC/块、样本精确往返及路径失败 |
| P6 exposure / 曝光 | 4 | `2^EV`, EV in [-20,20]; identity, composition, overflow/underflow / 恒等、复合、溢出和下溢 |
| P6 tone / 影调 | 4 | `RGB/(1+s*max(Y,0))`, s in [0,1]; identity, luminance monotonicity, finite extremes / 恒等、亮度单调及有限极值 |
| P6 histogram / 直方图 | 6 | 256 bins plus outliers; exact pixel-count conservation and boundaries / 256 箱及越界计数、像素守恒与边界 |
| Render + CLI / 渲染与命令行 | 11 | Render 6 + CLI pipeline 3 + CLI 2; independent oracle, negative-EV neutral core, actual PNG and invalid arguments / 独立参考、负 EV 中性核心、实际 PNG 及非法参数 |

Historical full-image metrics remain useful because the production baseline is
unchanged. They were reviewed, not rerun or relabeled as P7 measurements:
生产基线未变，因此保留以下历史全图证据；本轮复核，不冒称重新测量：

- [P3 report](p3-verification-review.md): analytic normalization max error
  1.49011611383e-9, RMSE 7.45058056917e-10; decoding acceptance separately recorded.
  P3：解析归一化误差如上，真实解码验收另有记录。
- [P4 report](p4-verification-review.md): three-scene WB max ≤1.18976e-7,
  RMSE ≤4.81154e-9; demosaic max ≤1.19209e-7, RMSE ≤6.03631e-9;
  no failed components at `1e-6+1e-6*abs(ref)`.
  P4：三类白平衡/去马赛克最大误差及 RMSE 如上，该容差下无失败分量。
- [P5 report](p5-verification-review.md): matrix max ≤1.19209e-7,
  RMSE ≤1.05173e-8; working-buffer fingerprints preserved.
  P5：矩阵误差如上，工作数据指纹保持。
- [P6 corrected report](p6-highlight-regression-check.md): read-only inspection
  of the three local numeric logs confirms 36 passed cases, EV {-2,0,2},
  strength {0,1}, both branches; max error ≤4.76836904184e-7,
  RMSE ≤4.20691231097e-8 against `1e-5+1e-4*abs(ref)`.
  P6：只读复核三份本地日志，共 36 案例通过，范围及误差如上。

P6 final Debug/ASan/UBSan 102/102 and no-tests evidence is historical in
[p6-verification-review.md](p6-verification-review.md); sanitizers were not repeated
for this documentation-only acceptance. Leak detection was disabled there.
最终 P6 两套 102/102 与关闭测试证据为历史记录，本轮仅验收文档不重复 sanitizer；
历史检查关闭了泄漏检测，不能称为 LSan 通过。

## Visual category record / 分类视觉记录

P7 viewed the three existing corrected P6 contact sheets through the tool image
viewer, read-only. Source build: P6 Release; production outputs RGB16/sRGB;
contact sheets are reduced JPEG views, not full-resolution or calibrated displays.
本轮通过工具图像查看器只读查看三张 P6 修正后对照图。源构建为 P6 Release，生产输出
为 RGB16/sRGB；对照图为缩小 JPEG，不能替代全尺寸或标定显示验收。

| Category / 类别 | Parameters and observation / 参数与观察 | Acceptance boundary / 验收边界 |
|---|---|---|
| Daylight / 日光 | camera-clip, EV -2/0/+2, s 0/1; exposure progression, tone compresses sky and midtones / 曝光一致变化，影调压低天空和中间调 | Prior user no-obvious-issue conclusion retained; broad tone quality deferred / 保留用户无明显问题结论，更广影调质量延期 |
| Backlit / 逆光 | EV -2/0/+2, s0; old/approved/implemented columns; candidate and implementation visually agree; dimmed core is neutral with colored transitions / 三列旧版、确认候选、实现；后两列一致，压暗核心中性但仍有彩色过渡 | Approved candidate retained; historical byte identity recorded; hard clipping is not reconstruction / 保留确认及历史逐字节一致证据，硬裁剪非重建 |
| Dark / 暗景 | camera-clip, EV -2/0/+2, s0/1; dark shadows, brighter windows, consistent EV response / 阴影暗、窗户亮、曝光变化一致 | Prior coarse acceptance retained; no full-resolution noise/artifact claim / 保留粗验收，不宣称全尺寸噪声/伪影通过 |

The visual review needed no new RAW reads, copies or uploads. The separately
authorized external backup includes private originals and remains private. No new visual user approval is
invented. CFA artifacts, zippering, fine halos, banding and color accuracy are not
closed by these thumbnails. ADR-009 remains `T(2^EV*M*min(C,1))`; preserve is an
uncorrected diagnostic branch, not the normal original-image baseline.
视觉复核无需新增原片读取、复制或上传；另行授权的外部备份包含私人原片且保持私有。
不补造用户确认。缩略图不能关闭 CFA 伪影、拉链纹、
细光晕、断层及色准验收。ADR-009 公式如上，preserve 仅为未修正诊断分支。

## Completion boundary / 完成边界

Engineering checks above pass. The learner answered all three recap prompts on
2026-09-26. Range and clipping arithmetic were correct; the overly broad claim
that tests prove the entire pipeline defect-free was corrected. The actual answers,
feedback and remaining highlight questions are recorded locally. This records the
recap, not a claim of exhaustive mastery or acceptance of future algorithms.
P6 study remains user-confirmed. See [P8 entry audit](p8-entry-audit.md) for handoff.
上述工程检查通过。用户于 2026-09-26 回答三项复盘，范围与裁剪手算正确；对测试证明
全链无问题的过强结论已纠正。实际回答、反馈及高光疑问已本地记录。这是复盘完成
记录，不宣称穷尽掌握或已接受未来算法。P6 已学习结论保持，交接见 P8 入口检查。

Deferred/outside this run: broader tone appearance (user-requested), calibrated or
full-resolution visual acceptance, benchmark/optimization, Windows, generic RAW
support, complete untrusted-file security audit and LSan. None is silently marked
passed. This closeout changes documentation only; no algorithm or dependency changes.
Publication is authorized; PR/CI and the final backup receipt establish its final state.
延期/本轮范围外：用户指定的更广影调观感、标定/全尺寸视觉、benchmark/优化、Windows、
通用 RAW、完整不可信文件安全审计及 LSan，均未静默计通过。本次收尾只改文档，不改算法或依赖。
发布已授权，最终状态由 PR/CI 及最终备份凭据确定。
