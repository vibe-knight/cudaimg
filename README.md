# Mini-OpenCV — GPU Image Processing Library

[![CI](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/badge/Docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/mini-opencv/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

English | [简体中文](README.zh-CN.md)

A high-performance CUDA-based image processing library — a mini OpenCV with GPU-accelerated operators covering pixel operations, convolution, morphology, geometric transforms, filtering, color space conversion, and async pipeline processing.

> **⚡ Performance**: 30-50x faster than CPU OpenCV for comparable operations

## 📚 Documentation

| Resource | Description |
|----------|-------------|
| [Quick Start](docs/quickstart.md) | Get started in 5 minutes |
| [Installation](docs/installation.md) | Complete setup guide |
| [Architecture](docs/architecture.md) | Design and internals |
| [Performance](docs/performance.md) | Optimization guide |
| [API Reference](docs/api/) | Complete API docs |
| [FAQ](docs/faq.md) | Common questions |

**Full Documentation**: https://lessup.github.io/mini-opencv/

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
│            ImageProcessor  ·  PipelineProcessor              │
├─────────────────────────────────────────────────────────────┤
│                  Operator Layer (CUDA Kernels)               │
│  PixelOperator │ ConvolutionEngine │ HistogramCalculator    │
│  ImageResizer  │ Morphology        │ Threshold              │
│  ColorSpace    │ Geometric         │ Filters                │
│  ImageArithmetic                                            │
├─────────────────────────────────────────────────────────────┤
│                    Infrastructure Layer                      │
│  DeviceBuffer · MemoryManager · StreamManager · CudaError   │
│  GpuImage · HostImage · ImageIO (stb)                       │
└─────────────────────────────────────────────────────────────┘
```

## Features

| Category | Operators | Highlights |
|----------|-----------|------------|
| **Pixel Ops** | Invert, grayscale, brightness | Per-pixel parallel |
| **Convolution** | Gaussian blur, Sobel edge detection, custom kernels | Shared memory tiling |
| **Histogram** | Calculation, equalization | Atomic ops + parallel reduction |
| **Scaling** | Bilinear, nearest-neighbor | Arbitrary size |
| **Morphology** | Erosion, dilation, open/close, gradient, top/black-hat | Custom structuring elements |
| **Threshold** | Global, adaptive, Otsu auto | Histogram-driven |
| **Color Space** | RGB/HSV/YUV conversion, channel split/merge | Batch conversion |
| **Geometric** | Rotate, flip, affine, perspective, crop, pad | Bilinear interpolation |
| **Filters** | Median, bilateral, box, sharpen, Laplacian | Edge-preserving |
| **Arithmetic** | Add, subtract, multiply, blend, weighted sum, abs diff | Scalar & image |
| **Pipeline** | Multi-step chaining, batch async processing | Multi-stream concurrency |

## Requirements

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 compatible compiler
- NVIDIA GPU (Compute Capability 7.5+)

## Quick Start

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

# Run example
./bin/basic_example
```

## Usage

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpuImage = processor.loadFromHost(hostImage);

GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
GpuImage edges   = processor.sobelEdgeDetection(gpuImage);
GpuImage gray    = processor.toGrayscale(gpuImage);

HostImage result = processor.downloadImage(blurred);
```

### Pipeline Processing

```cpp
PipelineProcessor pipeline(4);  // 4 CUDA streams
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
    img = std::move(temp);
});
std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
```

## GPU Architecture Support

| Architecture | Compute Capability | Examples |
|-------------|-------------------|----------|
| Turing | SM 75 | RTX 20xx / T4 |
| Ampere | SM 80 / 86 | A100 / RTX 30xx |
| Ada Lovelace | SM 89 | RTX 40xx / L4 |
| Hopper | SM 90 | H100 |

## Project Structure

```
mini-opencv/
├── docs/               # Documentation (English & Chinese)
├── include/gpu_image/  # Public headers (19 modules)
├── src/                # CUDA/C++ source files
├── tests/              # Unit tests
├── examples/           # Example programs
├── benchmarks/         # Performance benchmarks
└── CMakeLists.txt      # Build system
```

## Engineering Quality

- **Modern CMake** — Target-based compile options with generator expressions
- **FetchContent dependencies** — GTest v1.14.0, Google Benchmark v1.8.3, stb
- **Auto GPU arch detection** — CMake 3.24+ uses `native`
- **Install support** — `gpu_image::gpu_image_processing` CMake export target
- **CI pipeline** — GitHub Actions with CUDA build + clang-format check
- **Test suite** — 12 test files covering main operator modules

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

## License

MIT License — see [LICENSE](LICENSE) file.

---

**⭐ Star this repo if you find it helpful!**

For support, open an [issue](https://github.com/LessUp/mini-opencv/issues) or start a [discussion](https://github.com/LessUp/mini-opencv/discussions).
