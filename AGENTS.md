# Agent Working Agreement / Agent 工作规范

This file applies to the entire repository. It defines how an agent should
plan, implement, verify, review, and publish work for Mini-Camera Raw.

本文件适用于整个仓库，用于规定 Agent 在 Mini-Camera Raw 中如何规划、实现、验证、审查和发布工作。

## 1. Project Purpose / 项目目标

Mini-Camera Raw is a learning-first C++ image-processing project. The primary
goal is to understand RAW data, ISP pipeline algorithms, digital image signal
processing, numerical behavior, and engineering practice. Shipping many
features is less important than making each stage correct, explainable,
testable, and reproducible.

Mini-Camera Raw 是一个以学习为首要目标的 C++ 图像处理项目。核心目标是理解 RAW 数据、ISP
流水线算法、数字图像信号处理、数值行为和工程实践。功能数量不是首要指标；每个阶段都应做到正确、
可解释、可测试、可复现。

Before changing code, read the relevant parts of:

修改代码前，应阅读与当前任务相关的以下文件：

- `docs/project-foundation.md`: project positioning, roadmap, and learning loop
- `docs/stage0-engineering-spec.md` or the active stage specification
- `docs/early-risk-register.md`: current risks, warning signs, and mitigations
- `docs/repository-boundary.md`: public and local-only content rules
- the current task plan under `tasks/`, when it exists

- `docs/project-foundation.md`：项目定位、路线图和学习闭环
- `docs/stage0-engineering-spec.md` 或当前阶段规格
- `docs/early-risk-register.md`：当前风险、预警信号和应对措施
- `docs/repository-boundary.md`：公开内容与仅本地内容的边界
- `tasks/` 下当前任务计划（如果存在）

When instructions conflict, use this order: the user's latest request, this
file, the active stage specification, other project documents, then generic
skill defaults. Surface any material conflict before proceeding.

发生冲突时，优先级依次为：用户最新要求、本文件、当前阶段规格、其他项目文档、通用 skill
默认规则。若冲突会影响结果，应先明确指出。

## 2. Repository Boundaries / 仓库边界

- Keep all personal study notes under `learning/`. They must be written in
  Chinese and must remain ignored by git.
- Public documents must provide equivalent Chinese and English content.
- Do not commit personal RAW photos, the original planning document, large
  outputs, local benchmark results, build products, credentials, or private
  reflections.
- Synthetic fixtures and redistributable samples may be committed when their
  source and license are clear.
- Promote a learning note into `docs/` only after rewriting it as polished,
  reproducible public documentation.

- 所有个人学习笔记都放在 `learning/` 下，必须使用中文，并始终保持 git 忽略状态。
- 公开文档必须提供内容等价的中文和英文版本。
- 不提交私人 RAW 照片、原始策划书、大型输出、本地 benchmark 结果、构建产物、凭据或私人反思。
- 只有来源和许可证清楚时，才可提交合成测试数据或允许再分发的样张。
- 学习笔记只有在重写为成熟、可复现的公开资料后，才能整理进入 `docs/`。

## 3. Start-of-Task Protocol / 每次任务的启动流程

For every non-trivial task:

每次开始非简单任务时：

1. Run `git status --short --branch` and preserve all pre-existing user changes.
2. Read the active specification and only the source files needed for the task.
3. Check the available skills and invoke every skill whose trigger clearly
   matches the work. Do not invoke unrelated skills for appearance.
4. State material assumptions, unresolved decisions, and scope boundaries.
   Proceed without blocking on low-risk details that have a conservative
   project-consistent default.
5. Define or confirm testable acceptance criteria before implementation.
6. For multi-step work, maintain an explicit plan and update it as checkpoints
   are completed.

1. 运行 `git status --short --branch`，保留所有任务开始前已经存在的用户修改。
2. 阅读当前规格，以及完成任务真正需要的源文件。
3. 检查当前可用 skill；凡是触发条件明确匹配的都要调用，但不要为了形式调用无关 skill。
4. 说明会影响结果的假设、未决问题和范围边界。对风险低且存在保守默认值的细节，按项目现有约定继续推进。
5. 实现前定义或确认可测试的验收标准。
6. 多步骤任务应维护明确计划，并在每个检查点完成后及时更新状态。

## 4. Required Skill Routing / 必须执行的 Skill 路由

### `using-agent-skills`

Use at the beginning of a substantial or unfamiliar task to identify the
smallest applicable skill set and execution order.

