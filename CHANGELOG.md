# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- Repository-specific AGENTS.md guide for coding agents

## [2.0.0] - 2026-03-24

### Fixed

- Geometric transform bilinear sampling boundary coordinate zeroing issue
- Removed convolution implementation dependency on global shared constant kernels, avoiding potential kernel data pollution under multi-stream
- Gaussian blur and bilateral filter sigma parameter validation with corresponding exception tests
- Converged Otsu/scaling round-trip test assumptions and reduced quantization error in HSV/YUV round-trip

### Added

- Geometric and convolution regression tests covering identity transform and concurrent convolution scenarios
- `ctest --output-on-failure` step in CI workflow
- Repository-level `.clang-format` file

### Changed

- CUDA language standard converged to C++14 compatible range
- Removed C++17 usages that could block older NVCC versions
- Updated README / README.zh-CN / index test coverage descriptions and fixed benchmark command in Chinese README

### Technical Notes

This release focuses on deterministic correctness fixes with minimal changes, complementing tests and engineering contracts to make the project more reliable in geometric transforms, concurrent convolution, parameter validation, and CI verification.

## [1.3.0] - 2026-03-10

### Added

- Professional landing page (`index.md`) with architecture diagram and feature matrix
- GPU architecture support table (Turing → Hopper)
- Technology stack table, project structure details, engineering quality highlights
- Complete badges (CI/Pages/License/CUDA/C++/CMake)
- Links section: repository, bilingual README, contributing guide, changelog

### Changed

- Optimized `_config.yml` description for better SEO
- Added exclude list to speed up Jekyll build
- Extended README.md with architecture diagram, feature table, build options
- Extended README.zh-CN.md with synchronized content

### Fixed

- Badge layout issues in README.zh-CN.md

### Technical Notes

GitHub Pages comprehensive optimization with professional documentation layout and SEO improvements.

## [1.2.0] - 2026-03-10

### Added

- CI workflow unified `permissions: contents: read` and `concurrency` configuration
- `actions/configure-pages@v5` step in Pages workflow
- `paths` trigger filter in Pages workflow to reduce unnecessary builds

### Changed

- Standardized naming, permissions, concurrency, path filtering and caching strategy across all GitHub Actions workflows

### Technical Notes

Second round of GitHub Actions deep standardization across the entire repository.

## [1.1.0] - 2026-03-09

### Added

- Standardized core GitHub Actions CI workflow (`.github/workflows/ci.yml`)
- Triggers for `push`, `pull_request`, and `workflow_dispatch`
- CUDA container-based build validation
- `clang-format` check job

### Technical Notes

Initial CI/CD infrastructure setup with automated build and format verification.

## [1.0.0] - 2025-02-13

### Added

- Core GPU image processing library with CUDA acceleration
- Memory management layer: `DeviceBuffer`, `MemoryManager`, `StreamManager`
- Processing layer: `PixelOperator`, `ConvolutionEngine`, `HistogramCalculator`, `ImageResizer`
- High-level API: `ImageProcessor`, `PipelineProcessor`
- Image operations:
  - Pixel: invert, grayscale, brightness adjustment
  - Convolution: Gaussian blur, Sobel edge detection, custom kernels
  - Histogram: calculation, equalization
  - Scaling: bilinear, nearest-neighbor interpolation
  - Morphology: erosion, dilation, open/close, gradient, top/black-hat
  - Threshold: global, adaptive, Otsu auto
  - Color space: RGB/HSV/YUV conversion, channel split/merge
  - Geometric: rotate, flip, affine/perspective transform, crop, pad
  - Filters: median, bilateral, box, sharpen, Laplacian
  - Arithmetic: add, subtract, multiply, blend, weighted sum, abs diff
  - Pipeline: multi-step chaining, batch async processing
- Unit test suite with Google Test v1.14.0
- Example programs: `basic_example.cpp`, `pipeline_example.cpp`
- MIT LICENSE file
- `.editorconfig` for unified code formatting
- Standardized badges in README (License, CUDA, C++, CMake)

### Technical Notes

Initial release of the mini-opencv GPU image processing library.

---

## Version History Summary

| Version | Date | Highlights |
|---------|------|------------|
| [2.0.0] | 2026-03-24 | Correctness fixes, CI hardening, parameter validation |
| [1.3.0] | 2026-03-10 | GitHub Pages optimization, professional landing page |
| [1.2.0] | 2026-03-10 | Workflow deep standardization |
| [1.1.0] | 2026-03-09 | Initial CI workflow setup |
| [1.0.0] | 2025-02-13 | Initial release with core GPU image processing features |

[Unreleased]: https://github.com/LessUp/mini-opencv/compare/v2.0.0...HEAD
[2.0.0]: https://github.com/LessUp/mini-opencv/compare/v1.3.0...v2.0.0
[1.3.0]: https://github.com/LessUp/mini-opencv/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/LessUp/mini-opencv/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/LessUp/mini-opencv/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/LessUp/mini-opencv/releases/tag/v1.0.0
