---
layout: default
title: Quick Start
nav_order: 2
parent: Documentation
description: Get started with Mini-OpenCV in 5 minutes - build, test, and run your first GPU image processing program
---

# Quick Start Guide

Get started with Mini-OpenCV in 5 minutes. This guide walks you through building the library, running tests, and processing your first image.

## Prerequisites Check

Before you begin, ensure you have the required tools:

```bash
# Check CUDA installation
nvcc --version
# Expected: Cuda compilation tools, release 11.0 or higher

# Check CMake version
cmake --version
# Expected: cmake version 3.18.0 or higher

# Check C++ compiler
g++ --version
# Expected: GCC 7+ or Clang 7+

# Verify NVIDIA GPU
nvidia-smi
# Expected: Shows GPU information and driver version
```

## Clone and Build

```bash
# Clone the repository
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Configure with tests and examples
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON

# Build with all available cores
cmake --build build -j$(nproc)
```

Build time: ~2-5 minutes depending on your hardware.

## Run Tests

Verify the installation by running the test suite:

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Or run the test binary directly
./build/bin/gpu_image_tests

# Run a specific test suite
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.*
```

All tests should pass. If tests fail, check [Troubleshooting](#troubleshooting).

## Run Examples

Try the included examples to see the library in action:

```bash
# Run basic example
./build/bin/basic_example

# Run pipeline example (async processing demo)
./build/bin/pipeline_example
```

Expected output for `basic_example`:
```
=== GPU Image Processing Library Demo ===
Version: 2.0.0
Device: NVIDIA GeForce RTX 4090, Compute Capability: 8.9, Memory: 24564 MB

Creating test image...
Original: 256x256x3, min=0, max=255, avg=127.5

Uploading to GPU...

--- Invert Operation ---
Inverted: 256x256x3, min=0, max=255, avg=127.5
Double Inverted: 256x256x3, min=0, max=255, avg=127.5

--- Grayscale Conversion ---
Grayscale: 256x256x1, min=0, max=255, avg=127.5

=== Demo Complete ===
```

## Write Your First Program

Create a simple program that loads, processes, and saves an image:

```cpp
// my_program.cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // Step 1: Check CUDA availability
    if (!isCudaAvailable()) {
        std::cerr << "CUDA not available!" << std::endl;
        return 1;
    }
    std::cout << getDeviceInfo() << std::endl;

    // Step 2: Create processor
    ImageProcessor processor;

    // Step 3: Create a test image
    HostImage hostImage = ImageUtils::createHostImage(512, 512, 3);
    
    // Fill with gradient pattern
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            hostImage.at(x, y, 0) = (x * 255) / 512;  // R
            hostImage.at(x, y, 1) = (y * 255) / 512;  // G
            hostImage.at(x, y, 2) = 128;              // B
        }
    }

    // Step 4: Load to GPU
    GpuImage gpuImage = processor.loadFromHost(hostImage);

    // Step 5: Apply operations
    GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(gpuImage);
    GpuImage gray = processor.toGrayscale(gpuImage);

    // Step 6: Download results
    HostImage resultBlur = processor.downloadImage(blurred);
    HostImage resultEdges = processor.downloadImage(edges);
    HostImage resultGray = processor.downloadImage(gray);

    std::cout << "Processing complete!" << std::endl;
    std::cout << "Blurred: " << resultBlur.width << "x" << resultBlur.height << std::endl;
    std::cout << "Edges: " << resultEdges.width << "x" << resultEdges.height << std::endl;
    std::cout << "Gray: " << resultGray.width << "x" << resultGray.height << std::endl;

    return 0;
}
```

### Compile Your Program

```bash
# Using CMake (recommended)
# Add to your CMakeLists.txt:
# find_package(gpu_image_processing REQUIRED)
# target_link_libraries(your_target gpu_image::gpu_image_processing)

# Or compile directly with nvcc
nvcc -std=c++17 my_program.cpp \
    -I/path/to/mini-opencv/include \
    -L/path/to/mini-opencv/build/lib \
    -lgpu_image_processing \
    -o my_program
```

## Pipeline Processing

For batch processing with maximum GPU utilization, use the pipeline processor:

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    // Create pipeline with 4 CUDA streams
    PipelineProcessor pipeline(4);

    // Add processing steps
    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        PixelOperator::adjustBrightness(img, temp, 20, stream);
        img = std::move(temp);
    });

    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, stream);
        img = std::move(temp);
    });

    // Process batch of images
    std::vector<HostImage> inputs = /* your images */;
    std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);

    return 0;
}
```

## Troubleshooting

### Build Errors

| Error | Solution |
|-------|----------|
| `CUDA not found` | Set `CUDAToolkit_ROOT` or ensure `nvcc` is in PATH |
| `CMake version too old` | Upgrade to CMake 3.18+ |
| `No CUDA-capable device` | Check NVIDIA driver installation |

### Runtime Errors

| Error | Solution |
|-------|----------|
| `CUDA out of memory` | Reduce image size or batch size |
| `Invalid kernel size` | Use odd kernel sizes (3, 5, 7, ...) |
| `Image format not supported` | Convert to RGB/RGBA or grayscale |

### Performance Issues

1. **Slow first run**: CUDA kernel compilation on first launch is normal
2. **Memory transfers**: Minimize CPU-GPU data transfers
3. **Stream utilization**: Use PipelineProcessor for batch processing

## Next Steps

- [Installation Guide](installation.md) - Detailed installation options
- [Architecture Overview](architecture.md) - Understanding the library design
- [Performance Guide](performance.md) - Optimization tips
- [API Reference](api/) - Complete API documentation

## Getting Help

- Check the [FAQ](faq.md)
- Search [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- Join [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)

---

*This guide was written for Mini-OpenCV v2.0.0*
