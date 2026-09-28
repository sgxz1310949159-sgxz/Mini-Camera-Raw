# ADR-003: 依赖策略

状态：Accepted

日期：2026-07-09

## 背景

测试依赖需要在干净 checkout 间保持可复现，而大型运行时库不应在每次 CMake 配置时
都重新下载和编译。

## 决定

- 使用 CMake 3.24 或更高版本。

- 使用 `FetchContent` 获取 GoogleTest 1.17.0 官方发布归档，并以 SHA-256
  `65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c`
  固定归档；发布 tag 对应提交
  `52eb8108c5bdec04579160ae17225d66034bd723`。

- LibRaw 等大型运行时依赖使用系统安装包。

- 依赖提供时，优先使用带命名空间的 imported CMake target。

- 不把第三方源码树复制进本仓库。

- 在构建输出或环境文档中记录依赖版本。

## 影响

第一次配置测试时需要网络访问，除非 GoogleTest 已在缓存中。阶段一必须显式安装并
发现 LibRaw。

## 资料依据

- GoogleTest 官方 CMake 快速入门提供 `FetchContent`、`GTest::gtest_main`
  和 CTest 集成示例。

- GoogleTest 1.17.0 是已核实并固定的正式版本。

- CMake 3.24 官方文档建议对不受项目控制的远端内容使用提交哈希，并记录了
  `FetchContent_MakeAvailable()` 与 `gtest_discover_tests()` 的行为。

相关资料与证据链接（保留原记录来源）：

- <https://google.github.io/googletest/quickstart-cmake.html>

- <https://github.com/google/googletest/releases/tag/v1.17.0>

- <https://cmake.org/cmake/help/v3.24/module/FetchContent.html>

- <https://cmake.org/cmake/help/v3.24/module/GoogleTest.html>
