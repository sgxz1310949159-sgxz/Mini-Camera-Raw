# 阶段0 阶段零收尾与 P3 交接

日期：2026-09-10

## 验收

阶段零按 `docs/stage0-engineering-spec.md` 验收通过。基线提交为
`eba0f64d0aeafd7c3aa6edcf2d3ad7d9904975f1`，P0–P2 已完成。本次仅文档收尾开始前
工作树干净，没有修改实现。

- 新 Ninja 构建目录配置、构建成功，CTest 20/20 通过。

- 独立的新 Ninja 目录中 `BUILD_TESTING=OFF` 配置、构建成功。

- 版本 smoke 测试通过；文末链接为该基线已成功的 Linux CI。

- 本地学习笔记、构建产物及 RAW 路径保持忽略；没有读取 RAW。

- 现有已确认决策和目录结构允许直接开始 P3，无须重构工程目录。

本地执行命令（测试构建复用了现有 GoogleTest 源码缓存，未重新验证依赖下载）：

```sh
cmake -S . -B build/stage0-closeout-20260910 -G Ninja -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$PWD/build/p2-verify/_deps/googletest-src"
cmake --build build/stage0-closeout-20260910
ctest --test-dir build/stage0-closeout-20260910 --output-on-failure
cmake -S . -B build/stage0-closeout-no-tests-20260910 -G Ninja -DBUILD_TESTING=OFF
cmake --build build/stage0-closeout-no-tests-20260910
```

用户已确认 P2 学习完成；已核对现有本地 P1/P2 中文复盘和学习索引。本次接受工程底座
学习范围，不代表后续 ISP 算法已经掌握。P3 前置学习仍与独立学习任务协调，私人笔记仅本地保留。

## P3 边界

遵循 `tasks/plan.md` 的 P3：LibRaw 马赛克读取与必要传感器元数据，然后按 CFA 位置
显式进行黑白电平归一化，输出 float 线性 Bayer。保留负值及超一值，不隐式裁剪或
启用 LibRaw 后处理。不实现 P4 白平衡/去马赛克、P5 色彩/输出或优化。

先读 AGENTS.md、当前规格、ADR、流水线契约、验证与依赖策略。核实已安装 LibRaw 和
官方 API 资料，先写中文契约，再按 TDD 实现。测试 CFA 相位/裁剪、pitch、所有权、溢出、
非法元数据和分母、数值参考向量及布局不变量；保持现有测试通过并验证关闭测试的构建。

私人 ARW 不得复制进 worktree、提交或上传。真实文件测试前确认所选本地样张与读取授权，
可先推进合成测试和实现。预览隐私审查是发布时条件项，不是阶段零遗漏。

每周仍为 28 小时；9 月 9 日恢复，10 月 1 日核心验收，10 月 8 日至 11 月 6 日扩展。
不再顺延排期。没有新的明确要求时，不提交或推送本次收尾/P3 改动。

相关资料与证据链接（保留原记录来源）：

- <https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw/actions/runs/34175112747>
