---
layout: default
title: 性能优化
description: Mini-OpenCV 优化指南 - GPU 选择、内存管理和最佳实践，实现最大吞吐量。
---

# 性能优化指南

本指南提供使用 Mini-OpenCV 时最大化性能的实用建议，从硬件选择到代码级优化。

## 硬件选择

### GPU 架构对比

| 架构 | 计算能力 | 内存带宽 | 最佳用途 | 示例 GPU |
|------|---------|----------|---------|----------|
| Turing | 75 | ~616 GB/s | 平衡型 | RTX 2080 Ti, T4 |
| Ampere | 80/86 | ~936 GB/s | 计算型 | A100, RTX 3090 |
| Ada Lovelace | 89 | ~1008 GB/s | 实时处理 | RTX 4090, L4 |
| Hopper | 90 | ~3 TB/s | AI/HPC | H100 |

### 内存需求估算

使用以下公式估算内存使用：

```
GPU 内存 = width × height × channels × (operations + 2) × 1.5

示例：4K (3840×2160) 图像，3 通道
= 3840 × 2160 × 3 × 4 × 1.5 ≈ 149 MB 每级联
```

### GPU 选择矩阵

| 使用场景 | 推荐 | 最低配置 | 说明 |
|---------|------|---------|------|
| 移动/边缘设备 | - | T4, Jetson | 功耗受限 |
| 桌面应用 | RTX 4070+ | RTX 3060 | 性价比 |
| 工作站 | RTX 4090 | RTX 4080 | 最大吞吐 |
| 数据中心 | A100, H100 | A10 | 企业级支持 |

## 内存优化

### 1. 减少主机-设备传输

```cpp
// 不推荐：多次传输
for (int i = 0; i < N; ++i) {
    GpuImage gpu = processor.loadFromHost(images[i]);  // 传输
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
    HostImage output = processor.downloadImage(result);  // 传输
    results[i] = output;
}

// 推荐：批量上传
PipelineProcessor pipeline(4);
for (int i = 0; i < N; ++i) {
    pipeline.processHost(images[i]);
}
results = pipeline.getResults();
```

### 2. 使用固定（页锁定）内存

```cpp
// 固定内存支持异步传输
cudaHostAlloc(&pinnedPtr, size, cudaHostAllocDefault);

// Mini-OpenCV 通过 PipelineProcessor 内部处理此优化
```

### 3. 内存池分配

```cpp
// 复用 GPU 内存，避免分配开销
DeviceBuffer buffer(maxSize);
for (auto& image : images) {
    buffer.resize(image.size());
    // 处理...
}
```

## 流并发

### 最佳流数量

最佳 CUDA 流数量取决于您的 GPU：

```cpp
// 经验法则：流数 = SM 数 / 4
// RTX 4090 (128 SM): 32 个流
// RTX 3080 (68 SM): 17 个流
// A100 (108 SM): 27 个流

// 但通常 4-8 个流效果最好：
PipelineProcessor pipeline(4);  // 良好的默认值
```

### 基准测试结果

| 流数 | 10 张图像 (512×512) | 加速比 | 效率 |
|------|-------------------|--------|------|
| 1 | 120 ms | 1.0x | 100% |
| 2 | 75 ms | 1.6x | 80% |
| 4 | 52 ms | 2.3x | 58% |
| 8 | 45 ms | 2.7x | 34% |
| 16 | 42 ms | 2.9x | 18% |

*在 RTX 4080 上使用高斯模糊流水线测试*

### 流排序

```cpp
// 同一流中的操作自动排序
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp1, temp2;
    // 这些按顺序执行
    ConvolutionEngine::gaussianBlur(img, temp1, 3, 1.0f, s);
    PixelOperator::invert(temp1, temp2, s);
    Geometric::rotate(temp2, img, 45.0f, s);
});
```

## 内核融合

### 何时融合

在以下情况下融合操作：
- 操作为简单逐像素操作
- 不需要中间结果
- 内存带宽是瓶颈

### 示例：融合操作

```cpp
// 不推荐：多次内核启动
GpuImage temp1, temp2;
PixelOperator::invert(input, temp1, stream);
PixelOperator::adjustBrightness(temp1, temp2, 50, stream);
ColorSpace::toGrayscale(temp2, output, stream);

// 推荐：单次融合操作（如果已实现）
// PixelOperator::invertBrightnessGrayscale(input, output, 50, stream);
```

### 内置融合操作

| 融合操作 | 说明 | 加速 |
|---------|------|------|
| `histogramEqualize` | 均衡化 + 归一化 | 1.3x |
| `sobelEdgeDetection` | Sobel X + Y + 幅值 | 1.4x |

