# Early Risk Register / 前期风险清单

Status / 状态：Initial

Date / 日期：2026-07-09

## Purpose / 目的

This register records the problems most likely to make Mini-Camera Raw look
successful while weakening its engineering or learning value. Review it at
stage boundaries and whenever evidence contradicts an assumption.

本清单记录那些最可能让 Mini-Camera Raw “表面上完成”，却削弱工程质量或学习价值的问题。每个
阶段边界都应重新检查；当证据与假设不一致时也要立即更新。

## Highest-Concern Areas / 最担心的领域

### 1. Plausible Images Without Proven Correctness / 图像能看但无法证明正确

A pipeline can produce a pleasant image while using the wrong black level,
CFA phase, channel gain, matrix direction, numeric range, or transfer
function. Visual inspection alone cannot distinguish many of these errors.

即使黑电平、CFA 相位、通道增益、矩阵方向、数值范围或传递函数存在错误，流水线仍可能生成一张
看起来不错的图像。仅靠肉眼很难识别这些问题。

Mitigation / 应对：

- define input, output, units, range, color state, and invariants for every stage
- keep a simple scalar reference implementation
- use synthetic patterns with analytically predictable outputs
- compare against documented reference vectors or trusted implementations
- use numeric metrics and tolerances in addition to visual comparisons

- 为每个阶段定义输入、输出、单位、范围、色彩状态和不变量
- 保留简单的标量参考实现
- 使用结果可解析预测的合成图案
- 与有文档依据的参考向量或可信实现比较
- 除视觉比较外，同时使用数值指标和容差

Exit evidence / 解除风险的证据：a stage contract, reference tests, and a
documented visual check all agree.

解除风险的证据：阶段契约、参考测试和有记录的视觉检查结论一致。

### 2. Losing Explicit Pipeline State / 流水线状态变得隐含

The same array of floating-point values can mean normalized Bayer data, linear
camera RGB, linear working RGB, or display-encoded RGB. If that state is
carried only in comments or function names, invalid stage ordering becomes
easy.

同一组浮点数组可能表示归一化 Bayer 数据、线性相机 RGB、线性工作空间 RGB 或显示编码 RGB。
如果状态只存在于注释或函数名中，就很容易出现非法的处理顺序。

Mitigation / 应对：

- make pixel format, CFA pattern, color state, range, and row layout explicit
- define stage contracts before implementation
- reject incompatible inputs at public boundaries
- do not allow implicit linear/nonlinear conversion

- 显式表达像素格式、CFA 排列、色彩状态、范围和行布局
- 实现前定义阶段契约
- 在公开边界拒绝不兼容输入
- 不允许隐式的线性/非线性转换

Exit evidence / 解除风险的证据：invalid stage combinations are difficult to
represent and are covered by tests.

解除风险的证据：非法阶段组合难以被表达，并且已有测试覆盖。

### 3. Completing Features Without Owning the Learning / 功能完成但原理没有真正掌握

Agent-generated code can move faster than the learner's mental model. The
project would then become a demonstration produced for the learner instead of
an engineering exercise completed by the learner.

Agent 生成代码的速度可能超过学习者建立心智模型的速度，最终项目会变成“替学习者做出的演示”，
而不是学习者真正完成的工程训练。

Mitigation / 应对：

- write Chinese principle notes before each major algorithm
- predict the result of at least one test case before running the code
- explain the formula, data range, and failure modes after implementation
- keep a short “what I can now explain” checkpoint for each stage
- use notebooks for derivation and exploration, not as a replacement for C++

- 每个主要算法开始前先写中文原理笔记
- 运行代码前，先预测至少一个测试样例的结果
- 实现后解释公式、数据范围和失效模式
- 每个阶段保留简短的“我现在能够解释什么”检查点
- 使用 notebook 做推导和探索，但不能替代 C++ 实现

Exit evidence / 解除风险的证据：the learner can explain and modify the
algorithm without relying on the generated implementation as the explanation.

解除风险的证据：学习者能够解释并修改算法，而不是把生成的实现本身当作解释。

### 4. Uncontrolled RAW Sample Scope / RAW 样张范围失控

Different cameras vary in CFA pattern, black levels, white levels, metadata,
color matrices, compression, and sensor behavior. Supporting “RAW” in general
too early would hide many camera-specific assumptions.

不同相机在 CFA 排列、黑白电平、元数据、色彩矩阵、压缩方式和传感器行为上存在差异。过早声称
支持通用 “RAW”，会掩盖大量相机相关假设。

Mitigation / 应对：

- begin with synthetic buffers and one explicitly selected camera/format
- record camera model, format, license, privacy status, and expected metadata
- separate LibRaw parsing from the project's own ISP transformations
- add another camera only after the first path is tested and documented

- 从合成 buffer 和一种明确选择的相机/格式开始
- 记录相机型号、格式、许可证、隐私状态和预期元数据
- 将 LibRaw 解析与项目自己的 ISP 变换分离
- 第一条路径完成测试和文档后，再加入第二种相机

Exit evidence / 解除风险的证据：the supported sample scope and unsupported
assumptions are written down and reproducible.

