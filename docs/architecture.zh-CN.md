---
layout: default
title: 架构说明
description: 深入了解 Mini-OpenCV 架构 - 三层设计、内存管理和 CUDA 流并发机制。
---

# 架构说明

Mini-OpenCV 采用三层架构设计，将面向用户的 API、算子实现和基础设施服务分离。本文档解释设计原则及其对使用方式的影响。

## 三层架构

```
┌─────────────────────────────────────────────────────────────────┐
│                         用户应用层                               │
│                                                                  │
│   ┌──────────────────┐    ┌──────────────────┐                  │
│   │ ImageProcessor   │    │ PipelineProcessor│                  │
│   │   (同步 API)      │    │   (异步 API)      │                  │
│   └────────┬─────────┘    └────────┬─────────┘                  │
│            │                       │                             │
│            ▼                       ▼                             │
│   ┌─────────────────────────────────────────┐                    │
│   │        统一入口: gpu_image_processing    │                    │
│   └─────────────────────────────────────────┘                    │
├─────────────────────────────────────────────────────────────────┤
│                         算子层                                   │
│                                                                  │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │ 像素操作算子  │  │   卷积引擎    │  │   直方图计算  │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │   图像缩放    │  │   形态学处理  │  │    阈值处理   │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │   色彩空间    │  │   几何变换    │  │    图像滤波   │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐                            │
│   │   图像算术    │  │   设备缓冲区  │                            │
│   └──────────────┘  └──────────────┘                            │
├─────────────────────────────────────────────────────────────────┤
│                         基础设施层                               │
│                                                                  │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │  DeviceBuffer│  │MemoryManager │  │StreamManager │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │   GpuImage   │  │   HostImage  │  │   ImageIO    │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐                                               │
│   │   CudaError  │                                               │
│   └──────────────┘                                               │
└─────────────────────────────────────────────────────────────────┘
```

### 各层职责

#### 1. 用户应用层

面向最终用户的高级 API：

| 组件 | 用途 | 同步/异步 |
|-----|------|----------|
| `ImageProcessor` | 一站式图像处理 | 同步 |
| `PipelineProcessor` | 多步骤批处理 | 异步 |

**使用示例：**
```cpp
// 同步操作 - 简单使用
ImageProcessor processor;
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);

// 异步操作 - 基于流的批处理
PipelineProcessor pipeline(4);
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
});
```

#### 2. 算子层

底层 CUDA 内核实现：

| 模块 | 算子 | 关键技术 |
|-----|------|---------|
| `PixelOperator` | 反色、灰度、亮度 | 逐像素并行 |
| `ConvolutionEngine` | 高斯、Sobel、自定义 | 共享内存分块 |
| `HistogramCalculator` | 计算、均衡化 | 原子操作+规约 |
| `ImageResizer` | 双线性、最近邻 | 纹理内存 |
| `Morphology` | 腐蚀、膨胀、开闭 | 结构元素 |
| `Threshold` | 全局、自适应、Otsu | 直方图驱动 |
| `ColorSpace` | RGB/HSV/YUV | 矩阵运算 |
| `Geometric` | 旋转、翻转、仿射 | 双线性插值 |
| `Filters` | 中值、双边、盒式 | 保边滤波 |

算子模块设计为**无状态**且**支持流**：

```cpp
// 无状态 - 调用之间不保留内部状态
namespace ConvolutionEngine {
    void gaussianBlur(const GpuImage& src, GpuImage& dst, 
                      int kernelSize, float sigma, 
                      cudaStream_t stream = nullptr);
}

// 支持流 - 所有操作接受可选的 cudaStream_t
void myOperation(GpuImage& img, cudaStream_t stream = nullptr);
```

#### 3. 基础设施层

内存和执行管理：

| 组件 | 职责 |
|-----|------|
| `DeviceBuffer` | RAII GPU 内存管理 |
| `MemoryManager` | 内存池和分配跟踪 |
| `StreamManager` | CUDA 流生命周期管理 |
| `GpuImage` | GPU 图像容器 |
| `HostImage` | 主机内存图像容器 |
| `CudaError` | 基于异常的错误处理 |

## 内存管理模型

### 基于 RAII 的设计

