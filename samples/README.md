# Samples

This folder documents the sample-data policy for the project.

RAW photos and generated outputs are local-only by default. Put them in:

```text
samples/raw/
samples/output/
```

Those folders are ignored by git.

Public sample files should only be added when they are small, useful for tests,
and safe to redistribute. For early testing, prefer synthetic buffers in unit
tests over committing real camera files.

Recommended private test set:

- one daylight scene with broad color
- one high-contrast scene
- one dark/low-light scene
- one image with skin tones
- one image with fine detail for demosaicing artifacts