解除风险的证据：支持的样张范围和暂不支持的假设已经写明并可复现。

### 5. Non-Reproducible Build or Validation / 构建或验证不可复现

Results may depend on the local Apple Clang version, dependency installation,
build type, CPU, color-managed viewer, or hidden LibRaw defaults.

结果可能依赖本机 Apple Clang 版本、依赖安装方式、构建类型、CPU、色彩管理查看器或 LibRaw 的
隐藏默认设置。

Mitigation / 应对：

- pin minimum tool versions and record detected versions
- provide documented CMake configure, build, and test commands
- add Linux CI after the local baseline works
- record all LibRaw options and avoid hidden processing
- distinguish Debug correctness runs from Release benchmarks
- record benchmark hardware, input, warm-up, repetitions, and measured scope

- 固定最低工具版本并记录实际检测到的版本
- 提供明确的 CMake 配置、构建和测试命令
- 本地基线稳定后加入 Linux CI
- 记录全部 LibRaw 选项，避免隐藏处理
- 区分 Debug 正确性运行与 Release 性能测试
- 记录 benchmark 的硬件、输入、预热、重复次数和测量范围

Exit evidence / 解除风险的证据：a clean checkout builds and produces
equivalent test results on local macOS and CI.

解除风险的证据：干净 checkout 能在本地 macOS 和 CI 上构建，并产生等价测试结果。

### 6. Summer Scope Expansion / 暑期范围持续膨胀

RAW decoding, demosaicing, color science, tone mapping, UI, optimization, and
GPU work are each substantial topics. Starting all of them can leave every
part shallow.

RAW 解码、去马赛克、色彩科学、影调映射、UI、性能优化和 GPU 本身都是较大的主题，同时展开会
导致每一部分都停留在浅层。

Mitigation / 应对：

- keep one active stage and one active algorithm at a time
- define a minimum learning outcome and stop condition for every stage
- treat UI, multithreading, and Metal as optional until the CPU pipeline is correct
- review scope and available time at each checkpoint

- 同一时间只推进一个阶段和一个算法
- 为每个阶段定义最低学习成果和停止条件
- CPU 流水线正确之前，把 UI、多线程和 Metal 视为可选项
- 每个检查点重新评估范围和剩余时间

Exit evidence / 解除风险的证据：each completed stage is independently
explainable, testable, and usable even if later stages are cut.

解除风险的证据：即使砍掉后续阶段，每个已完成阶段仍能独立解释、测试和使用。

Current schedule decision / 当前进度决定：

The 2026-07-18 to 2026-08-02 pause leaves roughly three active development
weeks before the fixed 2026-08-25 deadline. Therefore the August acceptance
baseline contains Stage 0, a simple complete Stage 1 CPU pipeline, and a
reduced Stage 2 tone engine. Stage 3 and Stage 4 move to September.

2026-07-18 至 2026-08-02 的暂停期使固定的 2026-08-25 截止日期前大约只剩
3 个有效开发周。因此，8 月验收基线包含阶段零、简单但完整的阶段一 CPU 流水线，
以及缩减后的阶段二影调引擎；阶段三和阶段四移至 9 月。

## Work Worth Doing Before Algorithm Implementation / 算法实现前值得完成的工作

1. Resolve Stage 0 decisions: test framework, core-library plus CLI target,
   dependency acquisition policy, license author string, and first camera/sample.
2. Create the first ADRs for image-buffer layout, numeric representation, and
   dependency policy.
3. Write a pipeline contract table covering every planned stage's input,
   output, color state, range, invariants, and error policy.
4. Define the validation policy: comparison metrics, floating-point tolerances,
   synthetic fixtures, golden-output policy, and visual-review record.
5. Record the development environment: OS, architecture, compiler, CMake, and
   package-manager versions.
6. Turn the approved Stage 0 specification into `tasks/plan.md` and
   `tasks/todo.md`.
7. Establish a weekly cadence: principle study, design, implementation,
   verification, review, and learning recap.

1. 解决阶段零决策：测试框架、核心库与 CLI target、依赖获取策略、许可证署名和第一种相机/样张。
2. 为图像 buffer 布局、数值表示和依赖策略创建首批 ADR。
3. 编写流水线契约表，记录每个计划阶段的输入、输出、色彩状态、范围、不变量和错误策略。
4. 定义验证策略：比较指标、浮点容差、合成 fixture、golden 输出政策和视觉审查记录。
5. 记录开发环境：操作系统、架构、编译器、CMake 和包管理器版本。
6. 把确认后的阶段零规格拆成 `tasks/plan.md` 和 `tasks/todo.md`。
7. 建立每周节奏：原理学习、设计、实现、验证、审查和学习复盘。

## Review Rhythm / 复查节奏

Review this register at the start and end of every stage. Add evidence rather
than deleting risks. Mark a risk controlled only when its exit evidence exists.

每个阶段开始和结束时复查本清单。应补充证据，而不是简单删除风险。只有解除风险的证据真实存在时，
才能把该风险标记为已控制。