在重大任务或不熟悉的任务开始时使用，用于确定最小必要 skill 集合及执行顺序。

### `spec-driven-development`

Use for a new project stage, ISP algorithm, public API, data model, dependency,
architecture decision, or any feature whose requirements or acceptance
criteria are unclear. Update the specification before implementation. A useful
algorithm specification records:

新阶段、新 ISP 算法、公开 API、数据模型、新依赖、架构决策，或需求与验收标准不清晰的功能，
必须使用。先更新规格，再实现。算法规格至少记录：

- purpose and pipeline position
- input/output color state, CFA pattern, bit depth, numeric range, and units
- formula or reference algorithm and assumptions
- boundary behavior, clipping policy, and error handling
- numeric tolerance, visual checks, and performance expectations

- 目的及其在流水线中的位置
- 输入/输出色彩状态、CFA 排列、位深、数值范围和单位
- 公式或参考算法及其前提
- 边界行为、裁剪策略和错误处理
- 数值容差、视觉检查和性能预期

Skip a full specification for typo-only, formatting-only, or obviously
self-contained one-line changes; still retain a concise acceptance criterion.

纯拼写、格式或边界完全明确的单行修改不需要完整规格，但仍应有简短验收条件。

### `source-driven-development`

Use whenever work depends on LibRaw, CMake, OpenCV, a test framework, a file
format, a compiler feature, or another versioned external API. Detect the
actual version, read the precise official documentation, implement the
documented pattern, and cite the source. For ISP algorithms, extend the source
hierarchy with original papers, standards, and authoritative color-science
references. Clearly label any claim that could not be verified.

工作依赖 LibRaw、CMake、OpenCV、测试框架、文件格式、编译器特性或其他带版本的外部 API 时必须
使用。先确定实际版本，再阅读准确的官方文档，按文档实现并记录来源。对于 ISP 算法，还应优先查阅
原始论文、标准和权威色彩科学资料。无法验证的结论必须明确标记。

### `planning-and-task-breakdown`

Use after an approved specification when implementation order is non-obvious,
the change spans multiple modules, or the work is too large for one focused
session. Store the implementation plan in `tasks/plan.md` and the actionable
checklist in `tasks/todo.md`. Prefer tasks touching no more than about five
files, with explicit dependencies, acceptance criteria, and verification
commands.

规格确认后，如果实现顺序不明显、修改跨多个模块，或工作无法在一次专注会话中完成，必须使用。
将实施计划写入 `tasks/plan.md`，可执行清单写入 `tasks/todo.md`。单个任务尽量不超过约五个
文件，并明确依赖、验收条件和验证命令。

### `api-and-interface-design`

Use before introducing or changing a public header, `ImageBuffer`, pixel
format, metadata type, processing-stage contract, error policy, or ownership
boundary. Define the contract before implementation. Make invalid states hard
to represent, validate external data at the boundary, keep error semantics
consistent, and prefer additive evolution over breaking changes.

新增或修改公开头文件、`ImageBuffer`、像素格式、元数据类型、处理阶段契约、错误策略或所有权边界
前必须使用。先定义契约，再实现。应尽量让非法状态难以表达，在外部数据边界进行校验，保持错误语义
一致，并优先采用增量扩展而不是破坏性修改。

### `incremental-implementation`

Use for every multi-file implementation. Work in the smallest complete slices:
implement, test, verify, review the diff, then continue. Keep the repository
buildable between slices. Do not mix unrelated cleanup, refactoring, and new
behavior. Keep each slice commit-ready; create commits or push only when the
user requests publication or the active task explicitly includes it.

所有多文件实现都必须使用。按最小完整增量推进：实现、测试、验证、审查 diff，然后继续。每个增量
结束时仓库都应可构建。不要把无关清理、重构和新行为混在一起。每个增量应保持可独立提交；只有
用户要求发布，或当前任务明确包含提交/推送时，才创建 commit 或 push。

### `test-driven-development`

Use for new logic, changed behavior, and bug fixes. Follow red, green,
refactor. A bug fix starts with a minimal failing reproduction test. Prefer
tests of observable inputs and outputs over implementation details.

新增逻辑、行为修改和缺陷修复必须使用，遵循红灯、绿灯、重构。修复缺陷时先写能够稳定复现问题的
最小失败测试。优先验证可观察的输入输出，不绑定内部实现细节。

