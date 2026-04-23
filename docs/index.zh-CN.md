---
layout: default
title: 中文文档
nav_order: 2
has_children: true
description: GPU 图像处理库 - 基于 CUDA 的高性能计算机视觉库，比 CPU OpenCV 快 30-50 倍
---

# GPU 图像处理库
{: .fs-9 .fw-700 }

基于 CUDA 的高性能计算机视觉库。**比 CPU OpenCV 快 30-50 倍**。
{: .fs-6 .fw-300 }

[快速开始]({{ site.baseurl }}/setup/quickstart){: .btn .btn-primary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[GitHub 仓库](https://github.com/LessUp/mini-opencv){: .btn .btn-secondary .fs-5 .mb-4 .mb-md-0 .mr-2 }
[API 文档]({{ site.baseurl }}/api.zh-CN/){: .btn .btn-outline .fs-5 .mb-4 .mb-md-0 }

---

## 为什么选择 GPU 图像处理？

传统的 CPU 图像处理难以满足实时应用需求。本库利用 **NVIDIA CUDA** 直接在 GPU 上运行图像运算，提供巨大的性能提升。

| 指标 | CPU (OpenCV) | GPU (本库) | 加速比 |
|:-----|:-------------|:-----------|:-------|
| 高斯模糊 (1024×1024) | ~15 ms | ~0.4 ms | **37×** |
| Sobel 边缘检测 | ~12 ms | ~0.3 ms | **40×** |
| 直方图均衡化 | ~8 ms | ~0.2 ms | **40×** |
| 双边滤波 | ~150 ms | ~4 ms | **37×** |

*在 RTX 4090 vs Intel i9-13900K 上的基准测试*

---

## 快速开始

```bash
# 克隆并构建
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure
```

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage result = processor.downloadImage(blurred);
```

---

## 功能特性

### 核心算子

| 类别 | 支持的操作 |
|:-----|:-----------|
| **像素操作** | 反转、灰度化、亮度调节 |
| **卷积** | 高斯模糊、Sobel 边缘检测、自定义卷积核 |
| **滤波** | 中值滤波、双边滤波、均值滤波、锐化 |
| **几何变换** | 缩放、旋转、翻转、仿射变换、透视变换 |
| **形态学** | 腐蚀、膨胀、开闭运算、形态学梯度 |
| **色彩空间** | RGB/HSV/YUV 转换 |
| **阈值** | 全局阈值、自适应阈值、Otsu 自动阈值 |
| **直方图** | 计算和均衡化 |

### GPU 架构支持

| 架构 | 计算能力 | 代表显卡 |
|:-----|:---------|:---------|
| Turing | SM 75 | RTX 20 系列, T4 |
| Ampere | SM 80/86 | A100, RTX 30 系列 |
| Ada Lovelace | SM 89 | RTX 40 系列, L4 |
| Hopper | SM 90 | H100 |

---

## 架构设计

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│         ImageProcessor  ·  PipelineProcessor                 │
├─────────────────────────────────────────────────────────────┤
│              Operator Layer (CUDA Kernels)                   │
│  PixelOperator  │  ConvolutionEngine  │  Geometric          │
│  Morphology     │  ColorSpace         │  Filters            │
│  Threshold      │  HistogramCalculator│  ImageResizer       │
├─────────────────────────────────────────────────────────────┤
│                  Infrastructure Layer                        │
│  DeviceBuffer  ·  GpuImage/HostImage  ·  CudaError          │
│  ImageIO       ·  StreamManager                              │
└─────────────────────────────────────────────────────────────┘
```

三层架构设计：**高层 API** → **CUDA 核函数** → **基础设施**

---

## 文档导航

| 文档 | 描述 |
|:-----|:-----|
| [快速开始]({{ site.baseurl }}/setup/quickstart) | 5 分钟构建并运行 |
| [安装指南]({{ site.baseurl }}/setup/installation) | 完整安装说明 |
| [API 文档]({{ site.baseurl }}/api.zh-CN/) | 完整 API 参考 |
| [架构说明]({{ site.baseurl }}/architecture/architecture) | 设计和内部原理 |
| [性能优化]({{ site.baseurl }}/tutorials/performance) | 优化指南 |
| [示例代码]({{ site.baseurl }}/tutorials/examples/) | 代码示例 |
| [常见问题]({{ site.baseurl }}/tutorials/faq) | 常见问题解答 |

---

## 系统要求

| 组件 | 最低要求 | 推荐配置 |
|:-----|:---------|:---------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ | C++17 | C++17 |
| GPU | 计算能力 7.5+ | RTX 30/40 系列 |

---

## 参与贡献

欢迎贡献代码！查看 [Contributing Guide](https://github.com/LessUp/mini-opencv/blob/main/CONTRIBUTING.md)

---

## 开源协议

[MIT License](https://github.com/LessUp/mini-opencv/blob/main/LICENSE) — Copyright 2025-2026 LessUp

---

[English]({{ site.baseurl }}/) · **[简体中文]({{ site.baseurl }}/index.zh-CN)**
