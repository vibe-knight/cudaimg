---
layout: default
title: API Reference
description: Complete API reference for Mini-OpenCV - all modules, classes, and functions.
---

# API Reference

Complete reference for all Mini-OpenCV modules.

## Modules Overview

### High-Level APIs

| Module | Description | Key Classes |
|--------|-------------|-------------|
| [ImageProcessor](image_processor) | Synchronous image processing | `ImageProcessor` |
| [PipelineProcessor](pipeline_processor) | Asynchronous batch processing | `PipelineProcessor` |

### Operator Modules

| Module | Description | Key Functions |
|--------|-------------|---------------|
| [PixelOperator](pixel_operator) | Per-pixel operations | `invert`, `toGrayscale`, `adjustBrightness` |
| [ConvolutionEngine](convolution_engine) | Convolution operations | `gaussianBlur`, `sobelEdgeDetection` |
| [Filters](filters) | Image filtering | `medianFilter`, `bilateralFilter` |
| [Geometric](geometric) | Geometric transformations | `rotate`, `resize`, `crop` |
| [Morphology](morphology) | Morphological operations | `erode`, `dilate`, `open`, `close` |
| [ColorSpace](color_space) | Color space conversions | `rgbToHsv`, `rgbToYuv` |
| [Threshold](threshold) | Thresholding operations | `threshold`, `adaptiveThreshold`, `otsuThreshold` |
| [HistogramCalculator](histogram_calculator) | Histogram operations | `calculate`, `equalize` |

### Infrastructure

| Module | Description | Key Classes |
|--------|-------------|-------------|
| [DeviceBuffer](device_buffer) | GPU memory management | `DeviceBuffer` |
| [GpuImage/HostImage](gpu_image) | Image containers | `GpuImage`, `HostImage` |
| [ImageIO](image_io) | File I/O | `ImageIO` |
| [CudaError](cuda_error) | Error handling | `CudaException` |

## Quick Access

### By Task

| Task | Module(s) |
|------|-----------|
| Load and save images | `ImageIO`, `ImageProcessor` |
| Apply blur | `ConvolutionEngine`, `Filters` |
| Detect edges | `ConvolutionEngine` |
| Resize image | `Geometric` |
| Convert colors | `ColorSpace`, `PixelOperator` |
| Remove noise | `Filters`, `Morphology` |
| Batch processing | `PipelineProcessor` |

### By Image Type

| Image Type | Recommended Operations |
|------------|----------------------|
| RGB/RGBA | All operations |
| Grayscale | Pixel, convolution, geometric |
| Binary | Morphology, threshold |

## Version Information

```cpp
#include "gpu_image/gpu_image_processing.hpp"

// Compile-time version
constexpr int major = GPU_IMAGE_VERSION_MAJOR;  // 2
constexpr int minor = GPU_IMAGE_VERSION_MINOR;  // 0
constexpr int patch = GPU_IMAGE_VERSION_PATCH;  // 0

// Runtime version
std::string version = gpu_image::getVersionString();  // "2.0.0"
```

## Common Patterns

### Basic Processing Pattern

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// Load
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);

// Process
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(blurred);

// Save
HostImage result = processor.downloadImage(edges);
ImageIO::save("output.jpg", result);
```

### Direct Operator Access

```cpp
#include "gpu_image/convolution_engine.hpp"

GpuImage src, dst;
// ... load src ...

ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f);
```

### Pipeline Pattern

```cpp
#include "gpu_image/pipeline_processor.hpp"

PipelineProcessor pipeline(4);  // 4 streams

pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
    img = std::move(temp);
});

std::vector<HostImage> results = pipeline.processBatchHost(inputs);
```

## Language Selection

- **English** (this page)
- [简体中文 (Chinese)](../api.zh-CN/)

## More Resources

- [Quick Start](../quickstart) - Get started in 5 minutes
- [Installation](../installation) - Setup guide
- [Architecture](../architecture) - Design overview
- [Performance](../performance) - Optimization guide
- [FAQ](../faq) - Common questions
