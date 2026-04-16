---
layout: default
title: ConvolutionEngine
description: ConvolutionEngine API reference - GPU-accelerated convolution operations.
---

# ConvolutionEngine

GPU-accelerated 2D convolution operations with shared memory optimization.

## Header

```cpp
#include "gpu_image/convolution_engine.hpp"
```

## Functions

### gaussianBlur()

```cpp
void gaussianBlur(const GpuImage& src, GpuImage& dst, 
                  int kernelSize, float sigma,
                  cudaStream_t stream = nullptr);
```

Applies Gaussian blur using separable kernel for efficiency.

**Parameters:**
- `src` - Source image
- `dst` - Destination image
- `kernelSize` - Kernel size (odd, >= 3)
- `sigma` - Standard deviation (> 0)
- `stream` - Optional CUDA stream

### sobelEdgeDetection()

```cpp
void sobelEdgeDetection(const GpuImage& src, GpuImage& dst,
                        cudaStream_t stream = nullptr);
```

Detects edges using Sobel operator (3×3).

**Parameters:**
- `src` - Source image
- `dst` - Edge magnitude output
- `stream` - Optional CUDA stream

### applyCustomKernel()

```cpp
void applyCustomKernel(const GpuImage& src, GpuImage& dst,
                       const float* kernel, int kernelSize,
                       cudaStream_t stream = nullptr);
```

Applies custom convolution kernel.

**Parameters:**
- `src` - Source image
- `dst` - Destination image
- `kernel` - Kernel weights (kernelSize×kernelSize)
- `kernelSize` - Kernel dimensions (odd)
- `stream` - Optional CUDA stream

## Example

```cpp
#include "gpu_image/convolution_engine.hpp"

GpuImage src, dst;
// ... load src ...

// Gaussian blur
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f);

// Sobel edge detection
GpuImage edges;
ConvolutionEngine::sobelEdgeDetection(dst, edges);
```

## See Also

- [Filters](filters) - Additional filtering operations
- [ImageProcessor](image_processor) - High-level interface
