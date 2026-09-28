# P3 验证与审查

日期：2026-09-10

## 结果

2026-09-18 提交前复核：使用现有 Command Line Tools 环境重建 `build/p3-clean`，
全套 CTest 42/42 `-Wall -Wextra -Wpedantic -Werror` 通过。先复核归一化/解码测试，再复核实现和构建修改，未发现新的
阻塞项。提交排除私人样张路径及生成文件；此前 sanitizer 和真实图片证据保留原日期。

2026-09-18 收尾：用户查看诊断材料后反馈“粗略检查认为没问题，继续推进”。按实际
检查深度记录为人工视觉验收，加上此前用户确认的学习完成，P3 本地验收完成。
此结论不代表穷尽逐像素检查、主分支集成、发布或新一轮远端 CI；下方带日期的待办
状态是历史快照。下一步：[P4 启动与学习交接](p4-handoff.md)。

2026-09-18 更新：从项目 decode/normalize API 生成并检查了九张本地诊断 PNG：
含四角的前后全图、中央/亮区/暗区马赛克与分离 CFA 局部、全范围及近黑四位置直方图。
在已检查尺度下未见明显行断裂、新增边框或显著双绿位置失配；低照度暗部噪声明显。
直方图可见梳状结构，本次未确定其成因。这不是最终色彩验收，也不能排除细微传感器
伪影。仍待用户确认视觉验收；用户已确认独立 P3 学习完成。

私人图像、元数据、导出程序和绘图脚本均保留在被忽略的 `build/p3-visual/`。
显示映射为 sqrt(clamp(gain*x,0,1))；全图采用 2×2 tile 均值，三张显示增益依次为
1/1/16，暗部局部为 64。不执行白平衡、去马赛克、旋转或色彩矩阵；生成后已删除
临时全分辨率数据。使用内置 Python NumPy/Pillow 和现有 Command Line Tools 编译器。
生产代码未改动，本次未将此前全套测试记录冒充为新测试结果。

2026-09-14 更新：用户授权使用主项目被忽略 RAW 目录中已有的日光、大光比、低照度
三张原件。新文件夹中三张副本经大小和 SHA-256 核对一致后删除，所有原件保留。
三张真实文件均通过解码和归一化，并逐像素对照 long double 参考，输出无非有限值。
私人详细数值保留在被忽略的 `build/p3-real/results.txt`。马赛克/视觉检查及独立
学习验收仍待完成。以下保留原日期的实施证据。

工程实现和合成验证已完成。P3 总体验收仍未完成：等待所选私人 Sony 样张读取授权、
真实文件集成/马赛克检查和独立学习验收。本次未 commit、push、merge，未读取/复制
私人 RAW，未编辑学习笔记。

## 验证

| 检查 | 证据 |
|---|---|
| 全新 Ninja Debug 构建 | `build/p3-clean`, Apple Clang 21.0.0, LibRaw 0.22.1, CMake 4.3.4 |
| 编译器诊断 | `-Wall -Wextra -Wpedantic -Werror` 通过 |
| 全套 CTest | 42/42 通过 |
| 关闭测试 | 新 `build/p3-no-tests` 全新配置构建通过 |
| 地址与未定义行为检查 | 新 `build/p3-sanitized` 全新构建、42/42 通过 |
| 可手算 2×2 归一化 | max absolute error `1.49011611383e-09`, RMSE `7.45058056917e-10` |
| 空白与边界 | `git diff --check`；忽略规则通过 |

链接警告为 `ignoring duplicate libraries: '-lc++'`。
测试构建复用已有 GoogleTest 1.17.0 源码缓存，未重新验证依赖下载。下方
`P3_GTEST_SOURCE` 表示该缓存绝对目录，通过 CMake 指定。
Homebrew LibRaw 的
pkg-config 文件添加重复标准库链接参数，Apple ld 报上述提示，链接成功。

