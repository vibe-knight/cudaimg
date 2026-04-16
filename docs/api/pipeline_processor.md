---
layout: default
title: PipelineProcessor
description: PipelineProcessor API reference - asynchronous batch processing with CUDA streams.
---

# PipelineProcessor

Asynchronous batch processing using multiple CUDA streams for maximum GPU utilization.

## Header

```cpp
#include "gpu_image/pipeline_processor.hpp"
```

## Class Overview

```cpp
namespace gpu_image {

class PipelineProcessor {
public:
    explicit PipelineProcessor(int numStreams);
    ~PipelineProcessor();

    // Add processing step
    void addStep(std::function<void(GpuImage&, cudaStream_t)> step);
    
    // Process single image
    HostImage processHost(const HostImage& input);
    
    // Process batch
    std::vector<HostImage> processBatchHost(const std::vector<HostImage>& inputs);
    
    // Synchronization
    void synchronize();
    
    // Info
    int stepCount() const;
    int streamCount() const;
};

}
```

## Constructor

### PipelineProcessor()

```cpp
explicit PipelineProcessor(int numStreams);
```

**Parameters:**
- `numStreams` - Number of CUDA streams for concurrent processing (typically 4-8)

**Example:**
```cpp
PipelineProcessor pipeline(4);  // 4 concurrent streams
```

## Adding Steps

### addStep()

Add a processing step to the pipeline.

```cpp
void addStep(std::function<void(GpuImage&, cudaStream_t)> step);
```

**Parameters:**
- `step` - Function that transforms an image using the provided stream

**Example:**
```cpp
pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, stream);
    img = std::move(temp);
});
```

## Processing

### processHost()

Process a single image through the pipeline.

```cpp
HostImage processHost(const HostImage& input);
```

### processBatchHost()

Process multiple images with concurrent streams.

```cpp
std::vector<HostImage> processBatchHost(const std::vector<HostImage>& inputs);
```

**Returns:** Vector of processed images in the same order as input

**Example:**
```cpp
std::vector<HostImage> inputs = loadImages();
std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
```

## Complete Example

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // Create pipeline with 4 streams
    PipelineProcessor pipeline(4);
    
    // Add processing steps
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        GpuImage temp;
        PixelOperator::adjustBrightness(img, temp, 20, s);
        img = std::move(temp);
    });
    
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        GpuImage temp;
        ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
        img = std::move(temp);
    });
    
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        PixelOperator::invertInPlace(img, s);
    });
    
    // Process batch
    std::vector<HostImage> inputs = loadBatch(100);
    auto results = pipeline.processBatchHost(inputs);
    
    std::cout << "Processed " << results.size() << " images" << std::endl;
    
    return 0;
}
```

## See Also

- [ImageProcessor](image_processor) - For simple synchronous processing
