# Stage 0 Engineering Spec

Status: Draft v0.1
Date: 2026-07-06

## Summary

Stage 0 creates the engineering base for Mini-Camera Raw. It should not try to
implement the full ISP pipeline yet. Its job is to make later algorithm work
organized, testable, and easy to explain.

Stage 0 is complete when the repository has a clear structure, a build/test
baseline, an initial image data model, and written rules for what belongs in
the public project versus local learning notes.

## Scope

In scope:

- initialize the local repository
- define the public project layout
- keep personal learning notes outside git
- choose initial C++ and CMake conventions
- define the first image-buffer concepts
- prepare a minimal test strategy
- document Stage 1 entry criteria

Out of scope:

- full RAW decoding
- real demosaicing
- OpenCV UI
- OpenMP optimization
- Metal compute
- dynamic library packaging

## Baseline Project Layout

```text
include/mini_camera_raw/   Public headers for the future core library
src/                       Core C++ implementation
apps/                      CLI tools and temporary validation apps
tests/                     Unit tests and small regression tests
benchmarks/                Performance experiments and benchmark source
cmake/                     Local CMake helper modules
docs/                      Public documentation and specs
samples/                   Sample policy and optional tiny fixtures
learning/                  Local-only study notes, ignored by git
```

## Initial Technical Decisions

- Language standard: C++17 for broad compiler and dependency compatibility.
- Build system: CMake.
- Namespace: `mini_camera_raw`.
- Core code must not depend on OpenCV UI types.
- CPU serial correctness comes before multithreading or GPU acceleration.
- Image-processing functions should document their expected color space, bit
  depth, and numeric range.
- Real camera RAW parsing will use LibRaw later, but Stage 0 can use synthetic
  buffers for tests.

## Data Model Direction

Stage 0 should define the minimum concepts needed before Stage 1:

- image width and height
- channel count
- pixel format, for example `uint16` source data or `float32` working data
- color state, for example Bayer, linear RGB, or display RGB
- row stride or a documented contiguous layout
- owned storage using standard C++ containers for the first version

The first implementation should prefer clarity:

- use owned buffers before designing external memory views
- avoid implicit conversion between linear and nonlinear spaces
- keep metadata explicit rather than hidden in comments

## Stage 0 Deliverables

- `README.md` with project purpose and directory map.
- `.gitignore` that excludes local learning notes, RAW files, build products,
  and the original planning document.
- public docs for project foundation and repository boundary.
- initial CMake skeleton.
- minimal placeholder library or executable that builds.
- a first unit-test target using synthetic data.
- an initial `ImageBuffer` or equivalent design note/header.

## Acceptance Criteria

Stage 0 is done when:

- the project builds from a clean checkout using CMake
- a basic test command runs successfully
- no local learning notes or RAW files appear in normal `git status`
- the repository has a clear public/private content boundary
- Stage 1 can start without redesigning the folder structure

Suggested future commands:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Learning Checklist

Before moving deeply into Stage 1, study and summarize:

- RAW versus JPEG
- Bayer/CFA sampling
- bit depth, black level, white level, and clipping
- image memory layout and row-major traversal
- linear light versus display-referred RGB
- CMake targets, include directories, and test targets

Personal notes for these topics should go under `learning/` first. Polished
summaries can later be moved into `docs/` if they become useful public material.

## Open Decisions

- Confirm GitHub repository owner and URL.
- Confirm license author string before adding `LICENSE`.
- Choose the first camera RAW format and sample image set.
- Choose the first test framework when writing code.
- Decide whether Stage 0 should include only a CLI placeholder or also an empty
  core library target.

