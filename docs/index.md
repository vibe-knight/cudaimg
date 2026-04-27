---
layout: default
title: Home
nav_order: 1
description: GPU Image Processing — High-performance CUDA-based image processing library with 30-50x speedup over CPU
---

# GPU Image Processing
{: .fs-9 .fw-700 }

CUDA-accelerated computer vision library. **30-50x faster** than CPU OpenCV.
{: .fs-6 .fw-300 }

[Get Started]({{ site.baseurl }}/setup/quickstart){: .btn .btn-primary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[View on GitHub](https://github.com/LessUp/mini-opencv){: .btn .btn-secondary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[API Reference]({{ site.baseurl }}/api/){: .btn .btn-outline .fs-5 .mb-4 .mb-md-0 }

---

## Why GPU Image Processing?

Traditional CPU-based image processing struggles with real-time applications. This library leverages **NVIDIA CUDA** to run image operations directly on the GPU, delivering massive performance gains.

| Metric | CPU (OpenCV) | GPU (This Library) | Speedup |
|:-------|:-------------|:-------------------|:--------|
| Gaussian Blur (1024×1024) | ~15 ms | ~0.4 ms | **37×** |
| Sobel Edge Detection | ~12 ms | ~0.3 ms | **40×** |
| Histogram Equalization | ~8 ms | ~0.2 ms | **40×** |
| Bilateral Filter | ~150 ms | ~4 ms | **37×** |

*Benchmarks on RTX 4090 vs Intel i9-13900K*

---

## Quick Start

```bash
# Clone and build
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure
```

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage result = processor.downloadImage(blurred);
```

---

## Features

### Core Operators

| Category | Operations |
|:---------|:-----------|
| **Pixel** | Invert, grayscale, brightness adjustment |
| **Convolution** | Gaussian blur, Sobel edge detection, custom kernels |
| **Filters** | Median, bilateral, box, sharpen |
| **Geometric** | Resize, rotate, flip, affine, perspective transforms |
| **Morphology** | Erosion, dilation, open/close/gradient |
| **Color Space** | RGB/HSV/YUV conversion |
| **Threshold** | Global, adaptive, Otsu auto-threshold |
| **Histogram** | Calculation and equalization |

### GPU Support

| Architecture | Compute | Examples |
|:-------------|:--------|:---------|
| Turing | SM 75 | RTX 20 series, T4 |
| Ampere | SM 80/86 | A100, RTX 30 series |
| Ada Lovelace | SM 89 | RTX 40 series, L4 |
| Hopper | SM 90 | H100 |

---

## Architecture

> **See [Architecture Specification]({{ site.baseurl }}/architecture/architecture) for complete design details.**

Three-layer design: **High-level APIs** → **CUDA Kernels** → **Infrastructure**

```
Application Layer → Operator Layer → Infrastructure Layer
```

---

## Documentation

| Guide | Description |
|:------|:------------|
| [Quick Start]({{ site.baseurl }}/setup/quickstart) | Build and run in 5 minutes |
| [Installation]({{ site.baseurl }}/setup/installation) | Complete setup guide |
| [API Reference]({{ site.baseurl }}/api/) | Complete API documentation |
| [Architecture]({{ site.baseurl }}/architecture/architecture) | Design and internals |
| [Performance]({{ site.baseurl }}/tutorials/performance) | Optimization guide |
| [Examples]({{ site.baseurl }}/tutorials/examples/) | Code examples |
| [FAQ]({{ site.baseurl }}/tutorials/faq) | Common questions |

---

## Requirements

| Component | Minimum | Recommended |
|:----------|:--------|:------------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ | C++17 | C++17 |
| GPU | Compute 7.5+ | RTX 30/40 series |

---

## Contributing

Contributions welcome! See [Contributing Guide](https://github.com/LessUp/mini-opencv/blob/main/CONTRIBUTING.md).

---

## License

[MIT License](https://github.com/LessUp/mini-opencv/blob/main/LICENSE) — Copyright 2025-2026 LessUp

---

**[English]({{ site.baseurl }}/)** · [简体中文]({{ site.baseurl }}/index.zh-CN)
