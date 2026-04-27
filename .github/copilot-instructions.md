# GitHub Copilot Instructions

This file provides project-specific instructions for GitHub Copilot when working in the mini-opencv repository.

## Project Overview

**Mini-OpenCV** is a CUDA-based high-performance image processing library. It provides GPU-accelerated operators for computer vision applications, delivering 30-50x faster performance than CPU OpenCV.

## Technology Stack

- **Language**: C++17, CUDA 14
- **Build**: CMake 3.18+
- **Testing**: Google Test v1.14.0
- **CUDA**: 11.0+ (Recommended: 12.x)
- **GPU**: Compute Capability 7.5+ (Turing or newer)

## Code Style

- **Indentation**: 2 spaces
- **Line width**: 80 characters
- **Naming**: 
  - Classes: `PascalCase` (e.g., `DeviceBuffer`)
  - Functions: `camelCase` (e.g., `copyFromHost`)
  - Files: `snake_case.hpp/.cpp/.cu`
- **Namespace**: `gpu_image`
- **Header guard**: `#pragma once`

## CUDA Kernel Rules

1. Always check boundaries: `if (x < width && y < height)`
2. Thread block size: 256 threads (16x16 for 2D)
3. Use `CUDA_CHECK(cudaGetLastError())` after kernel launch
4. Support `cudaStream_t stream = nullptr` parameter

## Testing

- Test files: `tests/<category>/test_<module>.cpp`
- Check CUDA availability in `SetUp()` with `GTEST_SKIP()`
- Use `EXPECT_THROW` for exception tests

## Build Commands

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

## Architecture

Three-layer architecture:
1. **Application**: `ImageProcessor`, `PipelineProcessor`
2. **Operator**: CUDA kernels (Pixel, Convolution, Histogram, etc.)
3. **Infrastructure**: `DeviceBuffer`, `GpuImage`, `CudaError`

## Key Files

- `AGENTS.md` - Full AI agent guidelines
- `openspec/specs/` - Specifications (SDD)
- `docs/` - User documentation (bilingual)
