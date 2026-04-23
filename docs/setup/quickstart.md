---
layout: default
title: Quick Start
nav_order: 1
parent: Documentation
description: Get started with GPU Image Processing in 5 minutes
---

# Quick Start

Get started in 5 minutes. Build the library, run tests, and process your first image.

## Prerequisites

Verify your environment:

```bash
# Check CUDA
nvcc --version
# Required: CUDA 11.0 or higher

# Check CMake
cmake --version
# Required: 3.18 or higher

# Check GPU
nvidia-smi
```

## Build

```bash
# Clone
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Configure
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON

# Build
cmake --build build -j$(nproc)
```

Build time: 2-5 minutes depending on hardware.

## Test

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Or run directly
./build/bin/gpu_image_tests
```

## Run Examples

```bash
./build/bin/basic_example
./build/bin/pipeline_example
```

## Your First Program

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // Check CUDA
    if (!isCudaAvailable()) {
        std::cerr << "CUDA not available!" << std::endl;
        return 1;
    }

    // Create processor
    ImageProcessor processor;

    // Create a test image
    HostImage hostImage = ImageUtils::createHostImage(512, 512, 3);
    
    // Fill with gradient
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            hostImage.at(x, y, 0) = (x * 255) / 512;
            hostImage.at(x, y, 1) = (y * 255) / 512;
            hostImage.at(x, y, 2) = 128;
        }
    }

    // Process on GPU
    GpuImage gpuImage = processor.loadFromHost(hostImage);
    GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
    HostImage result = processor.downloadImage(blurred);

    std::cout << "Success! Result: " 
              << result.width << "x" << result.height 
              << std::endl;

    return 0;
}
```

## Next Steps

- [Installation Guide]({{ site.baseurl }}/setup/installation) - Detailed setup options
- [API Reference]({{ site.baseurl }}/api/) - Complete API documentation
- [Examples]({{ site.baseurl }}/tutorials/examples/) - More code examples

---

*For Mini-OpenCV v2.0.0*
