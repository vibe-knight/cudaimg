---
layout: default
title: Mini-OpenCV — GPU 图像处理库
---

# Mini-OpenCV

[![CI](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml)
[![Pages](https://github.com/LessUp/mini-opencv/actions/workflows/pages.yml/badge.svg)](https://lessup.github.io/mini-opencv/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/LessUp/mini-opencv/blob/main/LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

基于 CUDA 的高性能图像处理库 — 覆盖像素操作、卷积、形态学、几何变换、滤波、色彩空间转换与异步流水线处理。所有算子均 GPU 并行实现，提供类 OpenCV 风格的 C++ API。

---

## 架构总览

```
┌─────────────────────────────────────────────────────────┐
│                    用户应用层                             │
│         ImageProcessor  ·  PipelineProcessor             │
├─────────────────────────────────────────────────────────┤
│                    算子层 (CUDA Kernels)                  │
│  PixelOperator │ ConvolutionEngine │ HistogramCalculator  │
│  ImageResizer  │ Morphology        │ Threshold            │
│  ColorSpace    │ Geometric         │ Filters              │
│  ImageArithmetic                                         │
├─────────────────────────────────────────────────────────┤
│                    基础设施层                             │
│  DeviceBuffer · MemoryManager · StreamManager · CudaError │
│  GpuImage · HostImage · ImageIO (stb)                    │
└─────────────────────────────────────────────────────────┘
```

---

## 功能矩阵

### 像素 & 卷积

| 算子 | 功能 | 优化技术 |
|------|------|----------|
| **PixelOperator** | 反色、灰度化、亮度调整 | 逐像素并行 |
| **ConvolutionEngine** | 高斯模糊、Sobel 边缘检测、自定义卷积核 | Shared Memory Tiling |
| **HistogramCalculator** | 直方图计算、均衡化 | 原子操作 + 并行规约 |

### 形态学 & 阈值

| 算子 | 功能 | 说明 |
|------|------|------|
| **Morphology** | 腐蚀、膨胀、开/闭运算、梯度、顶帽、黑帽 | 可自定义结构元素 |
| **Threshold** | 全局阈值、自适应阈值、Otsu 自动阈值 | 直方图驱动 |

### 几何变换 & 滤波

| 算子 | 功能 | 说明 |
|------|------|------|
| **Geometric** | 旋转、翻转、仿射/透视变换、裁剪、填充 | 双线性插值 |
| **ImageResizer** | 双线性插值、最近邻插值 | 任意尺寸缩放 |
| **Filters** | 中值、双边、盒式、锐化、拉普拉斯 | 保边去噪 |

### 色彩空间 & 图像算术

| 算子 | 功能 | 说明 |
|------|------|------|
| **ColorSpace** | RGB/HSV/YUV 转换、通道分离/合并 | 批量转换 |
| **ImageArithmetic** | 加/减/乘、Alpha 混合、加权和、绝对差 | 标量 & 图像 |

### 流水线

| 算子 | 功能 | 说明 |
|------|------|------|
| **PipelineProcessor** | 多步骤串联、批量异步处理 | 多 CUDA Stream 并发 |

---

## 快速开始

```bash
# 构建
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j$(nproc)

# 运行测试
ctest --output-on-failure

# 运行示例
./bin/basic_example
./bin/pipeline_example
```

### 代码示例

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

ImageProcessor processor;
GpuImage gpuImage = processor.loadFromHost(hostImage);

// 基础处理
GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
GpuImage edges   = processor.sobelEdgeDetection(gpuImage);
GpuImage gray    = processor.toGrayscale(gpuImage);

HostImage result = processor.downloadImage(blurred);
```

```cpp
// 流水线批量处理
PipelineProcessor pipeline(4);  // 4 个 CUDA streams
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
    img = std::move(temp);
});
std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
```

---

## 技术栈

| 类别 | 技术 |
|------|------|
| **语言** | CUDA + C++17 |
| **构建** | CMake 3.18+ |
| **测试** | Google Test v1.14.0 (FetchContent) |
| **基准** | Google Benchmark v1.8.3 (可选) |
| **图像 I/O** | stb (可选，FetchContent) |
| **代码格式** | clang-format |
| **CI** | GitHub Actions (CUDA build + format check) |

## GPU 架构支持

| 架构 | Compute Capability | 代号 |
|------|-------------------|------|
| Turing | SM 75 | RTX 20xx / T4 |
| Ampere | SM 80 / 86 | A100 / RTX 30xx |
| Ada Lovelace | SM 89 | RTX 40xx / L4 |
| Hopper | SM 90 | H100 |

---

## 项目结构

```
mini-opencv/
├── include/gpu_image/          # 公共头文件（19 个模块）
│   ├── gpu_image_processing.hpp  # 统一入口头文件
│   ├── image_processor.hpp       # 高级同步 API
│   ├── pipeline_processor.hpp    # 流水线异步 API
│   ├── convolution_engine.hpp    # 卷积算子
│   ├── morphology.hpp            # 形态学算子
│   ├── geometric.hpp             # 几何变换
│   ├── filters.hpp               # 滤波 + 图像算术
│   ├── color_space.hpp           # 色彩空间转换
│   ├── threshold.hpp             # 阈值处理
│   ├── device_buffer.hpp         # RAII GPU 内存
│   └── ...                       # cuda_error, gpu_image, stream_manager 等
├── src/                          # CUDA/C++ 源文件（16 个）
├── tests/                        # 单元测试（12 个测试文件）
├── examples/                     # 示例程序
│   ├── basic_example.cpp           # 基础用法
│   └── pipeline_example.cpp        # 流水线用法
├── benchmarks/                   # 性能基准测试
├── CMakeLists.txt                # CMake 构建系统
└── _config.yml                   # GitHub Pages 配置
```

---

## 工程质量

- **现代 CMake** — target-based 编译选项与 generator expressions，支持 `BUILD_INTERFACE`/`INSTALL_INTERFACE`
- **FetchContent 依赖** — GTest v1.14.0、Google Benchmark v1.8.3、stb（无需手动安装第三方库）
- **CUDA 架构自动检测** — CMake 3.24+ 自动使用 `native`，低版本回退常见架构列表
- **Install 支持** — `gpu_image::gpu_image_processing` CMake 导出目标，可作为依赖库使用
- **版本注入** — 编译期注入 `GPU_IMAGE_VERSION_MAJOR/MINOR/PATCH` 宏
- **CI 流水线** — GitHub Actions 自动化构建 + clang-format 格式检查
- **完整测试** — 12 个测试文件覆盖所有算子模块
- **跨平台编译选项** — GCC/Clang (`-Wall -Wextra`) + MSVC (`/W4`) 双支持

---

## 链接

- [GitHub 仓库](https://github.com/LessUp/mini-opencv)
- [README (English)](README.md)
- [README (中文)](README.zh-CN.md)
- [贡献指南](CONTRIBUTING.md)
- [变更日志](changelog/)
