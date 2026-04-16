---
layout: default
title: 快速入门
description: 5分钟内上手 Mini-OpenCV - 构建、测试并运行您的第一个 GPU 图像处理程序。
---

# 快速入门指南

在 5 分钟内开始使用 Mini-OpenCV。本指南将引导您构建库、运行测试并处理第一张图像。

## 前置条件检查

开始之前，请确保已安装所需工具：

```bash
# 检查 CUDA 安装
nvcc --version
# 预期输出: Cuda compilation tools, release 11.0 或更高版本

# 检查 CMake 版本
cmake --version
# 预期输出: cmake version 3.18.0 或更高版本

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

# 创建构建目录
mkdir build && cd build

# 配置（启用测试和示例）
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..

# 使用所有可用核心并行构建
make -j$(nproc)
```

构建时间：约 2-5 分钟，具体取决于硬件配置。

## 运行测试

运行测试套件验证安装：

```bash
# 运行所有测试
ctest --output-on-failure

# 或直接运行测试二进制文件
./bin/gpu_image_tests

# 运行特定测试
./bin/gpu_image_tests --gtest_filter=FiltersTest.*
```

所有测试应通过。如有失败，请参阅 [故障排除](#故障排除)。

## 运行示例

运行内置示例查看库的运作：

```bash
# 运行基础示例
./bin/basic_example

# 运行流水线示例（异步处理演示）
./bin/pipeline_example
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

## 编写您的第一个程序

创建一个加载、处理和保存图像的简单程序：

```cpp
// my_program.cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // 检查 CUDA 可用性
    if (!isCudaAvailable()) {
        std::cerr << "CUDA not available!" << std::endl;
        return 1;
    }
    std::cout << getDeviceInfo() << std::endl;

    // 创建处理器
    ImageProcessor processor;

    // 创建测试图像
    HostImage hostImage = ImageUtils::createHostImage(512, 512, 3);
    
    // 填充渐变图案
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            hostImage.at(x, y, 0) = (x * 255) / 512;  // R
            hostImage.at(x, y, 1) = (y * 255) / 512;  // G
            hostImage.at(x, y, 2) = 128;              // B
        }
    }

    // 加载到 GPU
    GpuImage gpuImage = processor.loadFromHost(hostImage);

    // 应用操作
    GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(gpuImage);
    GpuImage gray = processor.toGrayscale(gpuImage);

    // 下载结果
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

### 编译您的程序

```bash
# 使用 CMake（推荐）
# 在 CMakeLists.txt 中添加：
# target_link_libraries(your_target gpu_image_processing)

# 或使用 nvcc 直接编译
nvcc -std=c++17 my_program.cpp \
    -I/path/to/mini-opencv/include \
    -L/path/to/mini-opencv/build/lib \
    -lgpu_image_processing \
    -o my_program
```

## 流水线处理

要实现批处理的最大 GPU 利用率，使用流水线处理器：

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    // 创建流水线，使用 4 个 CUDA 流
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

    // 处理一批图像
    std::vector<HostImage> inputs = /* 您的图像 */;
    std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);

    return 0;
}
```

## 故障排除

### 构建错误

| 错误 | 解决方案 |
|-----|---------|
| `CUDA not found` | 设置 `CUDAToolkit_ROOT` 环境变量，或确保 `nvcc` 在 PATH 中 |
| `CMake version too old` | 升级到 CMake 3.18+ |
| `No CUDA-capable device` | 检查 NVIDIA 驱动是否安装 |

### 运行时错误

| 错误 | 解决方案 |
|-----|---------|
| `CUDA out of memory` | 减小图像尺寸或批次大小 |
| `Invalid kernel size` | 使用奇数大小的卷积核（3, 5, 7, ...） |
| `Image format not supported` | 转换为 RGB/RGBA 或灰度格式 |

### 性能问题

1. **首次运行慢**：CUDA 内核首次编译延迟是正常现象
2. **内存传输**：尽量减少 CPU-GPU 数据传输
3. **流利用率**：批处理时使用 PipelineProcessor

## 下一步

- [安装指南](installation.zh-CN) - 详细安装选项
- [架构说明](architecture.zh-CN) - 理解库的设计
- [性能优化](performance.zh-CN) - 优化技巧
- [API 参考](api.zh-CN/) - 完整 API 文档

## 获取帮助

- 查看 [常见问题](faq.zh-CN)
- 搜索 [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- 加入 [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)

---

*本指南适用于 Mini-OpenCV v2.0.0*
