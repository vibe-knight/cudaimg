---
layout: default
title: Documentation
description: Mini-OpenCV Documentation - Comprehensive guides and API reference for the GPU image processing library.
---

# Mini-OpenCV Documentation

Welcome to the Mini-OpenCV documentation. This GPU-accelerated image processing library provides high-performance CUDA operators covering pixel operations, convolution, morphology, geometric transforms, and more.

## Quick Links

| Getting Started | User Guides | API Reference | Examples |
|----------------|-------------|---------------|----------|
| [Quick Start](quickstart) | [Installation](installation) | [API Overview](api/) | [Basic Usage](examples/basic-usage) |
| [Architecture](architecture) | [Performance Guide](performance) | [ImageProcessor](api/image_processor) | [Pipeline Processing](examples/pipeline-processing) |
| [FAQ](faq) | - | [PipelineProcessor](api/pipeline_processor) | - |

## Documentation Structure

```
docs/
├── README.md              # This file - documentation index
├── quickstart.md          # 5-minute quick start guide
├── installation.md        # Detailed installation instructions
├── architecture.md        # Architecture and design overview
├── performance.md         # Performance optimization guide
├── faq.md                 # Frequently asked questions
├── api/                   # API Reference (English)
│   ├── README.md
│   ├── image_processor.md
│   ├── pipeline_processor.md
│   └── ...
├── api.zh-CN/             # API Reference (中文)
└── examples/              # Extended examples
```

## Language Selection

- **English** (this page)
- [简体中文 (Chinese)](README.zh-CN)

## Getting Started

New to Mini-OpenCV? Start with the [Quick Start Guide](quickstart) to get up and running in 5 minutes.

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage image = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);
```

## What's Included

### Image Processing Operators

| Category | Operators |
|----------|-----------|
| **Pixel Operations** | Invert, grayscale, brightness adjustment |
| **Convolution** | Gaussian blur, Sobel edge detection, custom kernels |
| **Histogram** | Calculation, equalization |
| **Scaling** | Bilinear, nearest-neighbor interpolation |
| **Morphology** | Erosion, dilation, opening, closing, gradient |
| **Threshold** | Global, adaptive, Otsu auto |
| **Color Space** | RGB/HSV/YUV conversion, channel operations |
| **Geometric** | Rotate, flip, affine, perspective, crop |
| **Filters** | Median, bilateral, box, sharpen, Laplacian |
| **Arithmetic** | Add, subtract, multiply, blend, weighted sum |
| **Pipeline** | Multi-step async processing with CUDA streams |

## Requirements

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 compatible compiler
- NVIDIA GPU (Compute Capability 7.5+)

See [Installation Guide](installation) for detailed system requirements.

## Contributing

We welcome contributions! Please see our [Contributing Guide](../CONTRIBUTING) for guidelines on how to contribute to this project.

## Support

- **Issues**: [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- **Discussions**: [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)
- **Documentation**: [GitHub Pages](https://lessup.github.io/mini-opencv/)

## License

MIT License - see [LICENSE](../LICENSE) file for details.

---

*Last updated: 2026-04-16*
