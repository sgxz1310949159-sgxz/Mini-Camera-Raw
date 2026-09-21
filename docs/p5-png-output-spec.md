# P5 png-output / P5 PNG 输出契约

Status / 状态：Accepted within implementation authorization, 2026-09-21 / 在用户实施授权内采用。

`write_png16(encoded,path)` accepts sRGB uint16 EncodedRgb only; no color conversion,
WB, rotation or copied EXIF. Conventional libpng writes RGB16, no alpha/interlace,
sRGB relative-colorimetric intent, explicit network-order bytes. Respect input stride.
Use installed libpng/zlib via PNG::PNG. No third-party source copied into repository.
函数只接受 sRGB uint16 编码图像，不做变换/白平衡/旋转或复制 EXIF；常规 libpng
写 RGB16、无 alpha/交错、sRGB 相对比色标识和显式大端字节，遵守 stride。
通过 PNG::PNG 使用系统 libpng/zlib，不复制第三方源码。

Reject empty/NUL paths, dimensions above PNG_UINT_31_MAX, and row-size overflow before
opening. POSIX exclusive creation (macOS/Linux baseline) refuses existing files,
symlinks and directories. On a normal write/flush/close error, remove only the newly
created file; check identity before cleanup. No crash-atomic guarantee and no promise
against concurrent pathname or parent-directory replacement; callers must keep the output path stable. No partial successful result.
路径空/NUL、PNG 尺寸上限和行大小溢出在打开前拒绝。macOS/Linux 基线用 POSIX
排他创建拒绝已有文件、符号链接和目录。普通写入/刷新/关闭失败时，仅清理本次新建
文件并核对身份；不保证崩溃原子性或对抗恶意并发父目录替换，不返回部分成功。

Keep setjmp/longjmp in a private helper with only trivial locals and no exception
crossing C callbacks. Allocate a row buffer before entering it. C++ owns resources
outside the helper; convert errors to runtime_error without private paths.
私有 helper 只用平凡局部变量处理 longjmp，不跨 C++ 析构或 C 回调抛异常；外层
先分配行 buffer 并管理资源，错误转 runtime_error，不包含私人路径。

Tests independently inspect PNG chunks/CRC/zlib scanlines, decode sample integers,
sRGB metadata, odd/padded layout and >65535-byte rows; verify existing-target
preservation, symlink rejection, missing-parent and injected I/O failure cleanup.
Libpng and zlib versions must be reported; patched distribution packages allowed.
测试独立解析 chunk/CRC/zlib 扫描行、精确整数像素/色彩标识、奇数/padding/宽行，
验证已有目标/符号链接保护、缺失目录和注入 I/O 错误清理。记录库版本，允许发行版
安全补丁包。来源及依赖评估见 [preparation](p5-design-preparation.md)。

The scalar writer uses PNG_FILTER_NONE; compressed byte identity and file size are not acceptance criteria. png_library_version() returns the linked runtime version.
标量写入器使用 PNG_FILTER_NONE，不以压缩字节一致或文件大小为验收标准；版本函数返回实际链接库版本。
