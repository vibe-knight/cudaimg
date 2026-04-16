---
layout: default
title: FAQ
parent: Documentation
nav_order: 5
description: Common questions and answers about Mini-OpenCV - installation, usage, troubleshooting, and best practices
---

# Frequently Asked Questions

Quick answers to common questions about Mini-OpenCV.

## General Questions

### What is Mini-OpenCV?

Mini-OpenCV is a high-performance CUDA-based image processing library providing GPU-accelerated implementations of common computer vision operations. It offers a modern C++ API optimized for NVIDIA GPUs.

### How does it compare to OpenCV?

| Aspect | OpenCV | Mini-OpenCV |
|--------|--------|-------------|
| Backend | CPU (default) | GPU (CUDA) |
| GPU support | Via cv::cuda module | Native, required |
| Dependencies | Many | Minimal (CUDA only) |
| Performance | Good on CPU | 30-50x faster on GPU |
| API | Mature, complex | Modern, simpler |

### Is it a replacement for OpenCV?

No. Mini-OpenCV focuses on GPU-accelerated operators and complements OpenCV. Use OpenCV for CPU fallback, I/O operations, and algorithms not yet implemented here.

## Installation

### What GPU do I need?

**Minimum:** NVIDIA GPU with Compute Capability 7.5+ (Turing architecture)
- RTX 20 series, T4, and newer
- GTX 16 series (limited support)

**Recommended:** Ampere (RTX 30 series) or newer for best performance.

### Can I use it without a GPU?

No. A CUDA-capable NVIDIA GPU is required. The library is designed specifically for GPU acceleration.

### Which CUDA version should I use?

- **Minimum:** CUDA 11.0
- **Recommended:** CUDA 12.x (latest stable)
- **Tested:** CUDA 11.0 through 12.4

### CMake can't find CUDA

```bash
# Set CUDA path explicitly
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

# Or use CMake variables
cmake -S . -B build -DCUDAToolkit_ROOT=/usr/local/cuda
```

## Usage

### How do I load an image?

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

// Method 1: From file (requires stb)
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);

// Method 2: From memory buffer
std::vector<unsigned char> data(width * height * 3);
// Fill data...
HostImage host = ImageUtils::createHostImage(width, height, 3);
std::memcpy(host.data.data(), data.data(), data.size());
```

### What image formats are supported?

With `GPU_IMAGE_ENABLE_IO=ON` (default):
- JPEG, PNG, BMP, TGA, HDR via stb_image
- 8-bit per channel
- RGB, RGBA, grayscale

For other formats, use OpenCV or other libraries to load, then transfer to Mini-OpenCV.

### Can I process video?

Mini-OpenCV doesn't include video I/O, but works with OpenCV:

```cpp
#include <opencv2/opencv.hpp>
#include "gpu_image/gpu_image_processing.hpp"

cv::VideoCapture cap("video.mp4");
cv::Mat frame;

while (cap.read(frame)) {
    // Convert OpenCV Mat to HostImage
    HostImage host = ImageUtils::createHostImage(
        frame.cols, frame.rows, frame.channels());
    std::memcpy(host.data.data(), frame.data, 
                frame.total() * frame.elemSize());
    
    // Process with Mini-OpenCV
    GpuImage gpu = processor.loadFromHost(host);
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
}
```

### How do I save results?

```cpp
// Download from GPU
HostImage result = processor.downloadImage(gpuImage);

// Save to file
ImageIO::save("output.jpg", result);

// Or access raw data
std::vector<unsigned char>& data = result.data;
int width = result.width;
int height = result.height;
```

## Performance

### Why is the first operation slow?

CUDA kernel compilation (JIT) happens on first use. This adds 1-2 seconds to the first operation. Subsequent calls are fast.

To avoid this in production:
- Warm up the GPU with a dummy operation
- Use pipeline processing where overhead is amortized

### How can I process images faster?

1. **Use PipelineProcessor** for batch processing
2. **Reduce memory transfers** - keep data on GPU
3. **Use appropriate kernel sizes** - smaller is faster
4. **Process at lower resolution** for preview

### My GPU utilization is low

Common causes:
- Images too small (< 512×512)
- Not using batch processing
- Too many host-device transfers

Solution: Use `PipelineProcessor` with batch size of 8-16.

### Out of memory errors

```cpp
// Check available memory
size_t free, total;
cudaMemGetInfo(&free, &total);

// Estimate required memory
size_t required = width * height * channels * 4;

if (required > free) {
    // Process in tiles or reduce resolution
}
```

## Technical Questions

### Which CUDA stream should I use?

```cpp
// Default (nullptr) - synchronous, simplest
GpuImage result = processor.gaussianBlur(image, 5, 1.5f);

// Custom stream - for async/concurrent processing
cudaStream_t stream;
cudaStreamCreate(&stream);
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, stream);
cudaStreamSynchronize(stream);
```

For most users, the synchronous API is sufficient.

### Can I do in-place operations?

Some operations support in-place processing:

```cpp
// In-place
PixelOperator::invertInPlace(image, stream);

// Not in-place (requires separate output)
GpuImage output;
ConvolutionEngine::gaussianBlur(input, output, 5, 1.5f, stream);
```

### Thread safety

| Component | Thread Safety |
|-----------|---------------|
| `ImageProcessor` | Not thread-safe. Create one per thread. |
| `PipelineProcessor` | Not thread-safe. |
| Operator functions | Thread-safe with different images/streams |

```cpp
// Thread-safe usage
void processThread(int threadId, const HostImage& input) {
    ImageProcessor processor;  // Per-thread processor
    GpuImage gpu = processor.loadFromHost(input);
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
}
```

### Error handling

```cpp
try {
    GpuImage result = processor.gaussianBlur(image, 5, -1.0f);
} catch (const CudaException& e) {
    // CUDA runtime error
    std::cerr << "CUDA Error: " << e.what() << std::endl;
} catch (const std::invalid_argument& e) {
    // Invalid parameter
    std::cerr << "Invalid argument: " << e.what() << std::endl;
}
```

## Troubleshooting

### Build fails with "CUDA not found"

1. Verify CUDA: `nvcc --version`
2. Check CMake: `cmake --version` (need 3.18+)
3. Set CUDA path: `export CUDAToolkit_ROOT=/usr/local/cuda`

### Tests fail

```bash
# Run with verbose output
./build/bin/gpu_image_tests --gtest_filter=*

# Check CUDA availability
./build/bin/basic_example
```

### Different results on different GPUs

Minor numerical differences are expected due to:
- Different floating-point implementations
- Order of operations in reductions

Differences are usually < 1 pixel value and not visually noticeable.

### Program crashes at startup

Common causes:
1. No CUDA-capable GPU
2. Outdated NVIDIA drivers
3. CUDA runtime not in PATH

Check: `nvidia-smi` should show GPU information.

## Contributing

### How can I contribute?

See [ContributING.md](../CONTRIBUTING.md) for:
- Reporting bugs
- Requesting features
- Submitting pull requests

### Where is the roadmap?

Check [GitHub Issues](https://github.com/LessUp/mini-opencv/issues) for planned features and known bugs.

## Getting Help

If your question isn't answered here:

1. Search [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
2. Check [API Reference](api/)
3. Read [Performance Guide](performance)
4. Ask in [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)

---

*Last updated: 2026-04-16*
