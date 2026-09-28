# 处理命令行

状态：P6 已本地实现。

可执行文件为 `<build-directory>/apps/mini-camera-raw`。
从仓库根目录按 CMake 文档构建，可执行文件位置如上。仅支持 Sony ILCE-7CM2 原生
单帧整数 Bayer 文件。输出为带 sRGB 标识的 RGB16 PNG；不旋转、不自动曝光、不做
创作白平衡或高光重建、不提供 GUI。

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build
build/apps/mini-camera-raw --help
build/apps/mini-camera-raw --version
build/apps/mini-camera-raw --input /path/to/input.ARW --output /path/to/new.png \
  --highlights camera-clip --ev 1 --tone-strength 0.5 --histogram
```

macOS 的构建命令可能需要前缀 `DEVELOPER_DIR=/Library/Developer/CommandLineTools`。
替换两个示例路径，输出必须不存在、父目录必须已存在。不覆盖现有文件、符号链接或
目录，不提供 `--force`。私人路径/照片保持本地；macOS 可能需要在 CMake 命令前
设置上述 DEVELOPER_DIR。

| 选项 | 含义 |
|---|---|
| `--input PATH` | 必填原始 RAW |
| `--output PATH` | 必填新 PNG 路径 |
| `--highlights preserve` | 未修正诊断，不是 P5 基准 |
| `--highlights camera-clip` | 显式输出副本：上界 1 硬裁剪→矩阵→曝光→影调 |
| `--ev NUMBER` | 有限 [-20,20]，默认 0，线性增益 2^EV |
| `--tone-strength NUMBER` | 有限 [0,1]，默认 0 关闭，越大压缩越强 |
| `--histogram` | 成功写出后打印四阶段统计 |

必须显式选择高光策略；普通 P5 兼容预览使用 camera-clip。EV0/强度0 复现 P5，负 EV
保持修正后的核心。preserve 仅作未修正诊断，可显示饱和粉紫。两者均不重建丢失细节；
更广影调观感延期讨论。

`--help`、`--version` 单独使用。未知/重复选项、缺值、非有限/越界/带尾字符数字在
解码前失败。数字使用点作小数分隔、不随 locale 改变，拒绝空白。文件名若以 `--`
开头，使用绝对路径或 `./` 前缀。成功退出 0，参数/解码/处理/写出错误退出非零并
写入 stderr。

统计阶段为曝光前、曝光后、影调后（均 preserve）和实际输出 rendered（所选分支）。
表头包括状态、空间、传递、EV 和强度；R/G/B/Y 每行输出极值、两类越界计数及 256
个逗号分隔箱。[0,1) 使用 floor(256*v)，1 进入第 255 箱，越界单列，每行计数总和为像素数。Y 为线性相对亮度，
并非编码亮度；通道越界数不是去重像素数，也不证明传感器饱和。统计针对编码前
浮点图，不是整数 PNG；不自动归一化或均衡。

契约：[P6](p6-design-preparation.md), [ADR-008](decisions/ADR-008-p6-tone-and-render-order.md).
