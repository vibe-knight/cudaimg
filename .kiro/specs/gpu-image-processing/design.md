# Design Document: GPU Image Processing Library

## Overview

本设计文档描述了一个基于 CUDA 的高性能图像处理库的架构和实现细节。该库采用分层架构，将内存管理、计算内核和高级 API 分离，以实现良好的可维护性和可扩展性。

核心设计原则：
- **零拷贝优化**：尽可能减少 Host-Device 数据传输
- **内存复用**：通过内存池减少频繁的内存分配开销
- **异步执行**：利用 CUDA Streams 实现计算与传输的重叠
- **模块化设计**：各功能模块独立，便于扩展和测试

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      User Application                        │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    High-Level API Layer                      │
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────────────┐   │
│  │ ImageLoader │ │ ImageSaver  │ │ PipelineProcessor   │   │
│  └─────────────┘ └─────────────┘ └─────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Processing Layer                           │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐    │
│  │PixelOperator│ │ConvolutionEng│ │HistogramCalc     │    │
│  └──────────────┘ └──────────────┘ └──────────────────────┘    │
│  ┌──────────────┐                                            │
│  │ ImageResizer │                                            │
│  └──────────────┘                                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Memory Management Layer                    │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐    │
│  │MemoryManager│ │ DeviceBuffer │ │ StreamManager    │    │
│  └──────────────┘ └──────────────┘ └──────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      CUDA Runtime                            │
└─────────────────────────────────────────────────────────────┘
```

## Components and Interfaces

### 1. Memory Management Layer

#### DeviceBuffer
管理单个 GPU 内存缓冲区的生命周期。

```cpp
class DeviceBuffer {
public:
    // 构造函数：分配指定大小的 Device 内存
    explicit DeviceBuffer(size_t size);
    
    // 析构函数：自动释放 Device 内存
    ~DeviceBuffer();
    
    // 禁用拷贝，启用移动语义
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;
    
    // 从 Host 复制数据到 Device
    void copyFromHost(const void* hostPtr, size_t size);
    
    // 从 Device 复制数据到 Host
    void copyToHost(void* hostPtr, size_t size) const;
    
    // 异步版本（使用指定的 stream）
    void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
    void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;
    
    // 获取 Device 指针
    void* data();
    const void* data() const;
    
    // 获取缓冲区大小
    size_t size() const;

private:
    void* devicePtr_;
    size_t size_;
};
```

#### MemoryManager
管理内存分配和传输，提供内存池功能。

```cpp
class MemoryManager {
public:
    static MemoryManager& instance();
    
    // 分配 Device 内存（可能从池中获取）
    DeviceBuffer allocate(size_t size);
    
    // 释放内存回池
    void deallocate(DeviceBuffer&& buffer);
    
    // 清空内存池
    void clearPool();
    
    // 获取当前 GPU 内存使用统计
    struct MemoryStats {
        size_t totalAllocated;
        size_t poolSize;
        size_t peakUsage;
    };
    MemoryStats getStats() const;

private:
    MemoryManager() = default;
    std::unordered_map<size_t, std::vector<void*>> memoryPool_;
    std::mutex mutex_;
};
```

#### StreamManager
管理 CUDA Streams 用于异步操作。

```cpp
class StreamManager {
public:
    static StreamManager& instance();
    
    // 获取一个可用的 stream
    cudaStream_t acquireStream();
    
    // 释放 stream 回池
    void releaseStream(cudaStream_t stream);
    
    // 同步所有 streams
    void synchronizeAll();
    
    // 同步指定 stream
    void synchronize(cudaStream_t stream);

private:
    StreamManager();
    ~StreamManager();
    std::vector<cudaStream_t> streams_;
    std::queue<cudaStream_t> availableStreams_;
    std::mutex mutex_;
};
```

### 2. Processing Layer

#### GpuImage
GPU 上的图像表示。

```cpp
struct GpuImage {
    DeviceBuffer buffer;
    int width;
    int height;
    int channels;  // 1 for grayscale, 3 for RGB, 4 for RGBA
    
    size_t pitch() const { return width * channels; }
    size_t totalBytes() const { return width * height * channels; }
};
```

#### PixelOperator
像素级操作的实现。

```cpp
class PixelOperator {
public:
    // 图像反色
    // 对每个像素执行: output[i] = 255 - input[i]
    static void invert(const GpuImage& input, GpuImage& output, 
                       cudaStream_t stream = nullptr);
    
