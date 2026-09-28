# 仓库边界

本项目计划做成开源仓库，但并不是所有本地文件都应该放到 GitHub 上。

## 应公开

以下内容适合放入公开仓库：

- `src/`、`include/` 和 `apps/` 下的源代码

- 构建文件、CMake 模块和测试配置

- `docs/` 下可公开的项目文档

- 单元测试和合成测试数据

- benchmark 源代码

- 只有在安全、可解释且确认授权无问题时，才放入小型生成示例

## 仅本地保留

以下内容不应提交：

- `learning/`：个人学习笔记、粗略推导、阅读记录、错误记录和私人反思

- `项目策划书.docx`：原始个人策划文档

- 私人 RAW 照片和相机文件

- 大型生成输出、benchmark 结果和临时渲染结果

- 编译产物和动态库

- 依赖缓存和本地 IDE 状态

`.gitignore` 已经体现了这套边界规则。

## 视情况公开

部分内容未来可以公开，但必须先检查版权和隐私：

- 样张 RAW 文件：仅限本人拍摄并明确愿意发布，或明确允许再分发的素材

- 前后对比图：不能暴露私人内容

- 从学习笔记中重写出来的正式技术总结

- benchmark 报告：需要可复现，而不是本地临时记录

## GitHub 设置状态

本文件夹已经初始化为本地 git 仓库。

远端 GitHub 发布已经配置完成：

- 仓库：`sgxz1310949159-sgxz/Mini-Camera-Raw`

- 可见性：`Public`

- 默认分支：`main`

2026-07-06 当前本地状态：

- 当前环境已安装 `gh` CLI：版本 2.96.0。

- `gh` 已验证登录到 `sgxz1310949159-sgxz`。

- 本地 `origin` 已指向上述 GitHub 仓库。

- 可用的 GitHub 插件可以操作已有仓库、issue、pull request、分支、commit 和文件，但目前没有暴露创建新仓库的工具。

推荐远端仓库名：

```text
Mini-Camera-Raw
```

推荐可见性：

```text
Public
```

推荐初始许可证：

```text
MIT
```

在确认作者署名/版权字符串之前，暂不添加 `LICENSE` 文件。

## `gh` 设置记录

GitHub CLI 已通过 Homebrew 安装：

```sh
brew install gh
```

登录已通过交互式方式完成：

```sh
gh auth login
```

推荐登录选项：

```text
What account do you want to log into? GitHub.com
What is your preferred protocol for Git operations? HTTPS
Authenticate Git with your GitHub credentials? Yes
How would you like to authenticate GitHub CLI? Login with a web browser
```

验证命令：

```sh
gh auth status
```

仓库已使用下面的命令创建并推送：

```sh
gh repo create Mini-Camera-Raw --public --source . --remote origin --push
```

相关资料与证据链接（保留原记录来源）：

- <https://github.com/sgxz1310949159-sgxz/Mini-Camera-Raw>