## 算法选择

### 卷积核大小

| 卷积核大小 | 相对速度 | 用途 |
|-----------|---------|------|
| 3×3 | 1.0x | 快速模糊、锐化 |
| 5×5 | 2.2x | 标准模糊 |
| 7×7 | 4.1x | 重度模糊 |
| 9×9 | 6.8x | 最大模糊 |

*更大的卷积核需要更多寄存器和共享内存*

### 插值方法

| 方法 | 质量 | 速度 | 用途 |
|------|------|------|------|
| 最近邻 | 低 | 最快 | 预览、缩略图 |
| 双线性 | 好 | 快 | 通用 |
| 双三次 | 更好 | 慢 | 高质量缩放 |

Mini-OpenCV 目前支持最近邻和双线性插值。

## 性能分析

### 内置计时

```cpp
#include <chrono>

class Timer {
    std::chrono::high_resolution_clock::time_point start_;
public:
    void start() { start_ = std::chrono::high_resolution_clock::now(); }
    double elapsedMs() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start_).count();
    }
};

Timer timer;
timer.start();
GpuImage result = processor.gaussianBlur(image, 5, 1.5f);
cudaDeviceSynchronize();  // 确保 GPU 完成
std::cout << "时间: " << timer.elapsedMs() << " ms" << std::endl;
```

### Nsight Systems

```bash
# 使用 Nsight Systems 分析
nsys profile -o report.qdrep ./your_program

# 在 Nsight Systems GUI 中查看
nsys-ui report.qdrep
```

### 关键监控指标

| 指标 | 目标 | 性能不佳时的措施 |
|------|------|----------------|
| GPU 利用率 | >80% | 增加批次大小 |
| 内存带宽 | >70% | 检查数据局部性 |
| 内核占用率 | >60% | 检查块大小 |
| PCIe 带宽 | 数据传输 <20% | 批量传输 |

## 与 OpenCV 对比

### 性能对比（GPU vs CPU）

| 操作 | OpenCV CPU | Mini-OpenCV GPU | 加速比 |
|------|------------|-----------------|--------|
| 高斯模糊 (5×5) | 12 ms | 0.3 ms | 40x |
| Sobel 边缘检测 | 8 ms | 0.2 ms | 40x |
| 直方图均衡化 | 15 ms | 0.5 ms | 30x |
| 缩放 (双线性) | 6 ms | 0.15 ms | 40x |
| RGB→灰度 | 2 ms | 0.05 ms | 40x |

*测试条件：1920×1080 图像，Intel i9-12900K vs RTX 4080*

### 何时使用哪个

| 使用场景 | 推荐方案 |
|---------|----------|
| 实时视频 | Mini-OpenCV (GPU) |
| 批处理 | Mini-OpenCV (流水线) |
| 单图像，低延迟关键 | OpenCV (CPU) - 无 GPU 预热 |
| 嵌入式系统 | OpenCV (CPU) - 无 GPU 可用 |

## 最佳实践

### 1. 预热

```cpp
// 第一次 CUDA 调用包含 JIT 编译开销
GpuImage warmup = processor.gaussianBlur(dummy, 5, 1.5f);
cudaDeviceSynchronize();  // 强制编译
// 后续调用更快
```

### 2. 图像尺寸对齐

```cpp
// 将图像尺寸对齐到 32 以优化内存访问
int alignedWidth = ((width + 31) / 32) * 32;
int alignedHeight = ((height + 31) / 32) * 32;
```

### 3. 批处理

```cpp
// 尽可能处理多张图像
PipelineProcessor pipeline(4);
auto results = pipeline.processBatchHost(images);
```

### 4. 分辨率级联

```cpp
// 对于预览质量，先降采样
GpuImage small = processor.resize(image, width/4, height/4);
GpuImage processed = processor.sobelEdgeDetection(small);
GpuImage fullSize = processor.resize(processed, width, height);
```

## 性能问题故障排除

| 症状 | 可能原因 | 解决方案 |
|------|----------|----------|
| GPU 利用率低 | 图像小或批次小 | 增加批次大小或使用分块 |
| PCIe 瓶颈 | 主机-设备传输过多 | 使用 PipelineProcessor 或批量上传 |
| 内核延迟 | 许多小操作 | 融合内核或使用更大分块 |
| 内存错误 | 图像太大 | 降低分辨率或使用分块处理 |

## 下一步

- [API 参考](api.zh-CN/) - 详细函数文档
- [示例代码](../examples/) - 工作代码样例
- 运行基准测试: `cmake --build build --target gpu_image_benchmark`

---

*有关性能问题，请参阅 [常见问题](faq.zh-CN) 或发起讨论*
