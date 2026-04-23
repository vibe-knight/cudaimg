---
layout: default
title: API Reference
nav_order: 4
has_children: true
description: Complete API reference for GPU Image Processing library
---

# API Reference

Complete reference for all modules and classes.

## High-Level APIs

| Module | Description | Key Classes |
|:-------|:------------|:------------|
| [ImageProcessor]({{ site.baseurl }}/api/image_processor) | Synchronous processing | `ImageProcessor` |
| [PipelineProcessor]({{ site.baseurl }}/api/pipeline_processor) | Async batch processing | `PipelineProcessor` |

## Operator Modules

| Module | Description | Key Functions |
|:-------|:------------|:--------------|
| [PixelOperator]({{ site.baseurl }}/api/pixel_operator) | Per-pixel ops | `invert`, `toGrayscale` |
| [ConvolutionEngine]({{ site.baseurl }}/api/convolution_engine) | Convolution | `gaussianBlur`, `sobelEdgeDetection` |
| [Filters]({{ site.baseurl }}/api/filters) | Filtering | `medianFilter`, `bilateralFilter` |
| [Geometric]({{ site.baseurl }}/api/geometric) | Transforms | `rotate`, `resize` |
| [Morphology]({{ site.baseurl }}/api/morphology) | Morphological | `erode`, `dilate` |
| [ColorSpace]({{ site.baseurl }}/api/color_space) | Conversions | `rgbToHsv`, `rgbToYuv` |
| [Threshold]({{ site.baseurl }}/api/threshold) | Thresholding | `threshold`, `otsuThreshold` |
| [HistogramCalculator]({{ site.baseurl }}/api/histogram_calculator) | Histogram | `calculate`, `equalize` |

## Quick Access

### By Task

| Task | Module(s) |
|:-----|:----------|
| Load/save images | `ImageIO`, `ImageProcessor` |
| Apply blur | `ConvolutionEngine`, `Filters` |
| Edge detection | `ConvolutionEngine` |
| Resize/rotate | `Geometric` |
| Color conversion | `ColorSpace` |
| Batch processing | `PipelineProcessor` |

### Common Patterns

**Basic Processing:**
```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage result = processor.downloadImage(blurred);
ImageIO::save("output.jpg", result);
```

**Pipeline Processing:**
```cpp
#include "gpu_image/pipeline_processor.hpp"

PipelineProcessor pipeline(4);  // 4 streams
std::vector<HostImage> results = pipeline.processBatchHost(inputs);
```

## Version

```cpp
#include "gpu_image/gpu_image_processing.hpp"

// Compile-time
constexpr int major = GPU_IMAGE_VERSION_MAJOR;  // 2
constexpr int minor = GPU_IMAGE_VERSION_MINOR;  // 0
constexpr int patch = GPU_IMAGE_VERSION_PATCH;  // 0

// Runtime
std::string version = gpu_image::getVersionString();  // "2.0.0"
```

---

## Language

- **English** (this page)
- [简体中文]({{ site.baseurl }}/api.zh-CN/)

---

## Resources

- [Quick Start]({{ site.baseurl }}/setup/quickstart)
- [Installation]({{ site.baseurl }}/setup/installation)
- [Examples]({{ site.baseurl }}/tutorials/examples/)
