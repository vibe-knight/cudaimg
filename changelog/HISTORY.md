# Development History

Historical archive of project development milestones and release history.

## Overview

| Version | Date | Focus | Highlights |
|---------|------|-------|------------|
| [v2.0.0](#v200) | 2026-03-24 | Correctness & Hardening | Bug fixes, CI improvement, parameter validation |
| [v1.3.0](#v130) | 2026-03-10 | Documentation | GitHub Pages optimization, professional landing page |
| [v1.2.0](#v120) | 2026-03-10 | CI/CD | Workflow deep standardization |
| [v1.1.0](#v110) | 2026-03-09 | Infrastructure | Initial CI workflow setup |
| [v1.0.0](#v100) | 2025-02-13 | Inception | Initial release with core GPU operators |

---

## v2.0.0

**Released:** 2026-03-24  
**Focus:** Deterministic correctness fixes with minimal changes

### 🐛 Bug Fixes

| Issue | Description |
|-------|-------------|
| Geometric Transform | Fixed bilinear sampling boundary coordinate zeroing issue |
| Convolution Engine | Removed global shared constant kernel dependency, preventing data pollution in multi-stream scenarios |
| Parameter Validation | Added sigma parameter validation for Gaussian blur and bilateral filter |
| Test Convergence | Converged Otsu/scaling round-trip test assumptions, reduced HSV/YUV quantization error |

### ✨ New Features

- Geometric and convolution regression tests
- Exception tests for invalid parameters
- Repository-level `.clang-format` configuration

### 🔧 Changed

- CUDA language standard: C++14 compatible
- Removed C++17 features blocking older NVCC versions
- Updated test coverage descriptions in documentation

### Impact

- More reliable geometric transforms
- Safer multi-stream convolution
- Better cross-compiler compatibility

---

## v1.3.0

**Released:** 2026-03-10  
**Focus:** GitHub Pages comprehensive optimization

### ✨ New Features

| Feature | Description |
|---------|-------------|
| Landing Page | Professional `index.md` with architecture diagram |
| Feature Matrix | Complete operator and optimization technique documentation |
| GPU Support Table | Architecture support from Turing to Hopper |
| Badges | Complete set (CI/Pages/License/CUDA/C++/CMake) |
| SEO | Optimized `_config.yml` with multi-line descriptions |

### 🔧 Changed

- Extended README.md with architecture and feature tables
- Synchronized README.zh-CN.md content
- Added Jekyll exclude list for faster builds

### 🐛 Fixed

- Badge layout issues in Chinese README

---

## v1.2.0

**Released:** 2026-03-10  
**Focus:** GitHub Actions deep standardization

### Standardized

| Aspect | Change |
|--------|--------|
| Permissions | Unified `contents: read` across workflows |
| Concurrency | Consistent configuration |
| Pages | Added `actions/configure-pages@v5` |
| Triggers | Path filters to reduce unnecessary builds |

### Technical Improvements

- Standardized job naming conventions
- Consistent checkout depth (`fetch-depth: 1`)
- Aligned timeout configurations
- Unified artifact handling

---

## v1.1.0

**Released:** 2026-03-09  
**Focus:** Initial CI/CD infrastructure

### ✨ New Features

- Standardized CI workflow (`.github/workflows/ci.yml`)
- Triggers: `push`, `pull_request`, `workflow_dispatch`
- CUDA container-based build validation
- `clang-format` check job

### Technical Details

- CI runs in `nvidia/cuda:12.4.1-devel-ubuntu22.04`
- Automatic build and test execution
- Parallel job execution (build + format check)

---

## v1.0.0

**Released:** 2025-02-13  
**Focus:** Initial library release

### ✨ Core Features

| Category | Features |
|----------|----------|
| **Memory** | `DeviceBuffer`, `MemoryManager`, `StreamManager` |
| **Processing** | `ImageProcessor`, `PipelineProcessor` |
| **Pixel Ops** | Invert, grayscale, brightness |
| **Convolution** | Gaussian blur, Sobel edge, custom kernels |
| **Histogram** | Calculation, equalization |
| **Scaling** | Bilinear, nearest-neighbor |
| **Morphology** | Erosion, dilation, open/close, gradient |
| **Threshold** | Global, adaptive, Otsu |
| **Color Space** | RGB/HSV/YUV conversion |
| **Geometric** | Rotate, flip, affine, perspective |
| **Filters** | Median, bilateral, box, sharpen |
| **Arithmetic** | Add, subtract, multiply, blend |
| **Pipeline** | Multi-step chaining, batch async |

### 📦 Deliverables

- Complete CMake build system with CUDA support
- Unit test suite (Google Test v1.14.0)
- Example programs (`basic_example.cpp`, `pipeline_example.cpp`)
- MIT License
- `.editorconfig` for code formatting

---

## Detailed Logs

For detailed technical logs, see individual files:

| File | Version | Focus |
|------|---------|-------|
| [2026-03-24_correctness-and-ci-hardening.md](2026-03-24_correctness-and-ci-hardening.md) | v2.0.0 | Correctness fixes |
| [2026-03-10_pages-optimization.md](2026-03-10_pages-optimization.md) | v1.3.0 | Documentation |
| [2026-03-10_workflow-deep-standardization.md](2026-03-10_workflow-deep-standardization.md) | v1.2.0 | CI/CD |
| [2026-03-09_workflow-optimization.md](2026-03-09_workflow-optimization.md) | v1.1.0 | Infrastructure |
| [2025-02-13_project-infrastructure.md](2025-02-13_project-infrastructure.md) | v1.0.0 | Inception |

---

## Versioning

This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

| Version Component | Description |
|-------------------|-------------|
| MAJOR | Incompatible API changes |
| MINOR | Backward-compatible functionality |
| PATCH | Backward-compatible bug fixes |

---

*Last updated: 2026-04-16*
