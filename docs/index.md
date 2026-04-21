---
layout: default
title: Home
nav_order: 1
description: Mini-OpenCV — High-performance CUDA-based image processing library with GPU-accelerated operators
---

# Mini-OpenCV
{: .fs-9 .fw-700 }

A **high-performance CUDA-based image processing library** delivering 30-50x speedup over CPU-based solutions. Production-ready GPU-accelerated operators for real-time video processing, batch image operations, and computer vision applications.
{: .fs-6 .fw-300 }

[Get Started](#getting-started){: .btn .btn-primary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[View on GitHub](https://github.com/LessUp/mini-opencv){: .btn .btn-secondary .fs-5 .mb-4 .mb-md-0 }
[API Reference](api/){: .btn .btn-outline .fs-5 .mb-4 .mb-md-0 }

---

## ⚡ Performance

> **30-50x faster** than CPU OpenCV for comparable operations

| Operation | CPU (OpenCV) | GPU (Mini-OpenCV) | Speedup |
|:----------|:-------------|:------------------|:--------|
| Gaussian Blur (1024x1024) | ~15ms | ~0.4ms | **37x** |
| Sobel Edge Detection | ~12ms | ~0.3ms | **40x** |
| Histogram Equalization | ~8ms | ~0.2ms | **40x** |
| Bilateral Filter | ~150ms | ~4ms | **37x** |

*Benchmarks on RTX 4090 vs Intel i9-13900K*

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│         ImageProcessor  ·  PipelineProcessor                 │
├─────────────────────────────────────────────────────────────┤
│              Operator Layer (CUDA Kernels)                   │
│  PixelOperator  │  ConvolutionEngine  │  Geometric          │
│  Morphology     │  ColorSpace         │  Filters            │
│  Threshold      │  HistogramCalculator│  ImageResizer       │
├─────────────────────────────────────────────────────────────┤
│                  Infrastructure Layer                        │
│  DeviceBuffer  ·  GpuImage/HostImage  ·  CudaError          │
│  ImageIO       ·  StreamManager                              │
└─────────────────────────────────────────────────────────────┘
```

**Three-layer design** separates concerns between user APIs, operator implementations, and infrastructure services for maximum flexibility and performance.

---

## ✨ Features

### Core Operators

| Category | Operators | Highlights |
|:---------|:----------|:-----------|
| **Pixel Operations** | Invert, grayscale, brightness | Per-pixel parallel |
| **Convolution** | Gaussian blur, Sobel edge, custom kernels | Shared memory tiling |
| **Histogram** | Calculation, equalization | Atomic ops + parallel reduction |
| **Scaling** | Bilinear, nearest-neighbor | Arbitrary size support |
| **Morphology** | Erosion, dilation, open/close/gradient | Custom structuring elements |
| **Threshold** | Global, adaptive, Otsu auto | Histogram-driven |
| **Color Space** | RGB/HSV/YUV conversion | Batch conversion |
| **Geometric** | Rotate, flip, affine, perspective | Bilinear interpolation |
| **Filters** | Median, bilateral, box, sharpen | Edge-preserving |
| **Arithmetic** | Add, subtract, multiply, blend | Scalar & image operations |
| **Pipeline** | Multi-step chaining, batch async | Multi-stream concurrency |

### GPU Architecture Support

| Architecture | Compute | Examples |
|:-------------|:--------|:---------|
| Turing | SM 75 | RTX 20 series, T4 |
| Ampere | SM 80/86 | A100, RTX 30 series |
| Ada Lovelace | SM 89 | RTX 40 series, L4 |
| Hopper | SM 90 | H100 |

---

## 🚀 Getting Started

### Requirements

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 compatible compiler
- NVIDIA GPU (Compute Capability 7.5+)

### Quick Build

```bash
# Clone
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Build
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j$(nproc)

# Test
ctest --output-on-failure
```

### Usage Example

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);

// Process
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);

// Download
HostImage result = processor.downloadImage(edges);
```

---

## 📚 Documentation

Explore our comprehensive documentation:

### Guides
- [Quick Start](docs/quickstart) — Get started in 5 minutes
- [Installation](docs/installation) — Complete setup guide
- [Architecture](docs/architecture) — Design and internals
- [Performance](docs/performance) — Optimization guide

### Reference
- [API Documentation](docs/api/) — Complete API reference
- [Examples](docs/examples/) — Code examples and tutorials
- [FAQ](docs/faq) — Frequently asked questions
- [Changelog](CHANGELOG) — Version history

---

## 🛠️ Engineering Quality

| Feature | Description |
|:--------|:------------|
| **Modern CMake** | Target-based with generator expressions |
| **FetchContent** | Dependencies: GTest, Google Benchmark, stb |
| **Auto GPU Detection** | CMake 3.24+ automatic architecture detection |
| **Install Support** | CMake export target for easy integration |
| **CI/CD** | GitHub Actions: CUDA build + clang-format |
| **Test Suite** | Comprehensive tests covering all operators |
| **Cross-Platform** | GCC/Clang + MSVC support |
| **Documentation** | Bilingual (English/Chinese) with Just the Docs |

---

## 📊 Project Stats

[![GitHub stars](https://img.shields.io/github/stars/LessUp/mini-opencv?style=social)](https://github.com/LessUp/mini-opencv/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/LessUp/mini-opencv?style=social)](https://github.com/LessUp/mini-opencv/network/members)
[![GitHub issues](https://img.shields.io/github/issues/LessUp/mini-opencv)](https://github.com/LessUp/mini-opencv/issues)
[![GitHub license](https://img.shields.io/github/license/LessUp/mini-opencv)](LICENSE)

---

## 🤝 Contributing

We welcome contributions! See our [Contributing Guide](CONTRIBUTING) for:
- How to report bugs
- Feature request guidelines
- Pull request process
- Code style requirements

---

## 📄 License

This project is licensed under the **MIT License** — see the [LICENSE](https://github.com/LessUp/mini-opencv/blob/main/LICENSE) file for details.

---

## 🔗 Quick Links

- [GitHub Repository](https://github.com/LessUp/mini-opencv)
- [Issue Tracker](https://github.com/LessUp/mini-opencv/issues)
- [Releases](https://github.com/LessUp/mini-opencv/releases)
- [Discussions](https://github.com/LessUp/mini-opencv/discussions)

---

**Made with ❤️ by LessUp**