For image algorithms, tests should cover representative examples, empty or
invalid inputs, minimum and maximum values, clipping boundaries, odd image
dimensions, CFA phase where relevant, NaN/Inf handling for floating-point
paths, and tolerance-based comparison.

图像算法测试应覆盖代表性示例、空或非法输入、最小值和最大值、裁剪边界、奇数图像尺寸、相关时的
CFA 相位、浮点路径的 NaN/Inf 处理，以及基于容差的比较。

### `property-based-testing`

Use when an image transform has useful mathematical properties or a large
input domain. High-value properties for this project include identity at
neutral parameters, output range preservation, monotonicity, idempotence,
round trips, equivalence to a simple reference implementation, and invariance
under valid layout changes. Start with deterministic generated cases; add a
library such as RapidCheck only after dependency review and approval.

当图像变换具有明确数学性质或输入域很大时使用。本项目重点性质包括：中性参数下恒等、输出范围
保持、单调性、幂等性、往返转换、与简单参考实现等价，以及合法内存布局变化下的不变量。优先从
确定性生成样例开始；RapidCheck 等新依赖必须经过审查和确认后才能加入。

Do not use property-based testing as a replacement for known reference vectors
or visual validation.

性质测试不能替代已知参考向量或视觉验证。

### `debugging-and-error-recovery`

Invoke immediately when a build, test, benchmark, or runtime result is
unexpected. Stop feature work, preserve the exact evidence, reproduce,
localize, reduce, fix the root cause, add a regression guard, and rerun the
relevant end-to-end path. Never silence a failing test or weaken a tolerance
without explaining why the original expectation was wrong.

构建、测试、benchmark 或运行结果出现异常时立即使用。暂停功能开发，保留完整证据，依次完成复现、
定位、最小化、根因修复、回归保护和相关端到端验证。不得通过屏蔽失败测试或随意放宽容差来“修复”
问题，除非能够说明原预期为何错误。

### `code-review-and-quality`

Use after each completed implementation task and before every merge. Review
tests first, then inspect correctness, readability, architecture, security,
and performance. For ISP code, additionally check color-space ordering,
numeric precision, overflow, clipping, row stride, ownership, cache behavior,
and accidental per-pixel allocation. Report findings by severity with exact
file and line references. If there are no findings, state the remaining test
or benchmark gaps.

每个实现任务完成后以及每次合并前必须使用。先审查测试，再检查正确性、可读性、架构、安全性和
性能。ISP 代码还要检查色彩空间顺序、数值精度、溢出、裁剪、行跨度、所有权、缓存行为和意外的
逐像素内存分配。发现应按严重程度排序，并给出准确文件与行号；若没有问题，应说明仍存在的测试
或 benchmark 缺口。

### `documentation-and-adrs`

Use when choosing a major dependency, image representation, color-space
convention, numeric precision, pipeline order, public API, optimization
strategy, or other decision that would be expensive to reverse. Store
bilingual ADRs under `docs/decisions/` using sequential names such as
`ADR-001-image-buffer-layout.md`. Record context, decision, alternatives,
consequences, status, and date. Never delete an accepted ADR; supersede it with
a new record.

选择重要依赖、图像表示、色彩空间约定、数值精度、流水线顺序、公开 API、优化策略，或其他难以
回退的决策时必须使用。中英双语 ADR 放在 `docs/decisions/` 下，按
`ADR-001-image-buffer-layout.md` 形式连续编号，并记录背景、决策、备选方案、后果、状态和日期。
已经接受的 ADR 不删除；若决策变化，应由新 ADR 替代。

### `jupyter-notebook`

Use for exploratory DSP/ISP derivations, plots, histogram experiments,
synthetic signal generation, and teaching walkthroughs. Notebooks belong under
`learning/experiments/`, remain local-only, and use Chinese Markdown
explanations. Scaffold them with the skill helper instead of hand-editing
notebook JSON, keep cells small and deterministic, clear large outputs, and run
the notebook top-to-bottom before calling it complete. Production algorithms
must still be implemented and tested in C++; a notebook is evidence and a
learning tool, not the product implementation.

DSP/ISP 推导、曲线绘制、直方图实验、合成信号生成和教学式演示应使用该 skill。Notebook 放在
`learning/experiments/` 下，仅本地保留，Markdown 说明全部使用中文。使用 skill 自带脚手架，
不要手写 notebook JSON；单元格保持小而确定，清理大型输出，并在完成前从头到尾运行。正式算法
仍必须在 C++ 中实现和测试；notebook 是实验依据和学习工具，不是产品实现。

