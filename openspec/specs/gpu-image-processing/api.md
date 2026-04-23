# GPU Image Processing Library - API Reference

**Status:** Implemented  
**Updated:** 2026-04-23

## Overview

This document defines the API contracts for the GPU image processing library. All function signatures, parameter types, and return specifications are defined here.

---

## Core Types

### Image Types

```cpp
namespace gpu_image {

// GPU image representation
struct GpuImage {
    DeviceBuffer buffer;    // GPU memory buffer
    int width;              // Image width in pixels
    int height;             // Image height in pixels
    int channels;           // Number of channels (1, 3, or 4)
    
    // Methods
    size_t pitch() const;       // Bytes per row
    size_t totalBytes() const;  // Total memory size
    bool isValid() const;       // Check validity
    size_t pixelCount() const;  // Total pixels
};

// Host image representation
struct HostImage {
    std::vector<unsigned char> data;  // Pixel data
    int width;
    int height;
    int channels;
    
    // Methods
    unsigned char& at(int x, int y, int c);
    size_t totalBytes() const;
    bool isValid() const;
};

} // namespace gpu_image
```

### Error Types

```cpp
namespace gpu_image {

enum class ErrorCode {
    Success = 0,
    OutOfMemory,
    InvalidMemoryAccess,
    InvalidImageSize,
    InvalidKernelSize,
    InvalidChannelCount,
    NullPointer,
    CudaDriverError,
    CudaKernelLaunchError,
    CudaSyncError,
    FileNotFound,
    FileReadError,
    FileWriteError,
    UnsupportedFormat
};

class CudaException : public std::runtime_error {
public:
    CudaException(cudaError_t error, const char* file, int line);
    cudaError_t errorCode() const;
};

} // namespace gpu_image
```

---

## Memory Management API

### DeviceBuffer

```cpp
namespace gpu_image {

class DeviceBuffer {
public:
    // Constructor & Destructor
    explicit DeviceBuffer(size_t size);
    ~DeviceBuffer();
    
    // Move semantics (copy disabled)
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;
    
    // Synchronous data transfer
    void copyFromHost(const void* hostPtr, size_t size);
    void copyToHost(void* hostPtr, size_t size) const;
    
    // Asynchronous data transfer
    void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
    void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;
    
    // Accessors
    [[nodiscard]] void* data() noexcept;
    [[nodiscard]] const void* data() const noexcept;
    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;
};

} // namespace gpu_image
```

### StreamManager

```cpp
namespace gpu_image {

class StreamManager {
public:
    static StreamManager& instance();
    
    // Stream management
    cudaStream_t acquireStream();
    void releaseStream(cudaStream_t stream);
    
    // Synchronization
    void synchronize(cudaStream_t stream);
    void synchronizeAll();
    
    // Non-copyable, non-movable
    StreamManager(const StreamManager&) = delete;
    StreamManager& operator=(const StreamManager&) = delete;
};

} // namespace gpu_image
```

---

## Processing Operators API

### PixelOperator

```cpp
namespace gpu_image {

class PixelOperator {
public:
    // Image invert: output[i] = 255 - input[i]
    static void invert(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
    
    // RGB to grayscale: gray = 0.299*R + 0.587*G + 0.114*B
    static void toGrayscale(const GpuImage& input, GpuImage& output,
                            cudaStream_t stream = nullptr);
    
    // Brightness adjustment: output[i] = clamp(input[i] + offset, 0, 255)
    static void adjustBrightness(const GpuImage& input, GpuImage& output,
                                 int offset, cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### ConvolutionEngine

```cpp
namespace gpu_image {

class ConvolutionEngine {
public:
    // General convolution
    static void convolve(const GpuImage& input, GpuImage& output,
                         const float* kernel, int kernelSize,
                         cudaStream_t stream = nullptr);
    
    // Gaussian blur (kernelSize must be odd)
    static void gaussianBlur(const GpuImage& input, GpuImage& output,
                             int kernelSize, float sigma,
                             cudaStream_t stream = nullptr);
    