```cpp
// DeviceBuffer 自动管理 GPU 内存
{
    DeviceBuffer buffer(1024 * 1024);  // 分配 1MB GPU 内存
    // 使用 buffer...
    void* ptr = buffer.data();
    size_t size = buffer.size();
}  // 超出作用域时自动释放
```

### 内存传输模式

```cpp
// 模式1：加载-处理-下载（典型工作流）
HostImage host = loadImage("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage output = processor.downloadImage(result);
saveImage("output.jpg", output);

// 模式2：零拷贝（使用统一内存时）
// 暂不支持，计划在未来版本中添加

// 模式3：使用固定内存批量上传
std::vector<HostImage> batch = loadBatch(100);
std::vector<GpuImage> gpuBatch;
for (auto& host : batch) {
    gpuBatch.push_back(processor.loadFromHost(host));
}
```

### 内存布局

图像以**交错格式**存储（灰度为平面格式）：

```
RGB 图像布局 (width=4, height=3, channels=3):

内存: [R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3]
      [R4 G4 B4 R5 G5 B5 R6 G6 B6 R7 G7 B7]
      [R8 G8 B8 R9 G9 B9 R10 G10 B10 R11 G11 B11]

步长: row_stride = width * channels（对齐填充）
      total_size = row_stride * height
```

## CUDA 流并发

### 流架构

```cpp
// 默认流 (nullptr) - 与主机同步
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, nullptr);

// 显式流 - 异步执行
cudaStream_t stream;
cudaStreamCreate(&stream);
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, stream);
cudaStreamSynchronize(stream);
cudaStreamDestroy(stream);
```

### Pipeline Processor 并发模型

```cpp
PipelineProcessor pipeline(4);  // 4 个并发流

// 内部执行：
// 流 0: [上传] -> [算子1] -> [算子2] -> [下载]
// 流 1: [上传] -> [算子1] -> [算子2] -> [下载]
// 流 2: [上传] -> [算子1] -> [算子2] -> [下载]
// 流 3: [上传] -> [算子1] -> [算子2] -> [下载]
// 时间线重叠最大化 GPU 利用率
```

### 流安全性

在以下情况下所有算子模块是**流安全**的：
- 输入和输出图像不同（非原位）
- 或使用流感知版本（如 `invertInPlace`）

```cpp
// 安全：不同的输入/输出
GpuImage temp;
ConvolutionEngine::gaussianBlur(input, temp, 5, 1.5f, stream);
ConvolutionEngine::sobelEdgeDetection(temp, output, stream);

// 安全：使用流感知方法的原位操作
PixelOperator::invertInPlace(image, stream);
```

## 错误处理策略

### 异常层次结构

```
std::exception
    └── CudaException (自定义异常)
            └── CUDA 运行时错误 + 上下文
```

### 错误处理模式

```cpp
// 模式1：允许异常传播
try {
    GpuImage result = processor.gaussianBlur(image, 5, -1.0f);  // 无效 sigma
} catch (const CudaException& e) {
    std::cerr << "CUDA 错误: " << e.what() << std::endl;
}

// 模式2：预先验证
if (sigma <= 0) {
    throw std::invalid_argument("Sigma 必须为正数");
}
GpuImage result = processor.gaussianBlur(image, kernelSize, sigma);

// 模式3：防御式编程
if (!image.isValid()) {
    return GpuImage();  // 返回空图像
}
```

## 设计原则

1. **关注点分离**：每一层都有单一职责
2. **零拷贝优先**：最小化主机-设备传输
3. **支持流**：所有操作支持异步执行
4. **RAII 内存**：自动资源管理
5. **快速失败**：及早验证输入，出错时抛出异常

## 性能考量

| 操作 | 瓶颈 | 优化方法 |
|-----|------|---------|
| 内核启动 | 开销 | 批量操作，融合内核 |
| 内存传输 | PCIe 带宽 | 使用固定内存，批量上传 |
| 小图像 | 内核开销 | 每个内核处理多个图像 |
| 大图像 | 内存容量 | 分块处理，流式处理 |

## 下一步

- [性能优化](performance.zh-CN) - 实用优化技巧
- [API 参考](api.zh-CN/) - 详细模块文档
- [示例代码](../examples/) - 代码样例

---

*有关架构的问题，请参阅 [常见问题](faq.zh-CN) 或发起讨论*
