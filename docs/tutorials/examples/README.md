---
layout: default
title: Examples
parent: Documentation
nav_order: 4
description: GPU Image Processing code examples and tutorials
---

# Examples

Practical examples demonstrating library usage patterns.

## Available Examples

| Example | Description | Level | Time |
|:--------|:------------|:------|:-----|
| [Basic Usage]({{ site.baseurl }}/tutorials/examples/basic-usage) | Load, process, and save images | Beginner | 10 min |
| [Pipeline Processing]({{ site.baseurl }}/tutorials/examples/pipeline-processing) | Batch processing with streams | Intermediate | 15 min |

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

## Source Code

All source code is available in the `examples/` directory:

| File | Description |
|:-----|:------------|
| `examples/basic_example.cpp` | Basic image processing |
| `examples/pipeline_example.cpp` | Async pipeline batch processing |

## Quick Samples

### Basic Processing

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

---

*More examples coming soon!*
