---
layout: default
title: 文档
description: Mini-OpenCV 文档 - GPU 图像处理库的完整指南和 API 参考手册。
---

# Mini-OpenCV 文档

欢迎使用 Mini-OpenCV 文档。这是一个基于 CUDA 的高性能图像处理库，提供像素操作、卷积、形态学、几何变换等 GPU 加速算子。

## 快速导航

| 快速开始 | 用户指南 | API 参考 | 示例教程 |
|---------|---------|---------|---------|
| [快速入门](quickstart.zh-CN) | [安装指南](installation.zh-CN) | [API 总览](api.zh-CN/) | [基础使用](examples/basic-usage.zh-CN) |
| [架构说明](architecture.zh-CN) | [性能优化](performance.zh-CN) | [ImageProcessor](api.zh-CN/image_processor) | [流水线处理](examples/pipeline-processing.zh-CN) |
| [常见问题](faq.zh-CN) | - | [PipelineProcessor](api.zh-CN/pipeline_processor) | - |

## 文档结构

```
docs/
├── README.md              # 本文档 - 文档索引
├── quickstart.md          # 5分钟快速入门（英文）
├── quickstart.zh-CN.md    # 5分钟快速入门（中文）
├── installation.md        # 详细安装指南（英文）
├── installation.zh-CN.md  # 详细安装指南（中文）
├── architecture.md        # 架构与设计概览（英文）
├── architecture.zh-CN.md  # 架构与设计概览（中文）
├── performance.md         # 性能优化指南（英文）
├── performance.zh-CN.md   # 性能优化指南（中文）
├── faq.md                 # 常见问题（英文）
├── faq.zh-CN.md           # 常见问题（中文）
├── api/                   # API 参考（英文）
├── api.zh-CN/             # API 参考（中文）
└── examples/              # 扩展示例
```

## 语言选择

- **简体中文**（当前页面）
- [English (英文)](README)

## 快速开始

初次使用 Mini-OpenCV？从 [快速入门指南](quickstart.zh-CN) 开始，5分钟即可上手。

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage image = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);
```

## 功能概览

### 图像处理算子

| 类别 | 算子 |
|-----|------|
| **像素操作** | 反色、灰度化、亮度调整 |
| **卷积** | 高斯模糊、Sobel 边缘检测、自定义卷积核 |
| **直方图** | 计算、均衡化 |
| **缩放** | 双线性插值、最近邻插值 |
| **形态学** | 腐蚀、膨胀、开运算、闭运算、梯度 |
| **阈值** | 全局阈值、自适应阈值、Otsu 自动阈值 |
| **色彩空间** | RGB/HSV/YUV 转换、通道操作 |
| **几何变换** | 旋转、翻转、仿射、透视、裁剪 |
| **滤波** | 中值、双边、盒式、锐化、拉普拉斯 |
| **图像算术** | 加、减、乘、混合、加权和 |
| **流水线** | 多步骤异步处理，基于 CUDA Streams |

## 系统要求

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 兼容编译器
- NVIDIA GPU (Compute Capability 7.5+)

详细要求请参阅 [安装指南](installation.zh-CN)。

## 参与贡献

欢迎贡献代码！请参阅我们的 [贡献指南](../CONTRIBUTING) 了解如何参与项目。

## 支持与帮助

- **问题反馈**: [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- **讨论交流**: [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)
- **在线文档**: [GitHub Pages](https://lessup.github.io/mini-opencv/)

## 许可证

MIT 许可证 - 详见 [LICENSE](../LICENSE) 文件。

---

*最后更新: 2026-04-16*
