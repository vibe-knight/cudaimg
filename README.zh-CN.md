# Mini-OpenCV — GPU 图像处理库

[![CI](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/badge/Docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/mini-opencv/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

[English](README.md) | 简体中文

基于 CUDA 的高性能图像处理库，为计算机视觉应用提供 GPU 加速算子。

> **⚡ 性能**：相比 CPU OpenCV 同等操作快 30-50 倍

---

## 📚 快速链接

| 资源 | 描述 |
|------|------|
| [安装指南](docs/installation.zh-CN.md) | 完整的安装配置指南 |
| [快速入门](docs/quickstart.zh-CN.md) | 5 分钟快速上手 |
| [API 参考](docs/api.zh-CN/) | 完整 API 文档 |
| [示例代码](docs/examples.zh-CN/) | 代码示例和教程 |
| [常见问题](docs/faq.zh-CN.md) | 常见问题解答 |

**完整文档：** https://lessup.github.io/mini-opencv/

---

## ✨ 功能特性

| 类别 | 算子 | 亮点 |
|------|------|------|
| **像素操作** | 反色、灰度化、亮度调整 | 逐像素并行 |
| **卷积** | 高斯模糊、Sobel 边缘检测 | Shared Memory Tiling |
| **直方图** | 计算、均衡化 | 原子操作 + 规约 |
| **几何变换** | 旋转、缩放、翻转、仿射 | 双线性插值 |
| **形态学** | 腐蚀、膨胀、开/闭运算 | 可自定义结构元素 |
| **阈值** | 全局、自适应、Otsu | 直方图驱动 |
| **色彩空间** | RGB/HSV/YUV 转换 | 批量转换 |
| **滤波** | 中值、双边、锐化 | 保边去噪 |
| **流水线** | 多步异步处理 | 多流并发 |

---

## 🏗️ 架构

```
┌─────────────────────────────────────────────────────────────┐
│                      应用层                                   │
│         ImageProcessor  ·  PipelineProcessor                 │
├─────────────────────────────────────────────────────────────┤
│                   算子层 (CUDA Kernels)                       │
│  像素操作      │  卷积引擎        │  几何变换                  │
│  形态学        │  色彩空间        │  滤波器                    │
│  阈值处理      │  直方图计算      │  图像缩放                  │
├─────────────────────────────────────────────────────────────┤
│                      基础设施层                               │
│  显存缓冲区  ·  GPU/Host 图像容器  ·  错误处理                 │
│  图像 I/O    ·  流管理器                                      │
└─────────────────────────────────────────────────────────────┘
```

---

## 🚀 快速开始

```bash
# 克隆并构建
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure

# 运行示例
./build/bin/basic_example
```

### 基础用法

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;

// 加载并处理
GpuImage gpu = processor.loadFromHost(hostImage);
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);
HostImage result = processor.downloadImage(edges);
```

---

## 📋 系统要求

| 组件 | 最低要求 | 推荐配置 |
|------|----------|----------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ | C++17 | C++17 |
| GPU | CC 7.5+ (Turing) | RTX 30/40 系列 |

---

## 📖 文档

完整文档请访问 [GitHub Pages](https://lessup.github.io/mini-opencv/)：

- [安装指南](docs/installation.zh-CN.md)
- [快速入门](docs/quickstart.zh-CN.md)
- [架构概览](docs/architecture.zh-CN.md)
- [性能优化](docs/performance.zh-CN.md)
- [API 参考](docs/api.zh-CN/)
- [常见问题](docs/faq.zh-CN.md)

---

## 🤝 贡献

请参阅 [CONTRIBUTING.md](CONTRIBUTING.md) 了解贡献指南。

---

## 📝 更新日志

请参阅 [CHANGELOG.md](CHANGELOG.md) 了解版本历史。

---

## 📄 许可证

MIT 许可证 — 详见 [LICENSE](LICENSE) 文件。

---

**⭐ 如果本项目对你有帮助，请给个 Star！**

如需支持，请提交 [Issue](https://github.com/LessUp/mini-opencv/issues) 或发起 [讨论](https://github.com/LessUp/mini-opencv/discussions)。
