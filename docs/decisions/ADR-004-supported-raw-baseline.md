# ADR-004: Supported RAW Baseline / RAW 支持基线

Status / 状态：Accepted

Date / 日期：2026-07-09

## Context / 背景

RAW formats and sensor metadata vary by camera. Claiming broad RAW support
before one path is understood would hide camera-specific assumptions.

RAW 格式和传感器元数据因相机而异。在理解第一条完整路径之前声称广泛支持 RAW，
会掩盖相机相关假设。

## Decision / 决定

- The first real-camera path targets Sony A7C II (`ILCE-7CM2`) `.ARW`.
- Three local files have been identified from their headers.
- The set contains one daylight, one high-dynamic-range, and one low-light
  scene.
- Original RAW files, metadata, filenames, and private outputs stay outside
  git unless publication is approved separately.
- Derived preview images from all three scenes are approved for public display
  after metadata removal and a final privacy check. This approval does not
  include the `.ARW` source files.
- Public tests use generated synthetic Bayer fixtures.
- Support for another camera requires its own metadata and regression review.

- 第一条真实相机路径以 Sony A7C II（`ILCE-7CM2`）`.ARW` 为目标。
- 已通过文件头识别 3 个本地文件。
- 样张集合包含一张日光图、一张大光比图和一张低照度图。
- 原始 RAW、元数据、文件名和私有输出均不进入 git，除非之后单独批准公开。
- 三张样张的派生预览图均可在移除元数据并完成最终隐私检查后公开展示；此许可
  不包含 `.ARW` 源文件。
- 公开测试使用生成的合成 Bayer fixture。
- 支持另一种相机前，必须单独审查其元数据和回归测试。

## Consequences / 影响

The initial compatibility statement is intentionally narrow. A public clone
can build and test without the local camera samples, but cannot run the real
ARW validation path until the user supplies compatible files.

初始兼容性声明有意保持狭窄。公开 clone 无需本地相机样张即可构建和测试，但用户
提供兼容文件前，无法运行真实 ARW 验证路径。
