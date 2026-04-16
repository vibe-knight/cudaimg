# Release v1.0.0 — Project Inception

**Release Date:** 2025-02-13  
**Focus:** Initial project foundation with core library

---

## Overview

Initial release establishing the project foundation with core GPU image processing library, build system, and basic tooling.

---

## ✨ Core Library

### Memory Management
- `DeviceBuffer` — RAII GPU memory
- `MemoryManager` — Allocation tracking
- `StreamManager` — CUDA stream lifecycle

### Processing API
- `ImageProcessor` — Synchronous operations
- `PipelineProcessor` — Asynchronous batch processing

### Operators

| Category | Operations |
|----------|------------|
| Pixel | Invert, grayscale, brightness |
| Convolution | Gaussian blur, Sobel edge, custom kernels |
| Histogram | Calculation, equalization |
| Scaling | Bilinear, nearest-neighbor |
| Morphology | Erosion, dilation, open/close |
| Threshold | Global, adaptive, Otsu |
| Color Space | RGB/HSV/YUV conversion |
| Geometric | Rotate, flip, affine, perspective |
| Filters | Median, bilateral, box, sharpen |
| Arithmetic | Add, subtract, multiply, blend |

---

## 📦 Build System

- CMake-based with CUDA support
- Google Test v1.14.0 integration
- Example programs

---

## 📄 Project Files

- MIT License
- `.editorconfig` for formatting

---

## Resources

- [CHANGELOG.md v1.0.0](../CHANGELOG.md)