    // RGB 转灰度
    // 使用公式: gray = 0.299*R + 0.587*G + 0.114*B
    static void toGrayscale(const GpuImage& input, GpuImage& output,
                            cudaStream_t stream = nullptr);
    
    // 亮度调整
    // 对每个像素执行: output[i] = clamp(input[i] + offset, 0, 255)
    static void adjustBrightness(const GpuImage& input, GpuImage& output,
                                  int offset, cudaStream_t stream = nullptr);
};
```

#### ConvolutionEngine
卷积操作的实现，使用 Shared Memory 优化。

```cpp
class ConvolutionEngine {
public:
    // 通用卷积操作
    static void convolve(const GpuImage& input, GpuImage& output,
                         const float* kernel, int kernelSize,
                         cudaStream_t stream = nullptr);
    
    // 高斯模糊
    // kernelSize 必须为奇数，sigma 控制模糊程度
    static void gaussianBlur(const GpuImage& input, GpuImage& output,
                             int kernelSize, float sigma,
                             cudaStream_t stream = nullptr);
    
    // Sobel 边缘检测
    // 返回梯度幅值图像
    static void sobelEdgeDetection(const GpuImage& input, GpuImage& output,
                                   cudaStream_t stream = nullptr);

private:
    // 生成高斯核
    static std::vector<float> generateGaussianKernel(int size, float sigma);
    
    // Sobel 核（预定义）
    static constexpr float SOBEL_X[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    static constexpr float SOBEL_Y[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
};
```

#### HistogramCalculator
直方图计算，使用原子操作和并行规约。

```cpp
class HistogramCalculator {
public:
    static constexpr int NUM_BINS = 256;
    
    // 计算灰度直方图
    // 输入必须是单通道灰度图像
    static std::array<int, NUM_BINS> calculate(const GpuImage& input,
                                                cudaStream_t stream = nullptr);
    
    // 计算 RGB 各通道直方图
    static std::array<std::array<int, NUM_BINS>, 3> calculateRGB(
        const GpuImage& input, cudaStream_t stream = nullptr);
};
```

#### ImageResizer
图像缩放，使用双线性插值。

```cpp
class ImageResizer {
public:
    // 双线性插值缩放
    static void resize(const GpuImage& input, GpuImage& output,
                       int newWidth, int newHeight,
                       cudaStream_t stream = nullptr);

private:
    // 双线性插值计算
    // 对于目标位置 (x, y)，计算源图像中对应的浮点坐标
    // 然后使用周围 4 个像素进行加权插值
    static __device__ float bilinearInterpolate(
        const unsigned char* src, int srcWidth, int srcHeight,
        float srcX, float srcY, int channel);
};
```

### 3. High-Level API Layer

#### ImageProcessor
主要的用户接口类。

```cpp
class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();
    
    // 从文件加载图像到 GPU
    GpuImage loadImage(const std::string& filepath);
    
    // 从内存加载图像到 GPU
    GpuImage loadFromMemory(const unsigned char* data, 
                            int width, int height, int channels);
    
    // 将 GPU 图像保存到文件
    void saveImage(const GpuImage& image, const std::string& filepath);
    
    // 将 GPU 图像复制到 Host 内存
    std::vector<unsigned char> downloadImage(const GpuImage& image);
    
    // 像素操作
    GpuImage invert(const GpuImage& input);
    GpuImage toGrayscale(const GpuImage& input);
    GpuImage adjustBrightness(const GpuImage& input, int offset);
    
    // 卷积操作
    GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& input);
    
    // 直方图
    std::array<int, 256> histogram(const GpuImage& input);
    
    // 缩放
    GpuImage resize(const GpuImage& input, int newWidth, int newHeight);
};
```

#### PipelineProcessor
流水线处理器，用于高效处理多张图像。

```cpp
class PipelineProcessor {
public:
    explicit PipelineProcessor(int numStreams = 3);
    ~PipelineProcessor();
    
    // 定义处理步骤的函数类型
    using ProcessingStep = std::function<void(GpuImage&, cudaStream_t)>;
    
    // 添加处理步骤
    void addStep(ProcessingStep step);
    
    // 清空处理步骤
    void clearSteps();
    
