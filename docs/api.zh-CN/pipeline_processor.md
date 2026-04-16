---
layout: default
title: PipelineProcessor
description: PipelineProcessor API 参考 - 使用 CUDA 流的异步批处理。
---

# PipelineProcessor

使用多个 CUDA 流进行异步批处理，实现最大 GPU 利用率。

## 头文件

```cpp
#include "gpu_image/pipeline_processor.hpp"
```

## 类概览

```cpp
namespace gpu_image {

class PipelineProcessor {
public:
    explicit PipelineProcessor(int numStreams);
    ~PipelineProcessor();

    // 添加处理步骤
    void addStep(std::function<void(GpuImage&, cudaStream_t)> step);
    
    // 处理单张图像
    HostImage processHost(const HostImage& input);
    
    // 批处理
    std::vector<HostImage> processBatchHost(const std::vector<HostImage>& inputs);
    
    // 同步
    void synchronize();
    
    // 信息
    int stepCount() const;
    int streamCount() const;
};

}
```

## 构造函数

### PipelineProcessor()

```cpp
explicit PipelineProcessor(int numStreams);
```

**参数：**
- `numStreams` - 用于并发处理的 CUDA 流数量（通常 4-8）

**示例：**
```cpp
PipelineProcessor pipeline(4);  // 4 个并发流
```

## 添加步骤

### addStep()

向流水线添加处理步骤。

```cpp
void addStep(std::function<void(GpuImage&, cudaStream_t)> step);
```

**参数：**
- `step` - 使用提供的流转换图像的函数

**示例：**
```cpp
pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
    GpuImage temp;
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, stream);
    img = std::move(temp);
});
```

## 处理

### processHost()

通过流水线处理单张图像。

```cpp
HostImage processHost(const HostImage& input);
```

### processBatchHost()

使用并发流处理多张图像。

```cpp
std::vector<HostImage> processBatchHost(const std::vector<HostImage>& inputs);
```

**返回：** 处理后的图像向量，按输入顺序

**示例：**
```cpp
std::vector<HostImage> inputs = loadImages();
std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
```

## 完整示例

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    // 创建带 4 个流的流水线
    PipelineProcessor pipeline(4);
    
    // 添加处理步骤
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        GpuImage temp;
        PixelOperator::adjustBrightness(img, temp, 20, s);
        img = std::move(temp);
    });
    
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        GpuImage temp;
        ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
        img = std::move(temp);
    });
    
    pipeline.addStep([](GpuImage& img, cudaStream_t s) {
        PixelOperator::invertInPlace(img, s);
    });
    
    // 批处理
    std::vector<HostImage> inputs = loadBatch(100);
    auto results = pipeline.processBatchHost(inputs);
    
    std::cout << "已处理 " << results.size() << " 张图像" << std::endl;
    
    return 0;
}
```

## 另请参阅

- [ImageProcessor](image_processor) - 用于简单同步处理
