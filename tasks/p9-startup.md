# P9 启动与环境核查

## 当前有效状态 — 2026-10-12

P9功能、输出一致性、性能/内存及真实竖拍人工验收已通过；最终代码审查未发现必改问题，全新构建验证完成。基础影调停止调优；粉色高光和彩点仍保持未解决的独立质量项。暂不提交、推送或启动P10，阶段备份未执行。完整状态以[最终审查与收尾准备](p9-closeout.md)为准。

下文按日期保留历史过程；旧FIFO、旧尺寸及“待竖拍/待性能”描述不是当前要求，不重新开启已完成验收。

日期：2026-09-29
状态：已授权启动；步骤1环境准备进行中，最小窗口及显示路径尚未完成。

## 最新确认与工作边界

用户确认按既有七步顺序实施 P9，本工程窗口指定 GPT-6 Astra、medium；课堂继续 GPT-6 Sol。本记录是授权状态记录，不是模型运行时证明。
P8 已完成验收、提交、备份和远端合并；旧文档中的待收尾文字是历史检查点。
此次授权不含 commit、push、PR、merge 或 P10 启动；不重复备份、不挂载 SGXZPAN、不发送课堂消息。

学习仅三个主题：图像数据如何成为窗口图像；自动转正与像素坐标；图像缩放、平移与高像素密度显示。
第四项性能测量课程已移除，工程耗时/内存测量验收保留。只读检查主目录 learning/00-study-index.md 时只见 P8 通过记录，未见 P9 通过记录；不据此判断课堂当前是否正在进行。
独立环境准备可执行；依赖上述主题的显示、方向、缩放选择及实现须遵守学习后确认关口。

七步依次为：依赖/可选GUI/最小窗口；无提前裁剪、无P6肩部的共享RGB16 sRGB输出；RAW打开与自动转正；重复曝光与复位；有限FIFO/串行工作线程/呈现确认/失败恢复；超100%缩放、平移、视点保持与快照PNG导出；综合验证与时间/内存测量。
普通调整体验、队列容量、响应阈值与缓存方案在首个完整版本实测后由用户确认。低分辨率仅候选；不提前加入多核/SIMD/GPU或P11/P12算法。

## 工作区与基线证据

- 主目录仍有 README.md、docs/repository-boundary.md、tasks/todo.md 修改和未跟踪 tasks/stage0-closeout.md；全部保留。
- 复用原 p8-editing-contracts 工作区；起始干净，未发现引用它的运行中构建。
- 远端 `git ls-remote origin refs/heads/main` 实时返回 `5b20a9360732771bb8f78568762fb2b2dfeacc60`。
- 从该合并提交建立 `codex/p9-editor`；未移动旧主目录 main。P8提交4e65dd1与合并提交文件树相同。
- 本窗口附件列表为空；工具无附着既有工作区接口，按交接直接使用已有路径，没有另建工作区。
- 本地备份凭据确认快照20260928-192023-p8-routine覆盖4e65dd1，content/sample/Git恢复验证为true，restore_verified为false。本轮未重新验证外盘内容。

## 实测环境与基线

macOS 27.0 (26A428)，arm64；Xcode 27.0 (27A266a)，Apple Clang 21.0.0；CMake 4.3.4、Ninja 1.13.2、LibRaw 0.22.1、libpng 1.6.58。构建采用当前Xcode，不修改系统开发目录选择。

从本工作区运行：

```sh
cmake -S . -B build/p9-baseline -G Ninja -DCMAKE_BUILD_TYPE=Debug -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST='/Users/sgxz/Mini - Camera Raw/build/p2-verify/_deps/googletest-src'
cmake --build build/p9-baseline
ctest --test-dir build/p9-baseline --output-on-failure
```

配置和41项构建步骤成功；102/102测试通过（CTest总耗时2.25秒，不是性能验收）。链接仍有重复-lc++/静态库警告，未导致失败。此次基线不包含GUI、新算法、真实RAW视觉检查或Release性能测量。

## Qt依赖核查与安装边界

PATH、Homebrew已安装清单及常见Qt安装目录未发现Qt。Homebrew配方候选qtbase 6.11.2，包含所需Core/Gui/Widgets；尚未锁定为已安装版本。
尝试安装时设置NO_AUTO_UPDATE及NO_INSTALL_CLEANUP，但安装器仍要求升级14个已安装依赖；这两个选项不禁止依赖升级。
本轮在下载阶段用SIGINT停止自己启动的安装进程（退出130），日志未出现安装/Pouring阶段。复查qtbase未安装；已下载的brotli、freetype、pixman、libxext、libunistring仍是原安装版本，下载缓存保留。
此处为首次检查历史状态；用户随后确认独立安装，结果见下方更新。

