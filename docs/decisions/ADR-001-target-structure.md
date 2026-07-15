# ADR-001: Target Structure / Target 结构

Status / 状态：Accepted

Date / 日期：2026-07-09

## Context / 背景

Algorithm code must be testable without a command-line process, while the
project also needs a small end-to-end entry point for smoke tests and later
RAW conversion experiments.

算法代码需要脱离命令行进程独立测试，同时项目也需要一个小型端到端入口，用于
smoke test 和后续 RAW 转换实验。

## Decision / 决定

- Build `mini_camera_raw` as a static core library.
- Build `mini-camera-raw` as a CLI linked to the core library.
- Link tests directly to the core library.
- Keep parsing and presentation concerns out of reusable algorithm modules.

- 将 `mini_camera_raw` 构建为静态核心库。
- 将 `mini-camera-raw` 构建为链接核心库的 CLI。
- 测试直接链接核心库。
- 将命令解析和展示职责排除在可复用算法模块之外。

## Consequences / 影响

The initial project has more than one target, but algorithms gain a stable,
testable boundary. A shared library or GUI can be added later without changing
the core ownership model.

初始项目会包含多个 target，但算法获得了稳定且可测试的边界。后续可以添加动态库
或 GUI，而无需改变核心代码的职责划分。
