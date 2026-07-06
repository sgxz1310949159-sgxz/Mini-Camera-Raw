# Project Foundation

Date: 2026-07-06

## Positioning

Mini-Camera Raw is a learning-first engineering project. Its value comes from
making the RAW processing pipeline visible, explainable, and testable rather
than from matching every feature of a commercial RAW editor.

The core learning goal is to understand bottom-level image principles:

- RAW sensor data, Bayer/CFA layout, black level, white level, and dynamic range
- ISP pipeline order and why each stage exists
- linear-light image processing versus nonlinear display-referred processing
- color correction matrices, working color spaces, and gamma encoding
- tone curves, histograms, exposure values, and clipping
- later: local contrast, edge-preserving filters, multithreading, and GPU ideas

## Working Loop

Each feature should follow the same loop:

1. Learn the principle.
2. Write a short design note: input, output, formula, data range, edge cases.
3. Implement the smallest correct CPU version.
4. Validate it with numeric tests and visual checks.
5. Record what changed, what was learned, and what still looks wrong.

This loop keeps the project from becoming a pile of experiments. It also turns
the final repository into a portfolio artifact: code plus reasoning.

## Roadmap

### Stage 0: Engineering Baseline

Goal: create a durable project base before implementing image algorithms.

Outputs:

- public repository structure
- MVP boundary and acceptance criteria
- initial CMake/testing plan
- initial image data model
- local-only learning workspace

Learning focus:

- C++ project structure
- image memory layout
- bit depth and numeric ranges
- basic testing strategy

### Stage 1: RAW Ingestion and Basic ISP

Goal: turn a RAW file into a viewable RGB image.

Target pipeline:

```text
RAW file
-> LibRaw parse
-> Bayer/CFA buffer
-> black level correction
-> white balance
-> demosaicing
-> camera-to-working-space color transform
-> gamma/display transform
-> PNG/TIFF output
```

Learning focus:

- RAW versus JPEG
- Bayer pattern and demosaicing
- black/white level normalization
- white balance as channel gain
- color matrix conversion
- why most adjustments should happen in linear space

### Stage 2: Tone Engine and Temporary UI

Goal: add basic Camera Raw-style controls while keeping the core engine clean.

Priority controls:

- Exposure
- Contrast
- Blacks / Whites
- Temperature / Tint
- basic tone curve or gamma control

OpenCV highgui can be used as a temporary validation UI, but UI code must stay
outside the core engine.

Learning focus:

- EV exposure math
- S-curves and contrast
- histograms and clipping
- linear versus sRGB-space adjustment
- parameter range design

### Stage 3: Color and Local Light Control

Goal: explore higher-level color and local tone algorithms without losing the
project's explainable structure.

Candidate controls:

- Saturation
- Vibrance
- RGB <-> HSV/HSL conversion
- simple HSL color mixer
- simplified Shadows / Highlights

Learning focus:

- color model tradeoffs
- saturation versus vibrance
- luminance masks
- bilateral/guided filtering
- low-frequency illumination and high-frequency detail

### Stage 4: Performance and API Export

Goal: turn the correct CPU pipeline into a more serious engineering artifact.

Priority order:

1. stable single-threaded CPU version
2. benchmark tooling
3. OpenMP/multithreaded loops
4. memory-access optimization
5. dynamic library export
6. Metal compute as an optional advanced extension

Learning focus:

- cache-friendly pixel traversal
- memory bandwidth limits
- CPU versus GPU execution models
- C API design and memory ownership
- dynamic library packaging

## Acceptance Categories

Every stage should be checked with five categories:

- Functional: the feature runs end-to-end.
- Numeric: formulas match expected values within a documented tolerance.
- Visual: output has no obvious color cast, clipping, halo, or banding issue.
- Performance: timing and memory measurements are recorded for fixed inputs.
- Learning: the principle can be explained in the project notes.

