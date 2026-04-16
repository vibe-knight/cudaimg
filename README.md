# Mini-OpenCV — GPU Image Processing Library

[![CI](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/badge/Docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/mini-opencv/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

[English](README.md) | [简体中文](README.zh-CN.md)

A high-performance CUDA-based image processing library providing GPU-accelerated operators for computer vision applications.

> **⚡ Performance**: 30-50x faster than CPU OpenCV for comparable operations

---

## 📚 Quick Links

| Resource | Description |
|----------|-------------|
| [Installation](docs/installation.md) | Complete setup guide |
| [Quick Start](docs/quickstart.md) | Get started in 5 minutes |
| [API Reference](docs/api/) | Complete API documentation |
| [Examples](docs/examples/) | Code examples and tutorials |
| [FAQ](docs/faq.md) | Frequently asked questions |

**Full Documentation:** https://lessup.github.io/mini-opencv/

---

## ✨ Features

| Category | Operators | Highlights |
|----------|-----------|------------|
| **Pixel Operations** | Invert, grayscale, brightness | Per-pixel parallel |
| **Convolution** | Gaussian blur, Sobel edge detection | Shared memory tiling |
| **Histogram** | Calculation, equalization | Atomic ops + reduction |
| **Geometric** | Rotate, resize, flip, affine | Bilinear interpolation |
| **Morphology** | Erosion, dilation, open/close | Custom structuring elements |
| **Threshold** | Global, adaptive, Otsu | Histogram-driven |
| **Color Space** | RGB/HSV/YUV conversion | Batch conversion |
| **Filters** | Median, bilateral, sharpen | Edge-preserving |
| **Pipeline** | Multi-step async processing | Multi-stream concurrency |

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
│  ImageIO       ·  StreamManager                                │
└─────────────────────────────────────────────────────────────┘
```

---

## 🚀 Quick Start

```bash
# Clone and build
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure

# Run example
./build/bin/basic_example
```

### Basic Usage

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// Load and process
GpuImage gpu = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);
HostImage result = processor.downloadImage(edges);
```

---

## 📋 Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ | C++17 | C++17 |
| GPU | CC 7.5+ (Turing) | RTX 30/40 series |

---

## 📖 Documentation

Complete documentation available at [GitHub Pages](https://lessup.github.io/mini-opencv/):

- [Installation Guide](docs/installation.md)
- [Quick Start](docs/quickstart.md)
- [Architecture Overview](docs/architecture.md)
- [Performance Optimization](docs/performance.md)
- [API Reference](docs/api/)
- [FAQ](docs/faq.md)

---

## 🤝 Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

## 📝 Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

---

## 📄 License

MIT License — see [LICENSE](LICENSE) file.

---

**⭐ Star this repo if you find it helpful!**

For support, open an [issue](https://github.com/LessUp/mini-opencv/issues) or start a [discussion](https://github.com/LessUp/mini-opencv/discussions).
