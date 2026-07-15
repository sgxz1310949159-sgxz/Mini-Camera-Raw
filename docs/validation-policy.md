# Validation Policy / 验证策略

Status / 状态：Accepted Stage 0 baseline / 已确认的阶段零基线

Date / 日期：2026-07-09

## Evidence Layers / 证据层级

No single validation method is sufficient. Each implemented ISP stage should
collect the strongest applicable evidence from these layers:

任何单一验证方法都不充分。每个已实现 ISP 阶段都应收集以下层级中适用的最强证据：

1. analytic unit tests with hand-computable synthetic input
2. property and invariant tests over generated input
3. comparison with a simple scalar reference or documented trusted result
4. numeric image comparison
5. recorded visual inspection of representative outputs

1. 使用可手算合成输入的解析单元测试
2. 使用生成输入的性质与不变量测试
3. 与简单标量参考实现或有依据的可信结果比较
4. 数值图像比较
5. 对代表性输出进行有记录的视觉检查

Visual quality can reveal defects, but it never proves numeric correctness.

视觉质量可以暴露缺陷，但不能证明数值正确。

## Fixtures / 测试数据

- Public tests use deterministic generated fixtures such as `2x2`, `4x4`,
  odd-width, constant, impulse, ramp, saturated, and CFA-colored patterns.
- Fixtures must state CFA phase, dimensions, source values, and expected state.
- Private `.ARW` files are integration inputs, not unit-test dependencies.
- Golden files must be small, intentionally generated, reviewable, and
  redistributable. A golden file cannot replace analytic tests.

- 公开测试使用确定性生成 fixture，例如 `2x2`、`4x4`、奇数宽度、常量、脉冲、
  渐变、饱和与 CFA 分色图案。
- Fixture 必须注明 CFA 相位、尺寸、源值和预期状态。
- 私有 `.ARW` 是集成验证输入，不是单元测试依赖。
- Golden 文件必须小、明确生成、可审查且可再分发；不能代替解析测试。

## Comparison Rules / 比较规则

- Integer-preserving operations use exact equality.
- Simple scalar floating-point formulas start with absolute and relative
  tolerance `1e-6`.
- Multi-operation or image-stage comparisons start with `1e-5` absolute and
  `1e-4` relative tolerance.
- A value passes when
  `abs(actual - expected) <= abs_tolerance + rel_tolerance * abs(expected)`.
- Every looser tolerance requires a written reason based on accumulated error,
  reference differences, or platform behavior.
- Image comparisons report at least maximum absolute error and RMSE. PSNR may
  supplement them but does not replace per-pixel bounds.
- Later color work should use a documented color-difference metric, such as
  Delta E in a stated color space and illuminant.

- 保持整数的操作使用精确相等。
- 简单标量浮点公式初始使用 `1e-6` 的绝对与相对容差。
- 多步运算或图像阶段比较初始使用 `1e-5` 绝对容差和 `1e-4` 相对容差。
- 数值通过条件为
  `abs(actual - expected) <= abs_tolerance + rel_tolerance * abs(expected)`。
- 更宽容差必须说明累积误差、参考差异或平台行为方面的理由。
- 图像比较至少报告最大绝对误差和 RMSE；PSNR 可以补充，但不能代替逐像素边界。
- 后续颜色工作应使用有文档定义的色差指标，例如在明确色彩空间和光源下的 Delta E。

## Error and Edge Coverage / 错误与边界覆盖

Tests must cover, where applicable:

适用时，测试必须覆盖：

- zero dimensions, one-pixel dimensions, odd dimensions, and stride mismatch
- unknown or incompatible CFA pattern and color state
- invalid black/white levels and non-positive white-balance gains
- NaN, infinity, overflow risk, and values outside the nominal range
- deterministic behavior across repeated runs

- 零尺寸、单像素尺寸、奇数尺寸和行跨度不匹配
- 未知或不兼容的 CFA 排列与色彩状态
- 非法黑白电平和非正白平衡增益
- NaN、无穷、溢出风险和名义范围外数值
- 重复运行时的确定性

## Visual Review Record / 视觉检查记录

For each real-sample checkpoint, record the build type, input category without
publishing private filenames, parameters, output format/profile, viewer, and
observed defects. Check CFA phase artifacts, color cast, clipped highlights,
crushed shadows, halos, zippering, false color, NaN pixels, and orientation.

每个真实样张检查点都要记录构建类型、输入类别（不公开私有文件名）、参数、输出格式与
色彩配置、查看器和观察到的缺陷。检查 CFA 相位伪影、偏色、高光裁剪、暗部压死、
光晕、拉链纹、伪色、NaN 像素和方向。

## Benchmark Policy / 性能测试政策

Benchmarks begin only after correctness tests pass. Use Release builds, record
hardware and compiler, separate file I/O from algorithm timing, warm up before
measurement, run repeated trials, and report median plus dispersion. A speedup
is accepted only when output remains within the same validation bounds.

只有正确性测试通过后才开始 benchmark。使用 Release 构建，记录硬件和编译器，将文件
I/O 与算法计时分离，测量前预热，多次运行并报告中位数及离散程度。只有输出仍满足同一
验证边界时，性能提升才有效。