### Optional Security Skills / 可选安全 Skill

- Use `differential-review` for security-sensitive diffs involving untrusted
  file parsing, unsafe memory, dependency updates, public APIs, or validation
  removal. It is not required for greenfield documentation or routine numeric
  algorithms.
- Use `c-review` after a meaningful C/C++ surface exists and before treating
  untrusted RAW/file input as robust. Invoke it for focused security audits of
  memory ownership, bounds, integer arithmetic, races, and platform behavior.
  Agree on scope, threat model, worker model, and severity filter first. Keep
  `.c-review-results/` local and treat partial worker coverage as an incomplete
  audit.
- Use `semgrep` only for an explicitly requested static/security audit and only
  after presenting its exact scan plan for approval. Always disable telemetry
  with `--metrics=off`. Do not make it a routine per-commit requirement.

- 当 diff 涉及不可信文件解析、不安全内存、新依赖、公开 API 或移除校验时，使用
  `differential-review`。新建文档或普通数值算法不需要例行使用。
- 当项目已经形成有意义的 C/C++ 代码面，并准备把不可信 RAW/文件输入视为可靠支持对象时，使用
  `c-review`。重点审计内存所有权、边界、整数运算、并发和平台行为。执行前先确认范围、威胁模型、
  worker 模型和严重级别过滤器。`.c-review-results/` 仅本地保留；任何 worker 覆盖不完整都意味着
  审计尚未完成。
- 只有明确要求静态分析或安全审计时才使用 `semgrep`，并且必须先展示准确扫描计划并取得确认。
  所有扫描都要使用 `--metrics=off` 关闭遥测，不把它设为每次提交的固定步骤。

### GitHub Workflow Skills / GitHub 工作流 Skill

- Use `github:github` to inspect current remote repository, issue, or pull
  request context.
- Use `github:gh-fix-ci` when GitHub Actions checks fail.
- Use `github:gh-address-comments` when resolving pull request review threads.
- Use `github:yeet` only when the user explicitly asks to publish local changes
  through a branch, push, and draft pull request.

- 使用 `github:github` 检查远端仓库、issue 或 pull request 的当前上下文。
- GitHub Actions 检查失败时使用 `github:gh-fix-ci`。
- 处理 pull request 审查意见时使用 `github:gh-address-comments`。
- 只有用户明确要求通过分支、push 和 draft pull request 发布本地修改时，才使用
  `github:yeet`。

### Skills Not on the Current Critical Path / 当前主线不使用的 Skill

Do not invoke React, Vercel, web-design, browser-testing, image-generation, or
pet/asset skills while the project remains a C++ CLI/library and algorithm
learning project. Re-evaluate them only if a web UI, hosted service, or visual
asset deliverable enters an approved specification. Use document skills only
when editing or generating `.docx`/PDF artifacts.

项目仍是 C++ CLI/库和算法学习项目时，不调用 React、Vercel、Web 设计、浏览器测试、图像生成或
宠物/素材类 skill。只有经确认的规格加入 Web UI、托管服务或视觉资产交付物后才重新评估。仅在
编辑或生成 `.docx`/PDF 文档时使用文档类 skill。

Whenever skills are installed, removed, or upgraded, re-check this routing
section. Do not make an unavailable skill mandatory, and do not let a newly
installed relevant skill remain undocumented.

每次安装、移除或升级 skill 后，都要重新检查本节。不要把不可用的 skill 设为必用项，也不要让新安装
且与项目相关的 skill 长期处于未记录状态。

## 5. ISP Learning and Implementation Loop / ISP 学习与实现闭环

Every new processing stage should follow this order:

每个新处理阶段都按以下顺序推进：

1. Study the physical or mathematical principle and write Chinese notes under
   `learning/`.
2. Write or update the public bilingual design specification.
3. Create a scalar, single-threaded reference implementation with explicit
   metadata and numeric ranges.
4. Write deterministic numeric tests and, when valuable, property tests.
5. Run the stage on synthetic patterns and a legally usable real sample.
6. Compare numeric outputs, histograms, and before/after images; record the
   observed failure modes.
7. Establish a fixed-input benchmark before optimizing.
8. Optimize only after correctness is stable, and verify optimized output
   against the reference implementation.

