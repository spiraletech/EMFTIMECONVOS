# AETHER OS — An App for the Future

AETHER OS is SpiralOS's **mystery-driven engineering and local AI evolution workstation**. It is intended to grow into a native Windows C++20 application with signal analysis, research, sacred geometry, GUFF, automated testing, and careful provenance. It does **not** represent supernatural or extraterrestrial communications as verified.

## Current state: v0.1 alpha core

This first commit implements a **working command-line core**, not the planned graphical 12-workspace mainframe.

- Deterministic Flower-of-Life-style hexagonal circle generator exported as SVG.
- Bounded sine-wave generation exported as PCM WAV (offline file only; no device output).
- Atomic operator-resettable HARDSTOP used by oscillator generation.
- Evidence type labels and a GUFF prompt builder, **not local model inference**.
- CMake build and CTest unit assertions.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Try it

```sh
aether status
aether geometry flower.svg 2
aether tone tone528.wav 528
aether prompt "Investigate PRISM's FFT implementation"
```

On Windows invoke `aether.exe` from the generated build configuration directory.

## Anti-slop development law

**SPECIFIED != IMPLEMENTED != VERIFIED.** No fabricated AI responses or sensor measurements. No ghost or alien communication claimed by generated sound. No model is connected. No audio device capture. No autonomous repo modifications. Code and third-party research are untrusted input. Safety permission controls must remain outside the inference model.

Next vertical slice: add standalone desktop shell, WASAPI loopback/mic measurement, tested DSP, evidence persistence and GUFF's pinned local inference adapter. See `docs/repository_registry.json` for the 21-repository discovery inventory.
