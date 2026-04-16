---
layout: default
title: Performance
parent: Documentation
nav_order: 4
description: Optimization guide for Mini-OpenCV - GPU selection, memory management, stream concurrency, and best practices
---

# Performance Optimization Guide

Practical advice for maximizing performance with Mini-OpenCV.

## Hardware Selection

### GPU Architecture Comparison

| Architecture | CC | Memory Bandwidth | Best For | Example GPUs |
|--------------|-----|------------------|----------|--------------|
| Turing | 75 | ~616 GB/s | Balanced | RTX 2080 Ti, T4 |
| Ampere | 80/86 | ~936 GB/s | Compute | A100, RTX 3090 |
| Ada Lovelace | 89 | ~1008 GB/s | Real-time | RTX 4090, L4 |
| Hopper | 90 | ~3 TB/s | AI/HPC | H100 |

### Memory Requirements

Estimate memory usage:

```
GPU Memory ≈ width × height × channels × (operations + 2) × 1.5

Example: 4K (3840×2160) RGB image
= 3840 × 2160 × 3 × 4 × 1.5 ≈ 149 MB per cascade
```

### GPU Selection Matrix

| Use Case | Recommended | Minimum | Notes |
|----------|-------------|---------|-------|
| Mobile/Edge | - | T4, Jetson | Power constrained |
| Desktop | RTX 4070+ | RTX 3060 | Price/performance |
| Workstation | RTX 4090 | RTX 4080 | Max throughput |
| Data Center | A100, H100 | A10 | Enterprise support |

## Memory Optimization

### 1. Minimize Host-Device Transfers

```cpp
// BAD: Multiple transfers
for (int i = 0; i < N; ++i) {
    GpuImage gpu = processor.loadFromHost(images[i]);
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
    HostImage output = processor.downloadImage(result);
}

// GOOD: Pipeline batch processing
PipelineProcessor pipeline(4);
auto results = pipeline.processBatchHost(images);
```

### 2. Use Pinned Memory

Pinned memory enables asynchronous transfers:

```cpp
// Mini-OpenCV handles this internally via PipelineProcessor
PipelineProcessor pipeline(4);  // Uses pinned memory automatically
```

### 3. Pool Allocations

```cpp
// Reuse GPU memory to avoid allocation overhead
DeviceBuffer buffer(maxSize);
for (auto& image : images) {
    buffer.resize(image.size());
    // Process...
}
```

## Stream Concurrency

### Optimal Stream Count

```cpp
// Rule of thumb: streams ≈ number of SMs / 4
// RTX 4090 (128 SMs): ~32 streams
// RTX 3080 (68 SMs): ~17 streams
// A100 (108 SMs): ~27 streams

// Practical: 4-8 streams usually optimal
PipelineProcessor pipeline(4);
```

### Benchmark: Stream Scaling

| Streams | 100 images (512×512) | Speedup | Efficiency |
|---------|----------------------|---------|------------|
| 1 | 800 ms | 1.0x | 100% |
| 2 | 480 ms | 1.7x | 85% |
| 4 | 320 ms | 2.5x | 63% |
| 8 | 280 ms | 2.9x | 36% |

*RTX 4080 with gaussian blur pipeline*

### Stream Ordering

```cpp
// Operations in same stream execute sequentially
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp1, temp2;
    ConvolutionEngine::gaussianBlur(img, temp1, 3, 1.0f, s);
    PixelOperator::invert(temp1, temp2, s);
    Geometric::rotate(temp2, img, 45.0f, s);
});
```

## Kernel Fusion

### When to Fuse

Fuse operations when:
- Operations are simple (pixel-wise)
- Intermediate results aren't needed
- Memory bandwidth is the bottleneck

### Example

```cpp
// BAD: Multiple kernel launches
GpuImage temp1, temp2;
PixelOperator::invert(input, temp1, stream);
PixelOperator::adjustBrightness(temp1, temp2, 50, stream);
ColorSpace::toGrayscale(temp2, output, stream);

// GOOD: Single kernel (if implemented)
// PixelOperator::invertBrightnessGrayscale(input, output, 50, stream);
```

