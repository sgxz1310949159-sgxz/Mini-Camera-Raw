# Repository Boundary / 仓库边界

This project is planned as an open-source repository, but not every local file
belongs on GitHub.

本项目计划做成开源仓库，但并不是所有本地文件都应该放到 GitHub 上。

## Publish / 应公开

These should be included in the public repository:

以下内容适合放入公开仓库：

- source code under `src/`, `include/`, and `apps/`
- build files, CMake modules, and test configuration
- public project documentation under `docs/`
- unit tests and synthetic test fixtures
- benchmark source code
- small generated examples only when they are safe and useful for explanation

- `src/`、`include/` 和 `apps/` 下的源代码
- 构建文件、CMake 模块和测试配置
- `docs/` 下可公开的项目文档
- 单元测试和合成测试数据
- benchmark 源代码
- 只有在安全、可解释且确认授权无问题时，才放入小型生成示例

## Keep Local / 仅本地保留

These should not be committed:

以下内容不应提交：

- `learning/`: personal study notes, rough derivations, reading notes, mistakes,
  and private reflections
- `项目策划书.docx`: original personal planning document
- private RAW photos and camera files
- large generated outputs, benchmark results, and temporary renders
- compiled binaries and dynamic libraries
- dependency caches and local IDE state

- `learning/`：个人学习笔记、粗略推导、阅读记录、错误记录和私人反思
- `项目策划书.docx`：原始个人策划文档
- 私人 RAW 照片和相机文件
- 大型生成输出、benchmark 结果和临时渲染结果
- 编译产物和动态库
- 依赖缓存和本地 IDE 状态

The `.gitignore` file already reflects this policy.

`.gitignore` 已经体现了这套边界规则。

## Conditional / 视情况公开

Some files may be published later, but only after checking rights and privacy:

部分内容未来可以公开，但必须先检查版权和隐私：

- sample RAW files, if they are self-shot and intentionally released or clearly
  licensed for redistribution
- before/after images, if they do not reveal private content
- learning summaries rewritten as polished technical docs
- benchmark reports, once they are reproducible and not just local scratch data

- 样张 RAW 文件：仅限本人拍摄并明确愿意发布，或明确允许再分发的素材
- 前后对比图：不能暴露私人内容
- 从学习笔记中重写出来的正式技术总结
- benchmark 报告：需要可复现，而不是本地临时记录

## GitHub Setup Status / GitHub 设置状态

Local git has been initialized in this folder.

本文件夹已经初始化为本地 git 仓库。

Remote GitHub publication still needs one of these:

远端 GitHub 发布还需要满足以下条件之一：

- a GitHub repository URL created by the user, or
- a working GitHub CLI flow that can create repositories.

- 用户先创建一个 GitHub 仓库并提供仓库 URL，或
- 安装并登录可用的 GitHub CLI，让本机能执行建库流程。

Current local status on 2026-07-06:

2026-07-06 当前本地状态：

- `gh` CLI is installed: version 2.96.0.
- `gh` authentication is verified for `sgxz1310949159-sgxz`.
- The available GitHub connector can work with existing repositories, issues,
  pull requests, branches, commits, and files, but no create-repository tool is
  currently exposed.

- 当前环境已安装 `gh` CLI：版本 2.96.0。
- `gh` 已验证登录到 `sgxz1310949159-sgxz`。
- 可用的 GitHub 插件可以操作已有仓库、issue、pull request、分支、commit 和文件，但目前没有暴露创建新仓库的工具。

Recommended remote repository:

推荐远端仓库名：

```text
Mini-Camera-Raw
```

Recommended visibility:

推荐可见性：

```text
Public
```

Recommended first license:

推荐初始许可证：

```text
MIT
```

Do not add a `LICENSE` file until the author/copyright string is confirmed.

在确认作者署名/版权字符串之前，暂不添加 `LICENSE` 文件。

## Recommended `gh` Setup / 推荐的 `gh` 设置方式

Install GitHub CLI with Homebrew:

使用 Homebrew 安装 GitHub CLI：

```sh
brew install gh
```

Log in interactively:

交互式登录：

```sh
gh auth login
```

Recommended login choices:

推荐登录选项：

```text
What account do you want to log into? GitHub.com
What is your preferred protocol for Git operations? HTTPS
Authenticate Git with your GitHub credentials? Yes
How would you like to authenticate GitHub CLI? Login with a web browser
```

After login, verify:

登录后验证：

```sh
gh auth status
```

After `gh` is installed and authenticated, this project can be published with a
flow like:

安装并登录 `gh` 之后，本项目可以用类似下面的流程发布：

```sh
gh repo create Mini-Camera-Raw --public --source . --remote origin --push
```