    // 处理单张图像（使用流水线）
    GpuImage process(const GpuImage& input);
    
    // 批量处理多张图像（流水线并行）
    std::vector<GpuImage> processBatch(const std::vector<GpuImage>& inputs);
    
    // 同步等待所有操作完成
    void synchronize();

private:
    int numStreams_;
    std::vector<cudaStream_t> streams_;
    std::vector<ProcessingStep> steps_;
};
```

## Data Models

### Image Data Format

```cpp
// Host 端图像数据布局
// 数据按行优先存储，每行连续存储所有通道
// 例如 RGB 图像: R0G0B0 R1G1B1 R2G2B2 ...

struct HostImage {
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
    
    // 访问像素
    unsigned char& at(int x, int y, int c) {
        return data[(y * width + x) * channels + c];
    }
    
    const unsigned char& at(int x, int y, int c) const {
        return data[(y * width + x) * channels + c];
    }
};
```

### Error Handling

```cpp
// CUDA 错误包装
class CudaException : public std::runtime_error {
public:
    CudaException(cudaError_t error, const char* file, int line);
    cudaError_t errorCode() const { return error_; }
    
private:
    cudaError_t error_;
};

// 错误检查宏
#define CUDA_CHECK(call) do { \
    cudaError_t error = call; \
    if (error != cudaSuccess) { \
        throw CudaException(error, __FILE__, __LINE__); \
    } \
} while(0)

// 结果类型（用于不抛异常的 API）
template<typename T>
class Result {
public:
    static Result<T> ok(T value);
    static Result<T> error(std::string message);
    
    bool isOk() const;
    bool isError() const;
    T& value();
    const std::string& errorMessage() const;
    
private:
    std::variant<T, std::string> data_;
};
```

### Configuration

```cpp
struct ProcessorConfig {
    // 内存池配置
    size_t maxPoolSize = 512 * 1024 * 1024;  // 512 MB
    bool enableMemoryPool = true;
    
    // Stream 配置
    int defaultNumStreams = 4;
    
    // 卷积配置
    enum class BorderMode { Zero, Mirror, Replicate };
    BorderMode defaultBorderMode = BorderMode::Zero;
    
    // 性能配置
    int blockSizeX = 16;
    int blockSizeY = 16;
};
```



## Correctness Properties

*正确性属性是指在系统所有有效执行中都应该保持为真的特征或行为。属性是人类可读规范与机器可验证正确性保证之间的桥梁。*

基于需求分析，以下是本系统需要验证的核心正确性属性：

### Property 1: 数据传输往返一致性 (Round-Trip)

*For any* 有效的图像数据，将其从 Host 上传到 Device，然后从 Device 下载回 Host，得到的数据应该与原始数据完全相同。

**Validates: Requirements 1.1, 1.2**

### Property 2: 反色操作自逆性 (Involution)

*For any* 有效的图像，对其执行两次反色操作后，结果应该与原始图像完全相同。即 `invert(invert(image)) == image`。

**Validates: Requirements 2.1**

### Property 3: 灰度化公式正确性 (Invariant)

*For any* 有效的 RGB 图像中的任意像素 (R, G, B)，灰度化后的输出值应该等于 `round(0.299*R + 0.587*G + 0.114*B)`，且结果在 [0, 255] 范围内。

**Validates: Requirements 2.2**

### Property 4: 亮度调整范围不变性 (Invariant)

*For any* 有效的图像和任意亮度偏移量，亮度调整后的所有像素值应该在 [0, 255] 范围内。

**Validates: Requirements 2.3**

### Property 5: 卷积操作与参考实现一致性 (Model-Based)

*For any* 有效的图像和卷积核，GPU 卷积操作的结果应该与 CPU 参考实现的结果在数值精度范围内一致（允许 ±1 的舍入误差）。

**Validates: Requirements 3.1, 3.2**

### Property 6: 边界处理正确性 (Invariant)

*For any* 使用零填充的卷积操作，边界像素的计算应该将图像外部的像素视为 0。

**Validates: Requirements 3.4**

### Property 7: 直方图总和不变性 (Invariant)

*For any* 有效的灰度图像，其直方图所有 bin 的总和应该等于图像的总像素数（width × height）。

**Validates: Requirements 4.1**

### Property 8: 缩放操作近似可逆性 (Approximate Round-Trip)

*For any* 有效的图像，先放大再缩小回原尺寸（或先缩小再放大），结果应该与原图像在视觉上相似（PSNR > 30dB 或 SSIM > 0.9）。

**Validates: Requirements 5.1**

### Property 9: 流水线处理结果一致性 (Confluence)

*For any* 有效的图像和处理步骤序列，使用流水线处理和使用同步处理应该产生相同的结果。

**Validates: Requirements 6.4**

## Error Handling

### 错误类型分类

```cpp
enum class ErrorCode {
    // 内存错误
    OutOfMemory,
    InvalidMemoryAccess,
    
