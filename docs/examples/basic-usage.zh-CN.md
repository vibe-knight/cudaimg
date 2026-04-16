---
layout: default
title: 基础使用示例
description: Mini-OpenCV 基础使用示例 - 加载、处理和保存图像。
---

# 基础使用示例

完整的图像加载、操作应用和结果保存的演示。

## 完整代码

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // 步骤1：检查 CUDA 可用性
    if (!isCudaAvailable()) {
        std::cerr << "CUDA 不可用！" << std::endl;
        return 1;
    }
    std::cout << "使用: " << getDeviceInfo() << std::endl;

    // 步骤2：创建处理器
    ImageProcessor processor;

    // 步骤3：创建测试图像
    HostImage input = ImageUtils::createHostImage(512, 512, 3);
    for (int y = 0; y < 512; ++y) {
        for (int x = 0; x < 512; ++x) {
            input.at(x, y, 0) = (x * 255) / 512;  // R 渐变
            input.at(x, y, 1) = (y * 255) / 512;  // G 渐变
            input.at(x, y, 2) = 128;               // B 常量
        }
    }

    // 步骤4：上传到 GPU
    GpuImage gpu = processor.loadFromHost(input);

    // 步骤5：应用操作
    GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(blurred);

    // 步骤6：下载结果
    HostImage output = processor.downloadImage(edges);

    // 步骤7：验证输出
    std::cout << "输出: " << output.width << "x" << output.height 
              << "x" << output.channels << std::endl;

    return 0;
}
```

## 分步讲解

### 1. CUDA 检查

处理前始终验证 CUDA 是否可用。

### 2. 创建处理器

`ImageProcessor` 管理 GPU 资源并提供同步操作。

### 3. 创建图像

`HostImage` 将数据存储在 CPU RAM 中，并提供方便的像素访问器。

### 4. GPU 上传

`loadFromHost()` 通过 PCIe 将数据复制到 GPU 内存。

### 5. 处理

操作在 GPU 上运行并自动同步。

### 6. 下载

`downloadImage()` 将结果复制回 CPU 内存。

## 编译

```bash
# 使用 CMake
target_link_libraries(my_app gpu_image::gpu_image_processing)

# 直接编译
nvcc -std=c++17 example.cpp -I/path/to/mini-opencv/include \
    -L/path/to/mini-opencv/build/lib -lgpu_image_processing -o example
```