官方资料核查：

- [Qt macOS平台](https://doc.qt.io/qt-6/macos.html)：Qt6.11表列Xcode15及以后、macOS13及以后（明确列到26）；本机27仍须实际启动验证。
- [Qt Widgets](https://doc.qt.io/qt-6/qtwidgets-index.html) 与 [Qt GUI](https://doc.qt.io/qt-6/qtgui-index.html)：所需模块有LGPLv3/GPLv2及商业许可选项；只计划动态链接Core/Gui/Widgets，发行前仍须核对对应版本许可文本、第三方声明及可替换/重新链接等义务，不视为已完成分发合规。
- [Qt许可](https://doc.qt.io/qt-6/licensing.html)：不能把所有Qt模块当作相同许可；当前不引入GPL-only附加模块。
- [CMake起步](https://doc.qt.io/qt-6/cmake-get-started.html)：GUI候选采用find_package(Qt6 COMPONENTS Widgets)和目标级链接；保持核心C++17、Qt可选，尚未修改构建文件。
- [QImage](https://doc.qt.io/qt-6/qimage.html)：格式、颜色空间标记与实际颜色转换分别核查；支持16位图像不证明屏幕16位输出。显示适配及显示器颜色转换路径未定稿、未验收。

## 下一可执行检查点

1. 完成Qt安装方案与实际版本验证；形成只验证窗口/事件循环的环境探针，不提前决定像素显示API。
2. 第一主题学习及相关选择确认后，补显示缓冲布局、颜色转换、生命周期的具体契约与最小图像窗口。
3. GUI关闭构建须继续通过全部既有测试，GUI开启须真实macOS窗口运行；满足全部要求后才勾选步骤1。

本轮仅形成启动记录和基线证据，未修改生产代码或学习记录。没有P9步骤被整体记为通过。

## 独立安装完成 — 2026-09-29

用户明确同意独立安装，保留Homebrew共享依赖。使用仅本地的aqtinstall 3.3.0安装工具（不是产品运行依赖）获取Qt官方仓库qtbase归档；工具运行在独立Python虚拟环境中。

- 实际安装：Qt 6.11.2，`build/p9-qt/6.11.2/macos`，约177 MB；不增加全局PATH，不修改系统Python或Homebrew。
- 官方仓库查询架构名为clang_64；实际归档名含X86_64-ARM64，构建产物已验证为arm64。
- 仅安装qtbase归档；未安装Qt Creator、QML/WebEngine等额外归档。qtbase内含其他基础模块，但探针只链接Core/Gui/Widgets。
- 安装前后完整Homebrew版本清单逐字比较一致。Core/Gui/Widgets动态依赖检查没有/opt/homebrew路径。
- qmake实际报告6.11.2；独立CMake工程用EXACT版本查找、C++17构建成功。
- 原生窗口日志：`Qt 6.11.2 platform "cocoa" visible true exposed true DPR 2`。这是运行时窗口暴露与事件循环证据，不是人工视觉/显示颜色验收。
- 探针、安装日志和Homebrew前后清单在忽略目录 `build/p9-qt-probe/`；安装工具和Qt分别在 `build/p9-tools/`、`build/p9-qt/`，均不进Git。
- 系统Python3.9.6的LibreSSL触发urllib3兼容警告；本次HTTPS查询/下载成功，未禁用证书验证。工具不进入产品依赖；以后若需长期维护安装工具，可迁移到独立受支持Python。

复现命令（在本工作区运行）：

```sh
python3 -m venv build/p9-tools/aqt-venv
build/p9-tools/aqt-venv/bin/python -m pip install aqtinstall==3.3.0
build/p9-tools/aqt-venv/bin/python -m aqt list-qt mac desktop --arch 6.11.2
build/p9-tools/aqt-venv/bin/python -m aqt install-qt mac desktop 6.11.2 clang_64 --archives qtbase --outputdir build/p9-qt
cmake -S build/p9-qt-probe/source -B build/p9-qt-probe/out -G Ninja -DCMAKE_PREFIX_PATH="$PWD/build/p9-qt/6.11.2/macos"
cmake --build build/p9-qt-probe/out
build/p9-qt-probe/out/p9-qt-probe
```

工具参数依据：[aqtinstall维护者文档](https://aqtinstall.readthedocs.io/en/latest/cli.html)。窗口探针依据：[QApplication官方文档](https://doc.qt.io/qt-6/qapplication.html)。
独立Qt环境验证通过；正式可选GUI目标、图像颜色显示路径与对应学习确认仍未完成，步骤1不能整体勾选。未修改生产代码，未重跑无变化的102项核心测试；上一检查点结果仍作为该基线证据。

## 课堂核对及首个实现增量 — 2026-10-10

只读核对主目录第十八、十九、二十课和学习索引，三项已完成；前两项2026-10-08通过，第三项2026-10-10在补充讲解后由用户确认结束。用户要求检查并开始后续内容，本轮进一步明确同意RGBX64独立副本与sRGB窗口加macOS显示转换。

已新增可选GUI目标、应用层像素适配、3项数值测试与合成图窗口；详见docs/p9-preview-spec.md。关闭102/102、开启105/105及本机窗口冒烟通过，已观察色块顺序。原生CUA最初无法识别裸可执行文件，改用标准macOS .app后成功读取窗口及截图；不以第一次失败为通过证据。

自审覆盖测试输入输出、源stride、QImage所有权、颜色状态、整数尺寸检查及Qt依赖隔离；未发现本增量必改项。未注入分配失败、未验证其他平台、未进行系统查看器颜色对照/显示校准或性能验收。生产核心算法未改，未commit/push/merge、未备份。学习文件始终只读。

## 人工颜色对照检查点 — 2026-10-10

用户要求持续推进，仅在需要人工验证时停止。当前按既定顺序完成步骤1的人工颜色对照，再推进步骤2；后续实施许可不重复索取。

已在build/p9-step1-evidence/color-reference.png生成900×600 RGB16 sRGB参考图，六块颜色对应窗口3×2合成图的同一组编码值。使用标准PNG块编码，验证CRC、sRGB块和解压像素往返；尺寸扩大仅用于方便查看。
项目窗口可由原生工具识别；系统Preview文件选择框未响应自动输入，尚未自动打开参考图，不记人工对照通过。请用户在同一显示器、相同亮度/系统显示设置下打开参考图，与项目窗口比较RGB顺序及白灰黑是否存在明显色偏/亮度差异。此项仅为基本一致性观察，不是校准或屏幕位深验收。
等待本人观察结果；生产代码未在此检查点继续修改。

## 步骤1人工确认与步骤2/3推进 — 2026-10-10

用户明确核对合成参考图“无明显差异”，第1步基础对照通过。按持续推进授权实现共享输出、自动转正及后台RAW打开；未进入P10。
三张真实样张打开成功且原件哈希不变；四方向用合成精确向量通过，不把三张横向输出当作真实竖图验证。当前停在步骤3本人显示与交互检查，后续曝光/复位、FIFO、查看/导出仍待实现。
后台线程首次RAW打开发生SIGBUS，系统报告栈溢出；最小缺失路径回归复现，LibRaw堆分配修复后111/111与108/108及真实样张重跑通过。原生GUI运行日志与失败证据保留在忽略目录。

本次原生工具已读取新版窗口“Mini-Camera Raw — P9”，状态为已显示7028×4688、0EV；Agent截图观察到日光样张完整显示、无显著方向或行错位。已关闭本助手此前启动的旧合成色块进程，保留新版照片窗口供本人检查。
raw-identify核对现有三张样张的Image flip均为0；真实竖拍验证缺口明确保留，不能用合成旋转结果替代。

2026-10-10：收到5517太阳核心粉色反馈；当前流水线数值复现，见P9规格末尾。另两张本人未见明显问题；高光观感仍未通过，按已选D4 B进入P10复核，不撤回保留余量决策。
2026-10-10: The reported pink solar core was reproduced numerically before Qt. Other two images had no obvious user-observed issues. Highlight appearance remains unresolved for P10 under accepted D4 B; headroom preservation remains in effect.

2026-10-10步骤4/5：工程与自动检查完成，Release供本人试用。先确认曝光0→+2→−1→复位和短距离拖动的等待体验，再继续步骤6缩放/平移/导出；容量仍为暂定8项，性能目标未计通过。详细证据见P9规格末尾。