## Algorithm Selection

### Convolution Kernel Sizes

| Kernel | Relative Time | Use Case |
|--------|---------------|----------|
| 3×3 | 1.0x | Fast blur, sharpening |
| 5×5 | 2.2x | Standard blur |
| 7×7 | 4.1x | Heavy blur |
| 9×9 | 6.8x | Maximum blur |

### Interpolation Methods

| Method | Quality | Speed | Use Case |
|--------|---------|-------|----------|
| Nearest | Low | Fastest | Preview, thumbnails |
| Bilinear | Good | Fast | General purpose |
| Bicubic | Better | Slow | High quality resize |

Mini-OpenCV supports nearest and bilinear interpolation.

## Profiling

### Built-in Timing

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
cudaDeviceSynchronize();
std::cout << "Time: " << timer.elapsedMs() << " ms" << std::endl;
```

### Nsight Systems

```bash
# Profile with Nsight Systems
nsys profile -o report.qdrep ./your_program

# View in Nsight Systems GUI
nsys-ui report.qdrep
```

### Key Metrics

| Metric | Target | Action if Poor |
|--------|--------|----------------|
| GPU Utilization | >80% | Increase batch size |
| Memory Bandwidth | >70% | Check data locality |
| Kernel Occupancy | >60% | Check block size |
| PCIe Bandwidth | <20% transfer | Batch transfers |

## Performance Comparison

### Mini-OpenCV vs OpenCV CPU

| Operation | OpenCV CPU | Mini-OpenCV GPU | Speedup |
|-----------|------------|-----------------|---------|
| Gaussian Blur (5×5) | 12 ms | 0.3 ms | 40x |
| Sobel Edge | 8 ms | 0.2 ms | 40x |
| Histogram Equalize | 15 ms | 0.5 ms | 30x |
| Resize (bilinear) | 6 ms | 0.15 ms | 40x |
| RGB→Grayscale | 2 ms | 0.05 ms | 40x |

*1920×1080 image, Intel i9-12900K vs RTX 4080*

### Use Case Recommendations

| Use Case | Recommendation |
|----------|----------------|
| Real-time video | Mini-OpenCV (GPU) |
| Batch processing | Mini-OpenCV (Pipeline) |
| Single image, latency critical | OpenCV (CPU) |
| Embedded systems | OpenCV (CPU) |

## Best Practices

### 1. Warmup

```cpp
// First CUDA call includes JIT compilation overhead
GpuImage warmup = processor.gaussianBlur(dummy, 5, 1.5f);
cudaDeviceSynchronize();
// Subsequent calls are faster
```

### 2. Image Size Alignment

```cpp
// Align to 32 for optimal memory access
int alignedWidth = ((width + 31) / 32) * 32;
int alignedHeight = ((height + 31) / 32) * 32;
```

### 3. Batch Processing

```cpp
// Process multiple images when possible
PipelineProcessor pipeline(4);
auto results = pipeline.processBatchHost(images);
```

### 4. Resolution Cascade

```cpp
// For preview quality, downsample first
GpuImage small = processor.resize(image, width/4, height/4);
GpuImage processed = processor.sobelEdgeDetection(small);
GpuImage fullSize = processor.resize(processed, width, height);
```

## Troubleshooting Performance

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| Low GPU utilization | Small images or low batch | Increase batch size |
| PCIe bound | Too many transfers | Use PipelineProcessor |
| Kernel latency | Many small operations | Fuse kernels |
| Memory errors | Image too large | Process in tiles |

## Next Steps

- [API Reference](api/) - Detailed function documentation
- [Examples](../examples/) - Working code samples
- Run benchmarks: `cmake --build build --target gpu_image_benchmark`

---

*For performance questions, see [FAQ](faq.md) or open a discussion*
