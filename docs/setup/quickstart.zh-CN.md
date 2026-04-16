---
layout: default
title: 快速入门
title_en: Quick Start
parent: 中文文档
nav_order: 1
description: 5 分钟快速入门 Mini-OpenCV - 构建、测试并运行第一个 GPU 图像处理程序
---

# 快速入门指南

5 分钟快速入门 Mini-OpenCV。本指南将带你完成库的构建、测试以及第一张图像的处理。

## 前置条件检查

开始前，请确保已安装所需工具：

```bash
# 检查 CUDA 安装
nvcc --version
# 预期输出: Cuda compilation tools, release 11.0 或更高

# 检查 CMake 版本
cmake --version
# 预期输出: cmake version 3.18.0 或更高

# 检查 C++ 编译器
g++ --version
# 预期输出: GCC 7+ 或 Clang 7+

# 验证 NVIDIA GPU
nvidia-smi
# 预期输出: 显示 GPU 信息和驱动版本
```

## 克隆与构建

```bash
# 克隆仓库
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# 配置（开启测试和示例）
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON

# 使用所有可用核心构建
cmake --build build -j$(nproc)
```

构建时间：根据硬件配置约 2-5 分钟。

## 运行测试

运行测试套件验证安装：

```bash
# 运行所有测试
ctest --test-dir build --output-on-failure

# 或直接运行测试二进制文件
./build/bin/gpu_image_tests

# 运行特定测试套件
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.*
```

所有测试应该通过。如果测试失败，请查看[故障排除](#故障排除)部分。

## 运行示例

运行包含的示例查看库的实际效果：

```bash
# 运行基础示例
./build/bin/basic_example

# 运行流水线示例（异步处理演示）
./build/bin/pipeline_example
```

`basic_example` 的预期输出：
```
=== GPU Image Processing Library Demo ===
Version: 2.0.0
Device: NVIDIA GeForce RTX 4090, Compute Capability: 8.9, Memory: 24564 MB

Creating test image...
Original: 256x256x3, min=0, max=255, avg=127.5

Uploading to GPU...

--- Invert Operation ---
Inverted: 256x256x3, min=0, max=255, avg=127.5
Double Inverted: 256x256x3, min=0, max=255, avg=127.5

--- Grayscale Conversion ---
Grayscale: 256x256x1, min=0, max=255, avg=127.5

=== Demo Complete ===
```

## 编写你的第一个程序

创建一个简单的程序来加载、处理和保存图像：

```cpp
// my_program.cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // 步骤 1: 检查 CUDA 可用性
    if (!isCudaAvailable()) {
        std::cerr << "CUDA not available!" << std::endl;
        return 1;
    }
    std::cout << getDeviceInfo() << std::endl;

    // 步骤 2: 创建处理器
    ImageProcessor processor;

    // 步骤 3: 创建测试图像
    HostImage hostImage = ImageUtils::createHostImage(512, 512, 3);
    
    // 使用渐变模式填充
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            hostImage.at(x, y, 0) = (x * 255) / 512;  // R
            hostImage.at(x, y, 1) = (y * 255) / 512;  // G
            hostImage.at(x, y, 2) = 128;              // B
        }
    }

    // 步骤 4: 加载到 GPU
    GpuImage gpuImage = processor.loadFromHost(hostImage);

    // 步骤 5: 应用操作
    GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(gpuImage);
    GpuImage gray = processor.toGrayscale(gpuImage);

    // 步骤 6: 下载结果
    HostImage resultBlur = processor.downloadImage(blurred);
    HostImage resultEdges = processor.downloadImage(edges);
    HostImage resultGray = processor.downloadImage(gray);

    std::cout << "Processing complete!" << std::endl;
    std::cout << "Blurred: " << resultBlur.width << "x" << resultBlur.height << std::endl;
    std::cout << "Edges: " << resultEdges.width << "x" << resultEdges.height << std::endl;
    std::cout << "Gray: " << resultGray.width << "x" << resultGray.height << std::endl;

    return 0;
}
```

### 编译程序

```bash
# 使用 CMake（推荐）
# 在 CMakeLists.txt 中添加：
# find_package(gpu_image_processing REQUIRED)
# target_link_libraries(your_target gpu_image::gpu_image_processing)

# 或直接使用 nvcc 编译
nvcc -std=c++17 my_program.cpp \
    -I/path/to/mini-opencv/include \
    -L/path/to/mini-opencv/build/lib \
    -lgpu_image_processing \
    -o my_program
```

## 流水线处理

对于批量处理以最大化 GPU 利用率，使用流水线处理器：

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    // 创建带有 4 个 CUDA 流的流水线
    PipelineProcessor pipeline(4);

    // 添加处理步骤
    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        PixelOperator::adjustBrightness(img, temp, 20, stream);
        img = std::move(temp);
    });

    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, stream);
        img = std::move(temp);
    });

    // 处理图像批次
    std::vector<HostImage> inputs = /* 你的图像 */;
    std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);

    return 0;
}
```

## 故障排除

### 构建错误

| 错误 | 解决方案 |
|------|----------|
| `CUDA not found` | 设置 `CUDAToolkit_ROOT` 或确保 `nvcc` 在 PATH 中 |
| `CMake version too old` | 升级到 CMake 3.18+ |
| `No CUDA-capable device` | 检查 NVIDIA 驱动安装 |

### 运行时错误

| 错误 | 解决方案 |
|------|----------|
| `CUDA out of memory` | 减小图像尺寸或批次大小 |
| `Invalid kernel size` | 使用奇数卷积核大小 (3, 5, 7, ...) |
| `Image format not supported` | 转换为 RGB/RGBA 或灰度图 |

### 性能问题

1. **首次运行慢**: 首次启动时 CUDA 内核编译是正常的
2. **内存传输**: 减少 CPU-GPU 数据传输
3. **流利用率**: 使用 PipelineProcessor 进行批处理

## 下一步

- [安装指南](installation.zh-CN.md) - 详细的安装选项
- [架构概览](architecture.zh-CN.md) - 了解库的设计
- [性能指南](performance.zh-CN.md) - 优化技巧
- [API 参考](api.zh-CN/) - 完整的 API 文档

## 获取帮助

- 查看 [FAQ](faq.zh-CN.md)
- 搜索 [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- 加入 [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)

---

*本指南针对 Mini-OpenCV v2.0.0 编写*
