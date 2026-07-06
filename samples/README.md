# Samples / 示例数据

This folder documents the sample-data policy for the project.

本文件夹用于记录项目的示例数据策略。

RAW photos and generated outputs are local-only by default. Put them in:

RAW 照片和生成输出默认只保留在本地。请放在：

```text
samples/raw/
samples/output/
```

Those folders are ignored by git.

这些文件夹已被 git 忽略。

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

