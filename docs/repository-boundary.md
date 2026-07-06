# Repository Boundary

This project is planned as an open-source repository, but not every local file
belongs on GitHub.

## Publish

These should be included in the public repository:

- source code under `src/`, `include/`, and `apps/`
- build files, CMake modules, and test configuration
- public project documentation under `docs/`
- unit tests and synthetic test fixtures
- benchmark source code
- small generated examples only when they are safe and useful for explanation

## Keep Local

These should not be committed:

- `learning/`: personal study notes, rough derivations, reading notes, mistakes,
  and private reflections
- `项目策划书.docx`: original personal planning document
- private RAW photos and camera files
- large generated outputs, benchmark results, and temporary renders
- compiled binaries and dynamic libraries
- dependency caches and local IDE state

The `.gitignore` file already reflects this policy.

## Conditional

Some files may be published later, but only after checking rights and privacy:

- sample RAW files, if they are self-shot and intentionally released or clearly
  licensed for redistribution
- before/after images, if they do not reveal private content
- learning summaries rewritten as polished technical docs
- benchmark reports, once they are reproducible and not just local scratch data

## GitHub Setup Status

Local git can be initialized in this folder.

Remote GitHub publication still needs one of these:

- a GitHub repository URL created by the user, or
- a working GitHub CLI / connector flow that can create repositories.

Current local status on 2026-07-06:

- `gh` CLI is not installed in the environment.
- The available GitHub connector can work with existing repositories, issues,
  pull requests, and files, but no create-repository tool is currently exposed.

Recommended remote repository:

```text
Mini-Camera-Raw
```

Recommended visibility:

```text
Public
```

Recommended first license:

```text
MIT
```

Do not add a `LICENSE` file until the author/copyright string is confirmed.

