---
layout: default
title: Examples
description: Mini-OpenCV code examples and tutorials - from basic usage to advanced pipeline processing
---

# Examples

Practical examples demonstrating Mini-OpenCV usage patterns.

## Available Examples

| Example | Description | Complexity | Time |
|---------|-------------|------------|------|
| [Basic Usage](basic-usage.md) | Load, process, and save images | Beginner | 10 min |
| [Pipeline Processing](pipeline-processing.md) | Batch processing with streams | Intermediate | 15 min |

## Running Examples

After building the project:

```bash
# Build examples
cmake --build build --target basic_example pipeline_example

# Run basic example
./build/bin/basic_example

# Run pipeline example
./build/bin/pipeline_example
```

## Example Source Code

All source code is available in the `examples/` directory:

| File | Description |
|------|-------------|
| `examples/basic_example.cpp` | Basic image processing operations |
| `examples/pipeline_example.cpp` | Async pipeline batch processing |

## Quick Samples

### Basic Image Processing

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// Create and upload image
HostImage host = ImageUtils::createHostImage(512, 512, 3);
GpuImage gpu = processor.loadFromHost(host);

// Apply operations
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);

// Download result
HostImage result = processor.downloadImage(edges);
```

### Batch Processing

```cpp
PipelineProcessor pipeline(4);  // 4 streams

pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
    img = std::move(temp);
});

auto outputs = pipeline.processBatchHost(inputs);
```

## Language Selection

- **English** (this page)
- [简体中文 (Chinese)](basic-usage.zh-CN.md)

## Contributing Examples

Want to share your use case? See the [Contributing Guide](../../CONTRIBUTING.md).

---

*More examples coming soon!*
