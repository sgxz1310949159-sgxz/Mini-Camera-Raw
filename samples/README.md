# Samples / 示例数据

This folder documents the sample-data policy for the project.

本文件夹用于记录项目的示例数据策略。

RAW source files are always local-only for the current Sony A7C II sample set.
Derived preview images may be published after review. Put local files in:

当前 Sony A7C II 样张的 RAW 源文件始终只保留在本地。经过检查的派生预览图可以
公开展示。本地文件请放在：

```text
samples/raw/
samples/output/
```

Those folders are ignored by git. Publication permission for a preview does
not grant permission to upload its `.ARW` source file or original metadata.

这些文件夹已被 git 忽略。预览图的展示许可不代表可以上传对应的 `.ARW` 源文件
或原始元数据。

Public sample files should only be added when they are small, useful for tests,
and safe to redistribute. For early testing, prefer synthetic buffers in unit
tests over committing real camera files.

只有当示例文件体积小、对测试有用，并且确认可以安全再分发时，才应加入公开仓库。早期测试应优先使用单元测试中的合成 buffer，而不是提交真实相机文件。

Recommended private test set:

推荐的私人测试素材集合：

- one daylight scene with broad color
- one high-contrast scene
- one dark/low-light scene
- one image with skin tones
- one image with fine detail for demosaicing artifacts

- 一张色彩丰富的日光场景
- 一张高反差场景
- 一张暗光/低光场景
- 一张包含肤色的图像
- 一张细节丰富、适合观察去马赛克伪影的图像

Current verified set / 当前已确认集合：

- one daylight image / 一张日光图
- one high-dynamic-range image / 一张大光比图
- one low-light image / 一张低照度图

All three may be used to produce public preview images. Before committing a
preview, remove unnecessary metadata, confirm that the frame contains no
newly discovered private information, and record the processing parameters.

三张样张均可用于生成公开预览图。提交预览前，必须移除不必要的元数据、确认画面中
没有新发现的隐私信息，并记录处理参数。
