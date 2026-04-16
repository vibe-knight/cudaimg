---
layout: default
title: Basic Usage Example
description: Basic Mini-OpenCV usage example - loading, processing, and saving images.
---

# Basic Usage Example

Complete walkthrough of loading an image, applying operations, and saving the result.

## Full Code

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // Step 1: Check CUDA availability
    if (!isCudaAvailable()) {
        std::cerr << "CUDA not available!" << std::endl;
        return 1;
    }
    std::cout << "Using: " << getDeviceInfo() << std::endl;

    // Step 2: Create processor
    ImageProcessor processor;

    // Step 3: Create test image
    HostImage input = ImageUtils::createHostImage(512, 512, 3);
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            input.at(x, y, 0) = (x * 255) / 512;  // R gradient
            input.at(x, y, 1) = (y * 255) / 512;  // G gradient
            input.at(x, y, 2) = 128;               // B constant
        }
    }

    // Step 4: Upload to GPU
    GpuImage gpu = processor.loadFromHost(input);

    // Step 5: Apply operations
    GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(blurred);

    // Step 6: Download result
    HostImage output = processor.downloadImage(edges);

    // Step 7: Verify output
    std::cout << "Output: " << output.width << "x" << output.height 
              << "x" << output.channels << std::endl;

    return 0;
}
```

## Step-by-Step Explanation

### 1. CUDA Check

Always verify CUDA is available before processing.

### 2. Processor Creation

`ImageProcessor` manages GPU resources and provides synchronous operations.

### 3. Image Creation

`HostImage` stores data in CPU RAM with convenient pixel accessors.

### 4. GPU Upload

`loadFromHost()` copies data to GPU memory via PCIe.

### 5. Processing

Operations run on GPU and synchronize automatically.

### 6. Download

`downloadImage()` copies results back to CPU memory.

## Compilation

```bash
# With CMake
target_link_libraries(my_app gpu_image::gpu_image_processing)

# Direct compilation
nvcc -std=c++17 example.cpp -I/path/to/mini-opencv/include \
    -L/path/to/mini-opencv/build/lib -lgpu_image_processing -o example
```