```sh
cmake -S . -B build/p3-clean -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-Wall -Wextra -Wpedantic -Werror' \
  -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$P3_GTEST_SOURCE"
cmake --build build/p3-clean
ctest --test-dir build/p3-clean --output-on-failure
cmake -S . -B build/p3-no-tests -G Ninja -DBUILD_TESTING=OFF
cmake --build build/p3-no-tests
cmake -S . -B build/p3-sanitized -G Ninja -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined' \
  -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="$P3_GTEST_SOURCE"
cmake --build build/p3-sanitized
ctest --test-dir build/p3-sanitized --output-on-failure
git diff --check
```

参考向量：RAW `[0,200;700,1400]`，黑电平 `[100,200;300,400]`，白电平 `[1100,1200;1100,1200]`，预期 `[-0.1,0;0.5,1.25]`。
数值向量为上述 RAW、黑白参考及预期矩阵。仅本地、被忽略的验证程序调用已构建库，
计算表中误差。布局复制使用精确整数断言。本次无 benchmark 或性能结论。

## TDD 与已解决发现

合成测试调用 `open_bayer`/`unpack`；矩阵基底回归测试名为 `RejectsNonRgbgMatrixBasis`。

- 归一化测试先因缺少实现链接失败，再通过 27 个基线/归一化测试；解码测试同样先因
  缺函数失败，补实现后 36 个测试通过。

- 真实 LibRaw 合成解包测试暴露最后编码行等价绿色索引 1/3 混用；用生成的 8×8
  buffer 复现并核对上述源码。适配层只在颜色、黑电平校正和白平衡均一致时接受，
  新增回归测试拒绝不等校准，未放宽数值容差。该测试明确覆盖合成相机身份以进入
  有限适配层，不是 Sony 文件解码证据。

- 审查发现其他通道描述可能使保存的矩阵语义错误；对应测试先失败，加入 RGBG
  通道编号要求后通过。

## 安全与代码审查

使用 code-review-and-quality 和 differential-review 自审：先审测试，再审全部新
头/源、CMake/CI 改动和双语文档。基线 eba0f64 的 P2 校验未变，历史和 blame 未见
安全校验被移除。新文件解析边界风险高，归一化/API 中等，文档/构建注册较低。合成
范围内无遗留严重或必须修改代码发现；结论是保留真实文件门槛，不宣称完整 P3 验收
或任意不可信文件的加固支持。

调用关系：

```text
decode_raw -> LibRaw open_file/unpack -> copy_libraw_result -> copy_active_mosaic
normalize -> existing ImageBuffer allocation and row access
```

每个私有适配函数有一个生产调用点；两个公开 API 尚无定义以外的生产调用点，CLI
集成属于后续范围。P2 原本没有解析器，因此未弱化旧解析校验。审查了黑电平加法
溢出、pitch×height 溢出、裁剪越界、空/短 storage、错误通道基底、非有限参考及
伪造元数据；相关测试在受影响像素读取/运算前拒绝。RAII 在成功/异常时销毁 LibRaw，
返回图像拥有独立分配。未发现逐像素分配或隐式裁剪。

限制：未完成 LibRaw 全库安全审计/fuzzing，第三方二进制未加 sanitizer，未验证真实
Sony 文件成功/损坏路径，未在 Windows/Linux 运行，未新跑远端 CI，也无人工视觉/
学习验收。
LibRaw 不公开分配容量；适配层校验几何后仍信任成功 unpack 的长度契约。
白平衡仅在所用增益全为正时存在；非零有限矩阵会保存，但不保证校准质量。规格和
清单保留这些限制。

## 学习同步要点

独立学习任务可同步：码值与饱和参考的区别；CFA 四个空间位置与 RGB 索引；有效区域
相位与传感器 margins；字节 pitch 与元素 stride；无符号减法与 double 中间值；
保留负值/超一值；解码器所有权与返回值所有权；合成证据与相机集成证据。可先预测
上述手算向量再看结果。这只是主题交接，不代表学习掌握记录。

相关资料与证据链接（保留原记录来源）：

- [0.22.1 source](https://github.com/LibRaw/LibRaw/blob/0.22.1/src/utils/open.cpp)
