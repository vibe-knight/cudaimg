---
layout: default
title: ConvolutionEngine
description: ConvolutionEngine API 参考 - GPU 加速卷积操作。
---

# ConvolutionEngine

使用共享内存优化的 GPU 加速 2D 卷积操作。

## 头文件

```cpp
#include "gpu_image/convolution_engine.hpp"
```

## 函数

### gaussianBlur()

```cpp
void gaussianBlur(const GpuImage& src, GpuImage& dst, 
                  int kernelSize, float sigma,
                  cudaStream_t stream = nullptr);
```

应用高斯模糊，使用可分离卷积核提高效率。

**参数：**
- `src` - 源图像
- `dst` - 目标图像
- `kernelSize` - 卷积核大小（奇数，>= 3）
- `sigma` - 标准差（> 0）
- `stream` - 可选 CUDA 流

### sobelEdgeDetection()

```cpp
void sobelEdgeDetection(const GpuImage& src, GpuImage& dst,
                        cudaStream_t stream = nullptr);
```

使用 Sobel 算子（3×3）检测边缘。

**参数：**
- `src` - 源图像
- `dst` - 边缘幅值输出
- `stream` - 可选 CUDA 流

### applyCustomKernel()

```cpp
void applyCustomKernel(const GpuImage& src, GpuImage& dst,
                       const float* kernel, int kernelSize,
                       cudaStream_t stream = nullptr);
```

应用自定义卷积核。

**参数：**
- `src` - 源图像
- `dst` - 目标图像
- `kernel` - 卷积核权重（kernelSize×kernelSize）
- `kernelSize` - 卷积核尺寸（奇数）
- `stream` - 可选 CUDA 流

## 示例

```cpp
#include "gpu_image/convolution_engine.hpp"

GpuImage src, dst;
// ... 加载 src ...

// 高斯模糊
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f);

// Sobel 边缘检测
GpuImage edges;
ConvolutionEngine::sobelEdgeDetection(dst, edges);
```

## 另请参阅

- [Filters](filters) - 其他滤波操作
- [ImageProcessor](image_processor) - 高级接口
