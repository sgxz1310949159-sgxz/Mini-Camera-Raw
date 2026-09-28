# 开发环境基线

状态：Observed

日期：2026-07-09

## 当前机器

| 项目 | 当前值 |
|---|---|
| 操作系统 | macOS 26.5.1 (Build 25F80) |
| 架构 | Apple Silicon `arm64` |
| C++ 编译器 | Apple Clang 21.0.0 |
| 编译目标 | `arm64-apple-darwin25.5.0` |
| Git | 2.53.0 |
| Homebrew | 6.0.9 |
| CMake | 4.3.4 |
| Ninja | 1.13.2 |
| pkg-config (`pkgconf`) | 2.5.1 |
| LibRaw | 0.22.1 |

这些值只描述第一台开发机器，并不等于项目的跨平台支持契约。

## 构建准备状态

- 已安装 CMake 4.3.4，高于项目最低要求 3.24。

- 已安装 Ninja 1.13.2，并将其作为本地推荐生成器。

- Apple Command Line Tools 编译器可用。

- pkg-config 2.5.1 能够成功发现 LibRaw 0.22.1。

LibRaw 已为阶段一准备完成，但 P1 暂时不链接它。P1 只建立核心库、CLI、
GoogleTest 和 CI 骨架。

## 计划中的可移植性检查

本地 CMake 基线通过后，加入 Linux GitHub Actions 构建。第一版可移植性目标为：

- Linux x86_64 + GCC 或 Clang

- 核心库使用 C++17，不依赖编译器专属语言扩展

## P3 依赖核验

2026-09-10，P3 通过 CMake FindPkgConfig imported target 链接系统 LibRaw。本机已
验证 LibRaw 0.22.1 和 Apple Clang 21.0.0。API 基线为 0.21+；同时核对了 0.21.4
头文件中的 rawparams 和 CONVERTFLOAT_TO_INT，不代表所有旧版本支持 ILCE-7CM2。
Homebrew 包提供重复的 C++ 标准库链接参数，Apple ld 会提示重复库，未屏蔽参数。
现有 Linux 工作流已补充安装 pkg-config 与 libraw-dev；新远端 CI 需发布后运行，
本 worktree 未执行。

## 重新记录条件

当编译器、CMake 最低版本、依赖管理方式、主要操作系统或 CI 平台发生变化时，更新本文件。

## 回调兼容 — 2026-09-19

首次 P3/P4 Linux CI 暴露数据错误回调 ABI 差异：官方 0.21.2 的偏移为 int，
0.22.1 为 INT64。私有空操作回调现由 set_dataerror_handler 参数推导偏移类型，
不使用强制类型转换、不升级依赖、不移除错误检查。使用官方 0.21.2 头文件的
仅语法编译复现旧失败并验证修复，本机 0.22.1 全套测试仍须通过。

## P5 依赖 — 2026-09-21

P5 使用系统 libpng（本机运行时 1.6.58），zlib 为生产传递依赖和独立解码测试直接
依赖，CMake 报告 SDK zlib 1.2.12 接口。写入基线支持 macOS/Linux POSIX；CI 增加
发行版 libpng-dev，但未发布变更尚未运行远端 CI。旧版本编号包须核对安全补丁，
不宣称 Windows 写入支持。

libpng 使用 PNG Reference Library License v2，zlib 使用自身宽松许可证；分发库时
保留第三方通知。使用系统包，不复制第三方源码；日期化公告检查与 API 约束见 P5
准备文档。

相关资料与证据链接（保留原记录来源）：

- [LibRaw 0.21.2](https://github.com/LibRaw/LibRaw/blob/0.21.2/libraw/libraw_types.h)

- [0.22.1](https://github.com/LibRaw/LibRaw/blob/0.22.1/libraw/libraw_types.h)
