# RFC 0001: GPU Image Processing Library Design

**Status:** Implemented  
**Created:** 2024  
**Updated:** 2026-04-17  
**Author:** mini-opencv contributors

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

This design document describes the architecture and implementation details of a CUDA-based high-performance image processing library. The library uses a layered architecture, separating memory management, compute kernels, and high-level APIs to achieve good maintainability and extensibility.

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
│  └──────────────┘ └──────────────┘ └──────────────────┘    │
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
│  └──────────────┘ └──────────────┘ └──────────────────┘    │
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

Manages the lifecycle of a single GPU memory buffer.

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

Manages memory allocation and transfer, providing memory pool functionality.

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

Manages CUDA Streams for async operations.

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

GPU image representation.

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

Implementation of pixel-level operations.

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

Implementation of convolution operations, using Shared Memory optimization.

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

Histogram calculation using atomic operations and parallel reduction.

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

Image scaling using bilinear interpolation.

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

Main user-facing API class.

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

Pipeline processor for efficient batch image processing.

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

*Correctness properties are invariants or behaviors that should hold across all valid executions of the system. Properties bridge human-readable specifications and machine-verifiable correctness guarantees.*

Based on requirements analysis, here are the core correctness properties to verify for this system:

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
<summary>Property 1: Data Transfer Round-Trip Consistency</summary>

*For any* valid image data, uploading from Host to Device and then downloading back to Host should yield data identical to the original.

**Validates: Requirements 1.1, 1.2**
</details>

<details>
<summary>Property 2: Invert Operation Involution</summary>

*For any* valid image, applying the invert operation twice should yield the original image. That is, `invert(invert(image)) == image`.

**Validates: Requirement 2.1**
</details>

<details>
<summary>Property 3: Grayscale Formula Correctness</summary>

*For any* valid RGB image and any pixel (R, G, B), the grayscale output should equal `round(0.299*R + 0.587*G + 0.114*B)` and be within [0, 255] range.

**Validates: Requirement 2.2**
</details>

<details>
<summary>Property 4: Brightness Adjustment Range Invariance</summary>

*For any* valid image and any brightness offset, all pixel values after brightness adjustment should remain within [0, 255] range.

**Validates: Requirement 2.3**
</details>

<details>
<summary>Property 5: Convolution Consistency with Reference</summary>

*For any* valid image and convolution kernel, the GPU convolution result should match the CPU reference implementation within numerical precision (allowing ±1 rounding error).

**Validates: Requirements 3.1, 3.2**
</details>

<details>
<summary>Property 6: Boundary Handling Correctness</summary>

*For any* convolution operation with zero-padding, boundary pixels should treat pixels outside the image as 0.

**Validates: Requirement 3.4**
</details>

<details>
<summary>Property 7: Histogram Sum Invariance</summary>

*For any* valid grayscale image, the sum of all histogram bins should equal the total pixel count (width × height).

**Validates: Requirement 4.1**
</details>

<details>
<summary>Property 8: Scaling Approximate Round-Trip</summary>

*For any* valid image, scaling up then down (or down then up) should yield a result visually similar to the original (PSNR > 30dB or SSIM > 0.9).

**Validates: Requirement 5.1**
</details>

<details>
<summary>Property 9: Pipeline Processing Consistency</summary>

*For any* valid image and processing sequence, pipeline processing and synchronous processing should produce identical results.

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
| **Error Recovery** | System remains in consistent state after error |

#### Validation Rules

1. **Parameter Validation**: All public APIs validate parameters before execution
   - Image dimensions must be > 0
   - Kernel size must be odd and >= 3
   - Channel count must be 1, 3, or 4
   - Pointers must not be null

2. **CUDA Error Checking**: Check errors after every CUDA API call
   - Wrap all CUDA calls with `CUDA_CHECK` macro
   - Error messages include filename, line number, and CUDA error description

3. **Resource Cleanup**: Use RAII to ensure resource release
   - `DeviceBuffer` destructor automatically releases memory
   - Exception safety: no resource leaks even when exceptions occur

4. **Error Recovery**: Maintain consistent state after errors
   - Failed operations don't modify input data
   - Partially completed operations are rolled back

---

## Testing Strategy

### Test Framework Selection

| Framework | Purpose |
|-----------|---------|
| **Google Test (gtest)** | Unit testing framework |
| **RapidCheck** | C++ property-based testing |
| **Google Benchmark** | Performance benchmarking |

### Dual Testing Approach

This project uses a combination of unit tests and property-based tests:

#### Unit Tests

Verify specific examples and edge cases:

- Test expected output for known inputs
- Test boundary conditions (empty images, single-pixel images, maximum size images)
- Test error handling paths

#### Property Tests

Verify universal properties across all inputs:

- Each property test runs at least 100 iterations
- Use randomly generated image data
- Test mathematical invariants and round-trip properties

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

Each property test must include the following annotation:

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
| 1.1, 1.2 Data Transfer | ✓ Basic transfer | ✓ Property 1 |
| 2.1 Invert | ✓ Known image | ✓ Property 2 |
| 2.2 Grayscale | ✓ Known pixels | ✓ Property 3 |
| 2.3 Brightness | ✓ Boundary values | ✓ Property 4 |
| 3.1, 3.2 Convolution | ✓ Small images | ✓ Property 5 |
| 3.4 Boundary | ✓ Boundary pixels | ✓ Property 6 |
| 4.1 Histogram | ✓ Known distribution | ✓ Property 7 |
| 5.1 Scaling | ✓ Integer scaling | ✓ Property 8 |
| 6.4 Pipeline | ✓ Multi-image | ✓ Property 9 |
| 7.x Error Handling | ✓ Various errors | - |

---

## Related Documents

- [Requirements Document](../product/gpu-image-processing-requirements.md) - Detailed requirements specification
- [Tasks Document](0001-gpu-image-processing-tasks.md) - Implementation task checklist