    // Sobel edge detection
    static void sobelEdgeDetection(const GpuImage& input, GpuImage& output,
                                   cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### HistogramCalculator

```cpp
namespace gpu_image {

class HistogramCalculator {
public:
    static constexpr int NUM_BINS = 256;
    
    // Grayscale histogram
    static std::array<int, NUM_BINS> calculate(
        const GpuImage& input,
        cudaStream_t stream = nullptr);
    
    // RGB channel histograms
    static std::array<std::array<int, NUM_BINS>, 3> calculateRGB(
        const GpuImage& input,
        cudaStream_t stream = nullptr);
    
    // Histogram equalization
    static void equalize(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### ImageResizer

```cpp
namespace gpu_image {

class ImageResizer {
public:
    // Bilinear interpolation scaling
    static void resize(const GpuImage& input, GpuImage& output,
                       int newWidth, int newHeight,
                       cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### Morphology

```cpp
namespace gpu_image {

enum class MorphologyOp {
    Erode,
    Dilate,
    Open,
    Close,
    Gradient,
    TopHat,
    BlackHat
};

class Morphology {
public:
    static void erode(const GpuImage& input, GpuImage& output,
                      const unsigned char* kernel, int kernelSize,
                      cudaStream_t stream = nullptr);
    
    static void dilate(const GpuImage& input, GpuImage& output,
                       const unsigned char* kernel, int kernelSize,
                       cudaStream_t stream = nullptr);
    
    static void morphEx(const GpuImage& input, GpuImage& output,
                        MorphologyOp op, int kernelSize,
                        cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### Threshold

```cpp
namespace gpu_image {

enum class ThresholdType {
    Binary,       // dst(x,y) = src(x,y) > thresh ? maxval : 0
    BinaryInv,    // dst(x,y) = src(x,y) > thresh ? 0 : maxval
    Trunc,        // dst(x,y) = src(x,y) > thresh ? thresh : src(x,y)
    ToZero,       // dst(x,y) = src(x,y) > thresh ? src(x,y) : 0
    ToZeroInv     // dst(x,y) = src(x,y) > thresh ? 0 : src(x,y)
};

class Threshold {
public:
    // Global threshold
    static void threshold(const GpuImage& input, GpuImage& output,
                          double thresh, double maxVal,
                          ThresholdType type,
                          cudaStream_t stream = nullptr);
    
    // Adaptive threshold
    static void adaptiveThreshold(const GpuImage& input, GpuImage& output,
                                  double maxValue, int blockSize, double C,
                                  cudaStream_t stream = nullptr);
    
    // Otsu's method (returns optimal threshold)
    static double otsu(const GpuImage& input,
                       cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### ColorSpace

```cpp
namespace gpu_image {

class ColorSpace {
public:
    // RGB <-> HSV
    static void rgbToHsv(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
    static void hsvToRgb(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
    
    // RGB <-> YUV
    static void rgbToYuv(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
    static void yuvToRgb(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
    
    // Channel operations
    static void split(const GpuImage& input,
                      std::vector<GpuImage>& channels,
                      cudaStream_t stream = nullptr);
    static void merge(const std::vector<GpuImage>& channels,
                      GpuImage& output,
                      cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### Geometric

```cpp
namespace gpu_image {

enum class FlipDirection {
    Horizontal,  // Flip around y-axis
    Vertical,    // Flip around x-axis
    Both         // Flip around both axes
};

class Geometric {
public:
    // Rotation
    static void rotate(const GpuImage& input, GpuImage& output,
                       double angle,  // degrees
                       cudaStream_t stream = nullptr);
    
    // Flip
    static void flip(const GpuImage& input, GpuImage& output,
                     FlipDirection dir,
                     cudaStream_t stream = nullptr);
    
    // Affine transform
    static void affine(const GpuImage& input, GpuImage& output,
                       const float* transformMatrix,  // 2x3 matrix
                       cudaStream_t stream = nullptr);
    
    // Perspective transform
    static void perspective(const GpuImage& input, GpuImage& output,
                            const float* transformMatrix,  // 3x3 matrix
                            cudaStream_t stream = nullptr);
    
    // Crop
    static void crop(const GpuImage& input, GpuImage& output,
                     int x, int y, int width, int height,
                     cudaStream_t stream = nullptr);
    
    // Pad
    static void pad(const GpuImage& input, GpuImage& output,
                    int top, int bottom, int left, int right,
                    unsigned char value = 0,
                    cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

### Filters

```cpp
namespace gpu_image {

class Filters {
public:
    // Median filter
    static void median(const GpuImage& input, GpuImage& output,
                       int kernelSize,
                       cudaStream_t stream = nullptr);
    
    // Bilateral filter
    static void bilateral(const GpuImage& input, GpuImage& output,
                          int d, double sigmaColor, double sigmaSpace,
                          cudaStream_t stream = nullptr);
    
    // Box filter
    static void box(const GpuImage& input, GpuImage& output,
                    int kernelSize,
                    cudaStream_t stream = nullptr);
    
    // Sharpen
    static void sharpen(const GpuImage& input, GpuImage& output,
                        float amount = 1.0f,
                        cudaStream_t stream = nullptr);
    
    // Laplacian
    static void laplacian(const GpuImage& input, GpuImage& output,
                          cudaStream_t stream = nullptr);
};

} // namespace gpu_image
```

---

## High-Level API

### ImageProcessor

```cpp
namespace gpu_image {

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();
    
    // Image I/O
    GpuImage loadImage(const std::string& filepath);
    GpuImage loadFromHost(const HostImage& hostImage);
    GpuImage loadFromMemory(const unsigned char* data,
                            int width, int height, int channels);
    
    void saveImage(const GpuImage& image, const std::string& filepath);
    HostImage downloadImage(const GpuImage& image);
    
    // Pixel operations
    GpuImage invert(const GpuImage& input);
    GpuImage toGrayscale(const GpuImage& input);
    GpuImage adjustBrightness(const GpuImage& input, int offset);
    
    // Convolution
    GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& input);
    GpuImage convolve(const GpuImage& input, const float* kernel, int kernelSize);
    
    // Histogram
    std::array<int, 256> histogram(const GpuImage& input);
    GpuImage histogramEqualize(const GpuImage& input);
    
    // Scaling
    GpuImage resize(const GpuImage& input, int newWidth, int newHeight);
    
    // Morphology
    GpuImage erode(const GpuImage& input, int kernelSize);
    GpuImage dilate(const GpuImage& input, int kernelSize);
    
    // Threshold
    GpuImage threshold(const GpuImage& input, double thresh, double maxVal);
    
    // Color space
    GpuImage rgbToHsv(const GpuImage& input);
    GpuImage hsvToRgb(const GpuImage& input);
    
    // Geometric
    GpuImage rotate(const GpuImage& input, double angle);
    GpuImage flip(const GpuImage& input, FlipDirection dir);
    
    // Filters
    GpuImage medianFilter(const GpuImage& input, int kernelSize);
    GpuImage bilateralFilter(const GpuImage& input, int d, double sigmaColor, double sigmaSpace);
};

} // namespace gpu_image
```

### PipelineProcessor

```cpp
namespace gpu_image {

class PipelineProcessor {
public:
    using ProcessingStep = std::function<void(GpuImage&, cudaStream_t)>;
    
    explicit PipelineProcessor(int numStreams = 3);
    ~PipelineProcessor();
    
    // Pipeline configuration
    void addStep(ProcessingStep step);
    void clearSteps();
    
    // Execution
    GpuImage process(const GpuImage& input);
    std::vector<GpuImage> processBatch(const std::vector<GpuImage>& inputs);
    
    // Synchronization
    void synchronize();
    
    // Non-copyable
    PipelineProcessor(const PipelineProcessor&) = delete;
    PipelineProcessor& operator=(const PipelineProcessor&) = delete;
};

} // namespace gpu_image
```

---

## Error Handling Macros

```cpp
// CUDA error checking macro
#define CUDA_CHECK(call) do { \
    cudaError_t error = call; \
    if (error != cudaSuccess) { \
        throw CudaException(error, __FILE__, __LINE__); \
    } \
} while(0)

// CUDA error checking with custom message
#define CUDA_CHECK_MSG(call, msg) do { \
    cudaError_t error = call; \
    if (error != cudaSuccess) { \
        throw CudaException(error, __FILE__, __LINE__, msg); \
    } \
} while(0)
```

---

## Parameter Validation

All public APIs validate parameters before execution:

| Parameter | Validation Rule |
|-----------|-----------------|
| Image dimensions | `width > 0 && height > 0` |
| Channels | `channels ∈ {1, 3, 4}` |
| Kernel size | `kernelSize >= 3 && kernelSize % 2 == 1` |
| Pointers | `ptr != nullptr` |
| File paths | Non-empty string, valid path format |

---

## Related Documents

- [Requirements](requirements.md) - Product requirements and acceptance criteria
- [Design](design.md) - Architecture and implementation details
