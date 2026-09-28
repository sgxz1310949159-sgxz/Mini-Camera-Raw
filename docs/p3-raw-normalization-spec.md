# P3 RAW 读取与归一化

状态：已授权 P3 范围内的实施契约

日期：2026-09-10

## 范围与 API

使用 C++17、现有 ImageBuffer 和系统 LibRaw（本机 0.22.1）。`decode_raw(path)`
返回拥有所有权的有效区域 uint16 Bayer 图像和传感器元数据；`normalize(raw, levels)`
返回紧密排列的 float LinearBayer。两个函数独立，合成归一化无需文件。公开声明位于
`include/mini_camera_raw`，实现在 `src`，确定性 GoogleTest 在 `tests`。
沿用 snake_case 函数、PascalCase 类型和标准异常，例如：

```cpp
auto decoded = mini_camera_raw::decode_raw(path);
auto linear = mini_camera_raw::normalize(decoded.image, decoded.sensor.levels);
```

## 解码契约

有效区复制索引为 `raw_image[(top + y) * raw_pitch / 2 + left + x]`；各位置黑电平为 `color.black + color.cblack[COLOR(y,x)]`；白电平采用解包后 `color.maximum`。

- 文件仅支持 Sony ILCE-7CM2 原生 RAW、单帧、方形像素、RGBG 通道编号的规则 2×2 RGB Bayer 和整数
  样本。拒绝 DNG、其他相机、单色、X-Trans、全彩图、浮点 RAW 和非 2×2 可表达的
  黑电平表（首版拒绝所有空间黑电平表）。这是兼容目标，真实相机验证仍为独立门槛。

- 图像生成只调用 `open_file` 与 `unpack`；关闭 RawSpeed 和浮点转整数，选第零帧。
  不调用 raw2image、subtract_black、dcraw_process、缩略图提取或任何输出操作。

- 按上述索引将有效区域复制到紧密 owned storage；访问前检查非零尺寸、以减法验证
  裁剪边界、偶数字节 pitch、最小 pitch、元素/字节乘法及存储长度。
LibRaw 不公开
  分配容量，适配层依赖成功 unpack 的缓冲区长度契约；合成复制测试显式提供长度。

- 规则 filters 相对于有效区域原点（已对照 0.22.1 raw2image 源码）。COLOR(y,x)
  不再加 margins；校验编码八行重复同一 2×2 颜色及黑电平/白平衡参考。绿色索引 1/3
  仅在黑电平与白平衡相同时可互换，两个空间绿色位置仍保持独立。

- 每个有效区域 tile 位置黑电平为公共 black 加相应 cblack；遇到空间黑电平表维度
  非零则拒绝。白电平取 unpack 后 maximum，绝不取依赖画面内容的 data_maximum。

- 记录完整/有效区域、字节 pitch、报告的源位数（0 表示未知，不由白电平反推）、
  方向（不旋转）、版本、解包标志、可用时按位置排列的相机白平衡和 3×4 rgb_cam
  （相机 RGBG 到线性 sRGB）。不应用增益/矩阵；可选数据缺失保持为空，非有限值报错。

- RAW 存储声明 [0,65535]，与传感器白电平及压缩位深独立，因为解包可能扩展码值，
  超白电平样本仍需保留。

- 非法契约/路径（空或含 NUL）抛 invalid_argument；尺寸溢出抛 length_error；不支持
  格式、I/O、解包失败和非致命损坏抛 runtime_error。C++ 分配异常透传，LibRaw 分配
  失败状态与其他失败状态一样转为 runtime_error；错误文本不含私人
  路径，不返回部分结果。

## 归一化契约

`p = 2*(y%2) + x%2; output = float((double(raw) - black[p]) / (white[p] - black[p]))`.

SensorLevels 以有效区域 tile 的 00、01、10、11 顺序保存四个黑/白参考，单位为解包
码值。逐样本执行上述公式，要求参考有限且 0 <= black < white <= 65535。double
计算，转 float 前拒绝非有限或 float 溢出结果。输入必须为 RawBayer uint16；所有
float 输入（包括已归一化 Bayer 和 NaN/Inf）直接拒绝。检查有效像素符合 RAW 声明
区间，padding 不算像素并忽略。保持宽、高、CFA、负值和超一值，输出名义 [0,1]
允许超范围。不修改输入，仅分配一个输出、标量逐行遍历，P3 不优化也不宣称性能。
非法输入抛 invalid_argument，无法表达的数值结果抛 overflow_error。

## 验收与命令

测试手算向量、全部 CFA、单像素、奇数尺寸、padding 不变量、单调性、端点及超范围
保留、非法电平/状态、裁剪/pitch/长度/溢出、所有权和解码失败。整数复制须精确，
float 误差限为 `abs(error)<=1e-6+1e-6*abs(reference)`。合成测试不证明 Sony 文件解码或视觉质量；所选真实样张读取需
明确授权，之后记录数值指标和马赛克检查。学习仍由独立学习任务负责，不复制私人笔记或 RAW。

```sh
cmake -S . -B build/p3 -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/p3
ctest --test-dir build/p3 --output-on-failure
cmake -S . -B build/p3-no-tests -G Ninja -DBUILD_TESTING=OFF
cmake --build build/p3-no-tests
git diff --check
```

缓存参数为 `-DFETCHCONTENT_SOURCE_DIR_GOOGLETEST=/absolute/cache/path`。
可用上述 CMake 参数指定现有 GoogleTest 源码缓存，并记录该例外。完成前必须验证；
新依赖和真实样张读取需先确认；本次不提交/推送，不实现白平衡、去马赛克、色彩/输出或并行优化。

## 来源

- [数据结构](https://www.libraw.org/docs/API-datastruct.html)

- [坐标映射](https://github.com/LibRaw/LibRaw/blob/0.22.1/src/preprocessing/raw2image.cpp)

- [解包黑电平调整](https://github.com/LibRaw/LibRaw/blob/0.22.1/src/decoders/unpack.cpp)

- [CMake FindPkgConfig](https://cmake.org/cmake/help/v3.24/module/FindPkgConfig.html)

同时核对了本机对应 0.22.1 头文件和随包 API 文档。

相关资料与证据链接（保留原记录来源）：

- [LibRaw C++ API](https://www.libraw.org/docs/API-CXX.html)
