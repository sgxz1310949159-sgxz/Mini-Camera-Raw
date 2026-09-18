# Development Environment Baseline / 开发环境基线

Status / 状态：Observed

Date / 日期：2026-07-09

## Current Machine / 当前机器

| Item / 项目 | Observed value / 当前值 |
|---|---|
| Operating system / 操作系统 | macOS 26.5.1 (Build 25F80) |
| Architecture / 架构 | Apple Silicon `arm64` |
| C++ compiler / C++ 编译器 | Apple Clang 21.0.0 |
| Compiler target / 编译目标 | `arm64-apple-darwin25.5.0` |
| Git | 2.53.0 |
| Homebrew | 6.0.9 |
| CMake | 4.3.4 |
| Ninja | 1.13.2 |
| pkg-config (`pkgconf`) | 2.5.1 |
| LibRaw | 0.22.1 |

These values describe the first development machine, not the project's
portable support contract.

这些值只描述第一台开发机器，并不等于项目的跨平台支持契约。

## Build Readiness / 构建准备状态

- CMake 4.3.4 is installed and exceeds the project minimum of 3.24.
- Ninja 1.13.2 is installed and is the recommended local generator.
- The Apple Command Line Tools compiler is available.
- pkg-config 2.5.1 discovers LibRaw 0.22.1 successfully.

- 已安装 CMake 4.3.4，高于项目最低要求 3.24。
- 已安装 Ninja 1.13.2，并将其作为本地推荐生成器。
- Apple Command Line Tools 编译器可用。
- pkg-config 2.5.1 能够成功发现 LibRaw 0.22.1。

LibRaw is prepared for Stage 1, but P1 should not link it yet. P1 establishes
only the core library, CLI, GoogleTest, and CI skeleton.

LibRaw 已为阶段一准备完成，但 P1 暂时不链接它。P1 只建立核心库、CLI、
GoogleTest 和 CI 骨架。

## Planned Portability Check / 计划中的可移植性检查

After the local CMake baseline passes, add a Linux GitHub Actions build. The
initial portability target should be:

本地 CMake 基线通过后，加入 Linux GitHub Actions 构建。第一版可移植性目标为：

- macOS arm64 with Apple Clang
- Linux x86_64 with GCC or Clang
- C++17 with no compiler-specific language extensions in the core library

- macOS arm64 + Apple Clang
- Linux x86_64 + GCC 或 Clang
- 核心库使用 C++17，不依赖编译器专属语言扩展

## P3 Dependency Check / P3 依赖核验

On 2026-09-10, P3 links the installed LibRaw package through CMake FindPkgConfig
and its imported target. Local LibRaw 0.22.1 and Apple Clang 21.0.0 were verified.
The API baseline is 0.21+; 0.21.4 headers were also checked for rawparams and
CONVERTFLOAT_TO_INT. This does not claim every older release supports ILCE-7CM2.
The local Homebrew package supplies a redundant C++ standard-library link flag,
which Apple ld reports as a duplicate-library warning; no flags are suppressed.
The existing Linux workflow now installs pkg-config and libraw-dev. A new remote
CI run requires publication and has not been executed in this worktree.

2026-09-10，P3 通过 CMake FindPkgConfig imported target 链接系统 LibRaw。本机已
验证 LibRaw 0.22.1 和 Apple Clang 21.0.0。API 基线为 0.21+；同时核对了 0.21.4
头文件中的 rawparams 和 CONVERTFLOAT_TO_INT，不代表所有旧版本支持 ILCE-7CM2。
Homebrew 包提供重复的 C++ 标准库链接参数，Apple ld 会提示重复库，未屏蔽参数。
现有 Linux 工作流已补充安装 pkg-config 与 libraw-dev；新远端 CI 需发布后运行，
本 worktree 未执行。

## Recheck Triggers / 重新记录条件

Update this document when the compiler, CMake minimum version, dependency
manager, primary operating system, or CI platform changes.

当编译器、CMake 最低版本、依赖管理方式、主要操作系统或 CI 平台发生变化时，更新本文件。
