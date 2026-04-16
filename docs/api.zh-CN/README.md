---
layout: default
title: API 参考
description: Mini-OpenCV 完整 API 参考 - 所有模块、类和函数。
---

# API 参考

Mini-OpenCV 所有模块的完整参考。

## 模块概览

### 高级 API

| 模块 | 说明 | 关键类 |
|-----|------|--------|
| [ImageProcessor](image_processor) | 同步图像处理 | `ImageProcessor` |
| [PipelineProcessor](pipeline_processor) | 异步批处理 | `PipelineProcessor` |

### 算子模块

| 模块 | 说明 | 关键函数 |
|-----|------|---------|
| [PixelOperator](pixel_operator) | 逐像素操作 | `invert`, `toGrayscale`, `adjustBrightness` |
| [ConvolutionEngine](convolution_engine) | 卷积操作 | `gaussianBlur`, `sobelEdgeDetection` |
| [Filters](filters) | 图像滤波 | `medianFilter`, `bilateralFilter` |
| [Geometric](geometric) | 几何变换 | `rotate`, `resize`, `crop` |
| [Morphology](morphology) | 形态学操作 | `erode`, `dilate`, `open`, `close` |
| [ColorSpace](color_space) | 色彩空间转换 | `rgbToHsv`, `rgbToYuv` |
| [Threshold](threshold) | 阈值操作 | `threshold`, `adaptiveThreshold`, `otsuThreshold` |
| [HistogramCalculator](histogram_calculator) | 直方图操作 | `calculate`, `equalize` |

### 基础设施

| 模块 | 说明 | 关键类 |
|-----|------|--------|
| [DeviceBuffer](device_buffer) | GPU 内存管理 | `DeviceBuffer` |
| [GpuImage/HostImage](gpu_image) | 图像容器 | `GpuImage`, `HostImage` |
| [ImageIO](image_io) | 文件 I/O | `ImageIO` |
| [CudaError](cuda_error) | 错误处理 | `CudaException` |

## 快速访问

### 按任务分类

| 任务 | 模块 |
|------|------|
| 加载和保存图像 | `ImageIO`, `ImageProcessor` |
| 应用模糊 | `ConvolutionEngine`, `Filters` |
| 边缘检测 | `ConvolutionEngine` |
| 缩放图像 | `Geometric` |
| 颜色转换 | `ColorSpace`, `PixelOperator` |
| 去除噪点 | `Filters`, `Morphology` |
| 批处理 | `PipelineProcessor` |

### 按图像类型

| 图像类型 | 推荐操作 |
|---------|---------|
| RGB/RGBA | 所有操作 |
| 灰度 | 像素、卷积、几何 |
| 二值 | 形态学、阈值 |

## 版本信息

```cpp
#include "gpu_image/gpu_image_processing.hpp"

// 编译期版本
constexpr int major = GPU_IMAGE_VERSION_MAJOR;  // 2
constexpr int minor = GPU_IMAGE_VERSION_MINOR;  // 0
constexpr int patch = GPU_IMAGE_VERSION_PATCH;  // 0

// 运行时版本
std::string version = gpu_image::getVersionString();  // "2.0.0"
```

## 常用模式

### 基础处理模式

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// 加载
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);

// 处理
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(blurred);

// 保存
HostImage result = processor.downloadImage(edges);
ImageIO::save("output.jpg", result);
```

### 直接算子访问

```cpp
#include "gpu_image/convolution_engine.hpp"

GpuImage src, dst;
// ... 加载 src ...

ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f);
```

### 流水线模式

```cpp
#include "gpu_image/pipeline_processor.hpp"

PipelineProcessor pipeline(4);  // 4 个流

pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
    img = std::move(temp);
});

std::vector<HostImage> results = pipeline.processBatchHost(inputs);
```

## 语言选择

- **简体中文**（当前页面）
- [English (英文)](../api/)

## 更多资源

- [快速入门](../quickstart.zh-CN) - 5分钟上手
- [安装指南](../installation.zh-CN) - 安装指导
- [架构说明](../architecture.zh-CN) - 设计概览
- [性能优化](../performance.zh-CN) - 优化指南
- [常见问题](../faq.zh-CN) - 常见问题