    // 参数错误
    InvalidImageSize,
    InvalidKernelSize,
    InvalidChannelCount,
    NullPointer,
    
    // CUDA 运行时错误
    CudaDriverError,
    CudaKernelLaunchError,
    CudaSyncError,
    
    // 文件 I/O 错误
    FileNotFound,
    FileReadError,
    FileWriteError,
    UnsupportedFormat
};
```

### 错误处理策略

1. **参数验证**：所有公共 API 在执行前验证参数
   - 图像尺寸必须 > 0
   - 卷积核大小必须为奇数且 >= 3
   - 通道数必须为 1、3 或 4
   - 指针不能为空

2. **CUDA 错误检查**：每个 CUDA API 调用后检查错误
   - 使用 `CUDA_CHECK` 宏包装所有 CUDA 调用
   - 错误信息包含文件名、行号和 CUDA 错误描述

3. **资源清理**：使用 RAII 确保资源释放
   - `DeviceBuffer` 析构函数自动释放内存
   - 异常安全：即使发生异常也不泄漏资源

4. **错误恢复**：错误后系统保持一致状态
   - 失败的操作不修改输入数据
   - 部分完成的操作会回滚

## Testing Strategy

### 测试框架选择

- **单元测试框架**: Google Test (gtest)
- **属性测试框架**: RapidCheck (C++ 属性测试库)
- **性能测试**: Google Benchmark

### 双重测试方法

本项目采用单元测试和属性测试相结合的方法：

1. **单元测试**：验证特定示例和边界情况
   - 测试已知输入的预期输出
   - 测试边界条件（空图像、单像素图像、最大尺寸图像）
   - 测试错误处理路径

2. **属性测试**：验证跨所有输入的通用属性
   - 每个属性测试运行至少 100 次迭代
   - 使用随机生成的图像数据
   - 测试数学不变性和往返属性

### 属性测试配置

```cpp
// RapidCheck 配置
rc::detail::Configuration config;
config.max_success = 100;  // 最少 100 次成功迭代
config.max_size = 50;      // 最大生成尺寸

// 图像生成器
rc::Gen<HostImage> arbitraryImage() {
    return rc::gen::build<HostImage>(
        rc::gen::set(&HostImage::width, rc::gen::inRange(1, 1024)),
        rc::gen::set(&HostImage::height, rc::gen::inRange(1, 1024)),
        rc::gen::set(&HostImage::channels, rc::gen::element(1, 3, 4)),
        // data 根据尺寸生成
    );
}
```

### 测试标注格式

每个属性测试必须包含以下注释：

```cpp
// Feature: gpu-image-processing, Property 1: 数据传输往返一致性
// Validates: Requirements 1.1, 1.2
RC_GTEST_PROP(MemoryTransfer, RoundTrip, ()) {
    // 测试实现
}
```

### 测试覆盖矩阵

| 需求 | 单元测试 | 属性测试 |
|------|----------|----------|
| 1.1, 1.2 数据传输 | ✓ 基本传输 | ✓ Property 1 |
| 2.1 反色 | ✓ 已知图像 | ✓ Property 2 |
| 2.2 灰度化 | ✓ 已知像素 | ✓ Property 3 |
| 2.3 亮度调整 | ✓ 边界值 | ✓ Property 4 |
| 3.1, 3.2 卷积 | ✓ 小图像 | ✓ Property 5 |
| 3.4 边界处理 | ✓ 边界像素 | ✓ Property 6 |
| 4.1 直方图 | ✓ 已知分布 | ✓ Property 7 |
| 5.1 缩放 | ✓ 整数倍缩放 | ✓ Property 8 |
| 6.4 流水线 | ✓ 多图像 | ✓ Property 9 |
| 7.x 错误处理 | ✓ 各种错误 | - |
