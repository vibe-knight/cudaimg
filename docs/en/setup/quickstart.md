# Quick Start

Get started with Mini-OpenCV in 5 minutes.

## Prerequisites

- CUDA 11.0+ installed
- CMake 3.18+
- C++17 compiler
- NVIDIA GPU with Compute Capability 7.5+

## Build

```bash
# Clone the repository
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Run tests
ctest --test-dir build --output-on-failure
```

## Basic Usage

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    // Create an ImageProcessor
    ImageProcessor processor;

    // Load an image from host
    HostImage hostImage = ImageIO::load("input.jpg");
    
    // Upload to GPU
    GpuImage gpu = processor.loadFromHost(hostImage);

    // Apply operations (all GPU-accelerated)
    GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(gpu);
    GpuImage gray = processor.grayscale(gpu);

    // Download result back to host
    HostImage result = processor.downloadImage(blurred);
    
    // Save result
    ImageIO::save("output.jpg", result);

    return 0;
}
```

## Available Operations

| Operation | Method |
|-----------|--------|
| Gaussian Blur | `gaussianBlur(image, kernelSize, sigma)` |
| Sobel Edge Detection | `sobelEdgeDetection(image)` |
| Grayscale | `grayscale(image)` |
| Invert | `invert(image)` |
| Resize | `resize(image, width, height)` |
| Rotate | `rotate(image, angle)` |
| Threshold | `threshold(image, value)` |
| Histogram Equalization | `histogramEqualization(image)` |

## Next Steps

- [Installation](./installation) - Detailed setup guide
- [Configuration](./configuration) - CMake options
- [Examples](../tutorials/examples) - More code examples
