# Delta Specs: GPU Image Processing Library

**Change Type:** ADDED (Initial Implementation)

This document describes what was added in the initial implementation.

---

## ADDED

### Memory Management

#### DeviceBuffer

```cpp
class DeviceBuffer {
public:
    explicit DeviceBuffer(size_t size);
    ~DeviceBuffer();
    
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    
    void copyFromHost(const void* hostPtr, size_t size);
    void copyToHost(void* hostPtr, size_t size) const;
    void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
    void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;
    
    void* data();
    size_t size() const;
};
```

#### GpuImage

```cpp
struct GpuImage {
    DeviceBuffer buffer;
    int width;
    int height;
    int channels;  // 1, 3, or 4
    
    size_t pitch() const;
    size_t totalBytes() const;
    bool isValid() const;
};
```

#### HostImage

```cpp
struct HostImage {
    std::vector<unsigned char> data;
    int width;
    int height;
    int channels;
    
    unsigned char& at(int x, int y, int c);
    size_t totalBytes() const;
    bool isValid() const;
};
```

---

### Pixel Operations

#### PixelOperator

```cpp
class PixelOperator {
public:
    static void invert(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
    
    static void toGrayscale(const GpuImage& input, GpuImage& output,
                            cudaStream_t stream = nullptr);
    
    static void adjustBrightness(const GpuImage& input, GpuImage& output,
                                 int offset, cudaStream_t stream = nullptr);
};
```

---

### Convolution Operations

#### ConvolutionEngine

```cpp
class ConvolutionEngine {
public:
    static void convolve(const GpuImage& input, GpuImage& output,
                         const float* kernel, int kernelSize,
                         cudaStream_t stream = nullptr);
    
    static void gaussianBlur(const GpuImage& input, GpuImage& output,
                             int kernelSize, float sigma,
                             cudaStream_t stream = nullptr);
    
    static void sobelEdgeDetection(const GpuImage& input, GpuImage& output,
                                   cudaStream_t stream = nullptr);
};
```

---

### Histogram Operations

#### HistogramCalculator

```cpp
class HistogramCalculator {
public:
    static constexpr int NUM_BINS = 256;
    
    static std::array<int, NUM_BINS> calculate(const GpuImage& input,
                                                cudaStream_t stream = nullptr);
    
    static std::array<std::array<int, NUM_BINS>, 3> calculateRGB(
        const GpuImage& input, cudaStream_t stream = nullptr);
};
```

---

### Image Scaling

#### ImageResizer

```cpp
class ImageResizer {
public:
    static void resize(const GpuImage& input, GpuImage& output,
                       int newWidth, int newHeight,
                       cudaStream_t stream = nullptr);
};
```

---

### High-Level API

#### ImageProcessor

```cpp
class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor();
    
    GpuImage loadImage(const std::string& filepath);
    GpuImage loadFromHost(const HostImage& hostImage);
    void saveImage(const GpuImage& image, const std::string& filepath);
    HostImage downloadImage(const GpuImage& image);
    
    GpuImage invert(const GpuImage& input);
    GpuImage toGrayscale(const GpuImage& input);
    GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& input);
    GpuImage resize(const GpuImage& input, int newWidth, int newHeight);
    std::array<int, 256> histogram(const GpuImage& input);
};
```

#### PipelineProcessor

```cpp
class PipelineProcessor {
public:
    using ProcessingStep = std::function<void(GpuImage&, cudaStream_t)>;
    
    explicit PipelineProcessor(int numStreams = 3);
    
    void addStep(ProcessingStep step);
    void clearSteps();
    
    GpuImage process(const GpuImage& input);
    std::vector<GpuImage> processBatch(const std::vector<GpuImage>& inputs);
    void synchronize();
};
```

---

### Error Handling

```cpp
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

class CudaException : public std::runtime_error { ... };

#define CUDA_CHECK(call) ...
```

---

## MODIFIED

N/A (Initial implementation)

---

## REMOVED

N/A (Initial implementation)
