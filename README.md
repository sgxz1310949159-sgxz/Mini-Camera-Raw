# Mini-Camera Raw

Mini-Camera Raw is a summer personal project for learning image signal
processing, RAW development, and C++ engineering through a small, transparent
RAW-to-RGB processor.

The goal is not to clone the full Adobe Camera Raw feature set. The goal is to
build a clear pipeline that can read RAW image data, run basic ISP steps, expose
simple tone/color controls, and document the principles behind each stage.

## Project Focus

- Build a readable C++ core for RAW ingestion and ISP-style processing.
- Prioritize correctness, explainability, and testability before speed.
- Keep UI experiments separate from the core engine.
- Record technical decisions and learning notes as the project evolves.
- Treat GPU/Metal acceleration as an advanced extension after the CPU pipeline
  is stable.

## Public Repository Contents

This repository is intended to contain:

- source code for the core engine and small test applications
- CMake/build configuration
- public technical documentation and project specs
- unit tests, synthetic fixtures, and benchmark code
- sanitized examples that are safe and licensed to publish

Personal study notes, private RAW photos, generated outputs, and the original
planning document are kept local-only. See `docs/repository-boundary.md`.

## Current Status

Stage 0 planning has started. The immediate goal is to define the engineering
baseline: project structure, MVP scope, data model, build system, test approach,
and acceptance criteria.

## Directory Map

```text
include/mini_camera_raw/   Public C++ headers
src/                       Core implementation
apps/                      Small executables and temporary UI experiments
tests/                     Unit and regression tests
benchmarks/                Performance measurement tools
docs/                      Public project documentation
samples/                   Public sample policy and optional tiny fixtures
learning/                  Local-only study notes, ignored by git
```