1. 学习物理或数学原理，并在 `learning/` 下用中文记录。
2. 编写或更新公开的中英双语设计规格。
3. 编写标量、单线程参考实现，显式记录元数据和数值范围。
4. 编写确定性数值测试，并在有价值时加入性质测试。
5. 使用合成图案和合法可用的真实样张运行该阶段。
6. 比较数值输出、直方图和前后图像，记录观察到的失效模式。
7. 优化前先建立固定输入的性能基线。
8. 只有正确性稳定后才优化，并将优化结果与参考实现对照验证。

Do not merge an algorithm that is supported only by a visually pleasing
example. Visual quality is necessary but is not numerical proof.

不要合并仅凭“看起来不错”的样例支撑的算法。视觉质量很重要，但它不能替代数值证明。

## 6. C++ and Dependency Rules / C++ 与依赖规则

- Use C++17 unless an approved specification changes the standard.
- Keep the core library independent from UI frameworks.
- Prefer explicit ownership and standard containers in the initial design.
- Make pixel format, color state, numeric range, and row layout explicit at API
  boundaries.
- Prefer clear scalar code before SIMD, OpenMP, or Metal implementations.
- Avoid hidden color-space conversion, implicit clipping, unchecked integer
  arithmetic, and unnecessary copies.
- Before adding a dependency, check whether the standard library or current
  stack is sufficient, then review maintenance status, license, platform
  support, binary/build cost, and known vulnerabilities.
- Consult official documentation or primary papers for algorithm and API
  claims. Record citations in public design documents where they affect a
  technical decision.

- 除非已确认的规格修改标准，否则使用 C++17。
- 核心库保持独立，不依赖 UI 框架。
- 初始设计优先使用显式所有权和标准容器。
- 在 API 边界显式表达像素格式、色彩状态、数值范围和行布局。
- 先写清晰的标量实现，再考虑 SIMD、OpenMP 或 Metal。
- 避免隐藏的色彩空间转换、隐式裁剪、未检查的整数运算和不必要拷贝。
- 添加依赖前先确认标准库或现有技术栈是否足够，再检查维护状态、许可证、平台支持、二进制/构建
  成本和已知漏洞。
- 算法和 API 结论应查阅官方文档或原始论文；如果资料影响技术决策，应在公开设计文档中记录引用。

## 7. Verification and Definition of Done / 验证与完成标准

Use the commands defined by the active stage specification. Once the CMake
baseline exists, the normal local verification sequence is:

使用当前阶段规格定义的命令。CMake 工程底座建立后，常规本地验证顺序为：

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

A task is complete only when all applicable items are true:

只有满足所有适用条件，任务才算完成：

- acceptance criteria are met and the active specification reflects decisions
- new behavior has tests; bug fixes have a regression test
- relevant targeted tests and the full available suite pass
- the project builds from the documented command
- numeric tolerances and visual checks are documented where applicable
- no performance claim is made without a repeatable measurement
- code review has no unresolved critical or required findings
- public documentation is bilingual and local learning notes remain Chinese
- `git status` contains no accidental private files, outputs, or unrelated edits
- the final report states exactly what was verified and what could not be run

- 验收条件已满足，当前规格已反映实际决策
- 新行为有测试，缺陷修复有回归测试
- 相关定向测试和当前可用的完整测试套件均通过
- 项目能够使用文档中的命令构建
- 适用时已记录数值容差和视觉检查
- 没有可复现测量就不作性能结论
- 代码审查不存在未解决的严重或必须修改问题
- 公开文档保持中英双语，本地学习笔记保持中文
- `git status` 中没有意外的私有文件、输出或无关修改
- 最终说明准确列出已验证内容和无法执行的检查

## 8. Git Safety / Git 安全

- Never discard or overwrite changes that predate the current task.
- Do not use destructive git commands without explicit user approval.
- Inspect the diff before staging. Keep generated files and private material
  out of commits.
- Use focused, imperative commit messages that describe the result and reason.
- Do not push, create releases, change repository visibility, or rewrite
  history unless the user explicitly requests it.

- 不得丢弃或覆盖当前任务开始前已经存在的修改。
- 未经用户明确同意，不使用破坏性 git 命令。
- 暂存前检查 diff，确保生成文件和私有材料不进入 commit。
- commit 信息应聚焦、使用祈使语气，并说明结果和原因。
- 除非用户明确要求，否则不 push、不创建 release、不修改仓库可见性，也不改写历史。
