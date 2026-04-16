# Design Document: GPU Image Processing Library

[← Back to Specs](.) | [Requirements](requirements.md) | [Tasks](tasks.md)

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Components and Interfaces](#components-and-interfaces)
  - [Memory Management Layer](#1-memory-management-layer)
  - [Processing Layer](#2-processing-layer)
  - [High-Level API Layer](#3-high-level-api-layer)
- [Data Models](#data-models)
- [Correctness Properties](#correctness-properties)
- [Error Handling](#error-handling)
- [Testing Strategy](#testing-strategy)

---

## Overview

本设计文档描述了一个基于 CUDA 的高性能图像处理库的架构和实现细节。该库采用分层架构，将内存管理、计算内核和高级 API 分离，以实现良好的可维护性和可扩展性。

**Core Design Principles:**

| Principle | Description |
|-----------|-------------|
| **Zero-Copy Optimization** | Minimize Host-Device data transfers |
| **Memory Reuse** | Reduce allocation overhead via memory pool |
| **Async Execution** | Overlap computation and transfer via CUDA Streams |
| **Modular Design** | Independent modules for easy extension and testing |

---

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
│  │PixelOperator │ │ConvolutionEng│ │HistogramCalc     │    │
│  └──────────────┘ └──────────────┘ └──────────────────────┘│
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐    │
│  │ ImageResizer │ │  Morphology  │ │   Threshold      │    │
│  └──────────────┘ └──────────────┘ └──────────────────┘    │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐    │
│  │ ColorSpace   │ │  Geometric   │ │    Filters       │    │
│  └──────────────┘ └──────────────┘ └──────────────────┘    │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                   Memory Management Layer                    │
│  ┌──────────────┐ ┌──────────────┐ ┌──────────────────┐    │
│  │MemoryManager │ │ DeviceBuffer │ │ StreamManager    │    │
│  └──────────────┘ └──────────────┘ └──────────────────────┘│
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                      CUDA Runtime                            │
└─────────────────────────────────────────────────────────────┘
```

---

## Components and Interfaces

### 1. Memory Management Layer

#### DeviceBuffer

管理单个 GPU 内存缓冲区的生命周期。

```cpp
class DeviceBuffer {
public:
    // Constructor: allocate specified size of Device memory
    explicit DeviceBuffer(size_t size);

    // Destructor: automatically release Device memory
    ~DeviceBuffer();

    // Disable copy, enable move semantics
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;

    // Copy data from Host to Device
    void copyFromHost(const void* hostPtr, size_t size);

    // Copy data from Device to Host
    void copyToHost(void* hostPtr, size_t size) const;

    // Async versions (using specified stream)
    void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
    void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;

    // Get Device pointer
    void* data();
    const void* data() const;

    // Get buffer size
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

    // Allocate Device memory (possibly from pool)
    DeviceBuffer allocate(size_t size);

    // Release memory back to pool
    void deallocate(DeviceBuffer&& buffer);

    // Clear memory pool
    void clearPool();

    // Get current GPU memory usage statistics
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

    // Acquire an available stream
    cudaStream_t acquireStream();

    // Release stream back to pool
    void releaseStream(cudaStream_t stream);

    // Synchronize all streams
    void synchronizeAll();

    // Synchronize specified stream
    void synchronize(cudaStream_t stream);

private:
    StreamManager();
    ~StreamManager();
    std::vector<cudaStream_t> streams_;
    std::queue<cudaStream_t> availableStreams_;
    std::mutex mutex_;
};
```

---

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
    // Image invert
    // For each pixel: output[i] = 255 - input[i]
    static void invert(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);

    // RGB to grayscale
    // Using formula: gray = 0.299*R + 0.587*G + 0.114*B
    static void toGrayscale(const GpuImage& input, GpuImage& output,
                            cudaStream_t stream = nullptr);

    // Brightness adjustment
    // For each pixel: output[i] = clamp(input[i] + offset, 0, 255)
    static void adjustBrightness(const GpuImage& input, GpuImage& output,
                                  int offset, cudaStream_t stream = nullptr);
};
```

#### ConvolutionEngine

卷积操作的实现，使用 Shared Memory 优化。

```cpp
class ConvolutionEngine {
public:
    // General convolution operation
    static void convolve(const GpuImage& input, GpuImage& output,
                         const float* kernel, int kernelSize,
                         cudaStream_t stream = nullptr);

    // Gaussian blur
    // kernelSize must be odd, sigma controls blur intensity
    static void gaussianBlur(const GpuImage& input, GpuImage& output,
                             int kernelSize, float sigma,
                             cudaStream_t stream = nullptr);

    // Sobel edge detection
    // Returns gradient magnitude image
    static void sobelEdgeDetection(const GpuImage& input, GpuImage& output,
                                   cudaStream_t stream = nullptr);

private:
    // Generate Gaussian kernel
    static std::vector<float> generateGaussianKernel(int size, float sigma);

    // Sobel kernels (predefined)
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

    // Calculate grayscale histogram
    // Input must be single-channel grayscale image
    static std::array<int, NUM_BINS> calculate(const GpuImage& input,
                                                cudaStream_t stream = nullptr);

    // Calculate RGB channel histograms
    static std::array<std::array<int, NUM_BINS>, 3> calculateRGB(
        const GpuImage& input, cudaStream_t stream = nullptr);
};
```

#### ImageResizer

图像缩放，使用双线性插值。

```cpp
class ImageResizer {
public:
    // Bilinear interpolation scaling
    static void resize(const GpuImage& input, GpuImage& output,
                       int newWidth, int newHeight,
                       cudaStream_t stream = nullptr);

private:
    // Bilinear interpolation calculation
    // For target position (x, y), calculate corresponding floating-point coordinates in source
    // Then use surrounding 4 pixels for weighted interpolation
    static __device__ float bilinearInterpolate(
        const unsigned char* src, int srcWidth, int srcHeight,
        float srcX, float srcY, int channel);
};
```

---

### 3. High-Level API Layer

#### ImageProcessor

主要的用户接口类。

```cpp
class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();

    // Load image from file to GPU
    GpuImage loadImage(const std::string& filepath);

    // Load image from memory to GPU
    GpuImage loadFromMemory(const unsigned char* data,
                            int width, int height, int channels);

    // Save GPU image to file
    void saveImage(const GpuImage& image, const std::string& filepath);

    // Copy GPU image to Host memory
    std::vector<unsigned char> downloadImage(const GpuImage& image);

    // Pixel operations
    GpuImage invert(const GpuImage& input);
    GpuImage toGrayscale(const GpuImage& input);
    GpuImage adjustBrightness(const GpuImage& input, int offset);

    // Convolution operations
    GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& input);

    // Histogram
    std::array<int, 256> histogram(const GpuImage& input);

    // Scaling
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

    // Processing step function type
    using ProcessingStep = std::function<void(GpuImage&, cudaStream_t)>;

    // Add processing step
    void addStep(ProcessingStep step);

    // Clear processing steps
    void clearSteps();

    // Process single image (using pipeline)
    GpuImage process(const GpuImage& input);

    // Batch process multiple images (pipeline parallelism)
    std::vector<GpuImage> processBatch(const std::vector<GpuImage>& inputs);

    // Wait for all operations to complete
    void synchronize();

private:
    int numStreams_;
    std::vector<cudaStream_t> streams_;
    std::vector<ProcessingStep> steps_;
};
```

---

## Data Models

### Image Data Format

```cpp
// Host-side image data layout
// Data stored row-major, each row contains all channels consecutively
// Example RGB image: R0G0B0 R1G1B1 R2G2B2 ...

struct HostImage {
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;

    // Access pixel
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
// CUDA error wrapper
class CudaException : public std::runtime_error {
public:
    CudaException(cudaError_t error, const char* file, int line);
    cudaError_t errorCode() const { return error_; }

private:
    cudaError_t error_;
};

// Error checking macro
#define CUDA_CHECK(call) do { \
    cudaError_t error = call; \
    if (error != cudaSuccess) { \
        throw CudaException(error, __FILE__, __LINE__); \
    } \
} while(0)

// Result type (for non-throwing API)
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
    // Memory pool configuration
    size_t maxPoolSize = 512 * 1024 * 1024;  // 512 MB
    bool enableMemoryPool = true;

    // Stream configuration
    int defaultNumStreams = 4;

    // Convolution configuration
    enum class BorderMode { Zero, Mirror, Replicate };
    BorderMode defaultBorderMode = BorderMode::Zero;

    // Performance configuration
    int blockSizeX = 16;
    int blockSizeY = 16;
};
```

---

## Correctness Properties

*正确性属性是指在系统所有有效执行中都应该保持为真的特征或行为。属性是人类可读规范与机器可验证正确性保证之间的桥梁。*

基于需求分析，以下是本系统需要验证的核心正确性属性：

| # | Property | Type | Validates |
|---|----------|------|-----------|
| 1 | **Data Transfer Round-Trip Consistency** | Round-Trip | Requirements 1.1, 1.2 |
| 2 | **Invert Operation Involution** | Involution | Requirement 2.1 |
| 3 | **Grayscale Formula Correctness** | Invariant | Requirement 2.2 |
| 4 | **Brightness Adjustment Range Invariance** | Invariant | Requirement 2.3 |
| 5 | **Convolution vs Reference Implementation Consistency** | Model-Based | Requirements 3.1, 3.2 |
| 6 | **Boundary Handling Correctness** | Invariant | Requirement 3.4 |
| 7 | **Histogram Sum Invariance** | Invariant | Requirement 4.1 |
| 8 | **Scaling Approximate Round-Trip** | Approximate Round-Trip | Requirement 5.1 |
| 9 | **Pipeline Processing Result Consistency** | Confluence | Requirement 6.4 |

### Property Details

<details>
<summary>Property 1: 数据传输往返一致性 (Round-Trip)</summary>

*For any* 有效的图像数据，将其从 Host 上传到 Device，然后从 Device 下载回 Host，得到的数据应该与原始数据完全相同。

**Validates: Requirements 1.1, 1.2**
</details>

<details>
<summary>Property 2: 反色操作自逆性 (Involution)</summary>

*For any* 有效的图像，对其执行两次反色操作后，结果应该与原始图像完全相同。即 `invert(invert(image)) == image`。

**Validates: Requirement 2.1**
</details>

<details>
<summary>Property 3: 灰度化公式正确性 (Invariant)</summary>

*For any* 有效的 RGB 图像中的任意像素 (R, G, B)，灰度化后的输出值应该等于 `round(0.299*R + 0.587*G + 0.114*B)`，且结果在 [0, 255] 范围内。

**Validates: Requirement 2.2**
</details>

<details>
<summary>Property 4: 亮度调整范围不变性 (Invariant)</summary>

*For any* 有效的图像和任意亮度偏移量，亮度调整后的所有像素值应该在 [0, 255] 范围内。

**Validates: Requirement 2.3**
</details>

<details>
<summary>Property 5: 卷积操作与参考实现一致性 (Model-Based)</summary>

*For any* 有效的图像和卷积核，GPU 卷积操作的结果应该与 CPU 参考实现的结果在数值精度范围内一致（允许 ±1 的舍入误差）。

**Validates: Requirements 3.1, 3.2**
</details>

<details>
<summary>Property 6: 边界处理正确性 (Invariant)</summary>

*For any* 使用零填充的卷积操作，边界像素的计算应该将图像外部的像素视为 0。

**Validates: Requirement 3.4**
</details>

<details>
<summary>Property 7: 直方图总和不变性 (Invariant)</summary>

*For any* 有效的灰度图像，其直方图所有 bin 的总和应该等于图像的总像素数（width × height）。

**Validates: Requirement 4.1**
</details>

<details>
<summary>Property 8: 缩放操作近似可逆性 (Approximate Round-Trip)</summary>

*For any* 有效的图像，先放大再缩小回原尺寸（或先缩小再放大），结果应该与原图像在视觉上相似（PSNR > 30dB 或 SSIM > 0.9）。

**Validates: Requirement 5.1**
</details>

<details>
<summary>Property 9: 流水线处理结果一致性 (Confluence)</summary>

*For any* 有效的图像和处理步骤序列，使用流水线处理和使用同步处理应该产生相同的结果。

**Validates: Requirement 6.4**
</details>

---

## Error Handling

### Error Type Classification

```cpp
enum class ErrorCode {
    // Memory errors
    OutOfMemory,
    InvalidMemoryAccess,

    // Parameter errors
    InvalidImageSize,
    InvalidKernelSize,
    InvalidChannelCount,
    NullPointer,

    // CUDA runtime errors
    CudaDriverError,
    CudaKernelLaunchError,
    CudaSyncError,

    // File I/O errors
    FileNotFound,
    FileReadError,
    FileWriteError,
    UnsupportedFormat
};
```

### Error Handling Strategy

| Strategy | Description |
|----------|-------------|
| **Parameter Validation** | All public APIs validate parameters before execution |
| **CUDA Error Checking** | Every CUDA API call is wrapped with `CUDA_CHECK` macro |
| **Resource Cleanup** | RAII ensures resource release |
| **Error Recovery** | System remains consistent state after error |

#### Validation Rules

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

---

## Testing Strategy

### Test Framework Selection

| Framework | Purpose |
|-----------|---------|
| **Google Test (gtest)** | Unit testing framework |
| **RapidCheck** | C++ property-based testing |
| **Google Benchmark** | Performance benchmarking |

### Dual Testing Approach

本项目采用单元测试和属性测试相结合的方法：

#### Unit Tests

验证特定示例和边界情况：

- 测试已知输入的预期输出
- 测试边界条件（空图像、单像素图像、最大尺寸图像）
- 测试错误处理路径

#### Property Tests

验证跨所有输入的通用属性：

- 每个属性测试运行至少 100 次迭代
- 使用随机生成的图像数据
- 测试数学不变性和往返属性

### Property Test Configuration

```cpp
// RapidCheck configuration
rc::detail::Configuration config;
config.max_success = 100;  // Minimum 100 successful iterations
config.max_size = 50;      // Maximum generation size

// Image generator
rc::Gen<HostImage> arbitraryImage() {
    return rc::gen::build<HostImage>(
        rc::gen::set(&HostImage::width, rc::gen::inRange(1, 1024)),
        rc::gen::set(&HostImage::height, rc::gen::inRange(1, 1024)),
        rc::gen::set(&HostImage::channels, rc::gen::element(1, 3, 4)),
        // data generated based on dimensions
    );
}
```

### Test Annotation Format

每个属性测试必须包含以下注释：

```cpp
// Feature: gpu-image-processing, Property 1: Data Transfer Round-Trip Consistency
// Validates: Requirements 1.1, 1.2
RC_GTEST_PROP(MemoryTransfer, RoundTrip, ()) {
    // Test implementation
}
```

### Test Coverage Matrix

| Requirement | Unit Test | Property Test |
|-------------|-----------|---------------|
| 1.1, 1.2 数据传输 | ✓ Basic transfer | ✓ Property 1 |
| 2.1 反色 | ✓ Known image | ✓ Property 2 |
| 2.2 灰度化 | ✓ Known pixels | ✓ Property 3 |
| 2.3 亮度调整 | ✓ Boundary values | ✓ Property 4 |
| 3.1, 3.2 卷积 | ✓ Small images | ✓ Property 5 |
| 3.4 边界处理 | ✓ Boundary pixels | ✓ Property 6 |
| 4.1 直方图 | ✓ Known distribution | ✓ Property 7 |
| 5.1 缩放 | ✓ Integer scaling | ✓ Property 8 |
| 6.4 流水线 | ✓ Multi-image | ✓ Property 9 |
| 7.x 错误处理 | ✓ Various errors | - |

---

## Related Documents

- [Requirements Document](requirements.md) - Detailed requirements specification
- [Tasks Document](tasks.md) - Implementation task checklist
