# GPU Image Processing Library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)

[简体中文](README.md) | English

A high-performance CUDA-based image processing library — a mini OpenCV.

## Features

- **Pixel Operations**: Invert, grayscale, brightness adjustment
- **Convolution**: Gaussian blur, Sobel edge detection (shared memory optimized)
- **Histogram**: Calculation and equalization (atomic ops + parallel reduction)
- **Scaling**: Bilinear and nearest-neighbor interpolation
- **Morphology**: Erosion, dilation, opening, closing, gradient, top-hat, black-hat
- **Thresholding**: Global, adaptive, Otsu auto-threshold
- **Color Space**: RGB/HSV/YUV conversion, channel split/merge
- **Geometric Transforms**: Rotation, flip, affine, perspective, crop, pad
- **Filters**: Median, bilateral, box, sharpen, Laplacian
- **Image Arithmetic**: Add, subtract, multiply, blend, weighted sum, abs diff
- **Pipeline Processing**: Async parallel processing via CUDA Streams

## Requirements

- CUDA Toolkit 11.0+, CMake 3.18+, C++17, GPU CC 7.5+

## Build

```bash
mkdir build && cd build
cmake .. && make -j$(nproc)
```

## Usage

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpuImage = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpuImage);
HostImage result = processor.downloadImage(blurred);
```

### Pipeline

```cpp
PipelineProcessor pipeline(4);  // 4 CUDA streams
pipeline.addStep([](GpuImage& img, cudaStream_t s) { /* ... */ });
std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
```

## Project Structure

```
├── include/gpu_image/     # Headers
├── src/                   # CUDA source files
├── tests/                 # Unit tests
├── examples/              # Examples
├── benchmarks/            # Benchmarks
└── CMakeLists.txt
```

## License

MIT License
