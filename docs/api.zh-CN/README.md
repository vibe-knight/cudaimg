---
layout: default
title: API 参考
nav_order: 4
has_children: true
description: GPU Image Processing 完整 API 参考
---

# API 参考

所有模块和类的完整参考。

## 高级 API

| 模块 | 说明 | 关键类 |
|:-----|:-----|:-------|
| [ImageProcessor]({{ site.baseurl }}/api/image_processor) | 同步图像处理 | `ImageProcessor` |
| [PipelineProcessor]({{ site.baseurl }}/api/pipeline_processor) | 异步批处理 | `PipelineProcessor` |

## 算子模块

| 模块 | 说明 | 关键函数 |
|:-----|:-----|:---------|
| [PixelOperator]({{ site.baseurl }}/api/pixel_operator) | 逐像素操作 | `invert`, `toGrayscale` |
| [ConvolutionEngine]({{ site.baseurl }}/api/convolution_engine) | 卷积操作 | `gaussianBlur`, `sobelEdgeDetection` |
| [Filters]({{ site.baseurl }}/api/filters) | 图像滤波 | `medianFilter`, `bilateralFilter` |
| [Geometric]({{ site.baseurl }}/api/geometric) | 几何变换 | `rotate`, `resize`, `crop` |
| [Morphology]({{ site.baseurl }}/api/morphology) | 形态学操作 | `erode`, `dilate`, `open`, `close` |
| [ColorSpace]({{ site.baseurl }}/api/color_space) | 色彩空间转换 | `rgbToHsv`, `rgbToYuv` |
| [Threshold]({{ site.baseurl }}/api/threshold) | 阈值操作 | `threshold`, `otsuThreshold` |
| [HistogramCalculator]({{ site.baseurl }}/api/histogram_calculator) | 直方图操作 | `calculate`, `equalize` |

## 快速访问

### 按任务分类

| 任务 | 模块 |
|:-----|:-----|
| 加载/保存图像 | `ImageIO`, `ImageProcessor` |
| 应用模糊 | `ConvolutionEngine`, `Filters` |
| 边缘检测 | `ConvolutionEngine` |
| 缩放图像 | `Geometric` |
| 颜色转换 | `ColorSpace` |
| 批处理 | `PipelineProcessor` |

### 常用模式

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// 加载
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);

// 处理
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);

// 保存
HostImage result = processor.downloadImage(blurred);
ImageIO::save("output.jpg", result);
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

---

## 语言选择

- **简体中文** (本页)
- [English]({{ site.baseurl }}/api/)

---

## 更多资源

- [快速开始]({{ site.baseurl }}/setup/quickstart)
- [安装指南]({{ site.baseurl }}/setup/installation)
- [架构说明]({{ site.baseurl }}/architecture/architecture)
- [常见问题]({{ site.baseurl }}/tutorials/faq)
