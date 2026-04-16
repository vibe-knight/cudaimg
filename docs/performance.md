---
layout: default
title: Performance Optimization
description: Optimization guide for Mini-OpenCV - GPU selection, memory management, and best practices for maximum throughput.
---

# Performance Optimization Guide

This guide provides practical advice for maximizing performance when using Mini-OpenCV. From hardware selection to code-level optimizations.

## Hardware Selection

### GPU Architecture Comparison

| Architecture | CC | Memory Bandwidth | Best For | Example GPUs |
|--------------|-----|------------------|----------|--------------|
| Turing | 75 | ~616 GB/s | Balanced | RTX 2080 Ti, T4 |
| Ampere | 80/86 | ~936 GB/s | Compute | A100, RTX 3090 |
| Ada Lovelace | 89 | ~1008 GB/s | Real-time | RTX 4090, L4 |
| Hopper | 90 | ~3 TB/s | AI/HPC | H100 |

### Memory Requirements

Use this formula to estimate memory usage:

```
GPU Memory = width × height × channels × (operations + 2) × 1.5

Example: 4K (3840×2160) image with 3 channels
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
    GpuImage gpu = processor.loadFromHost(images[i]);  // Transfer
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
    HostImage output = processor.downloadImage(result);  // Transfer
    results[i] = output;
}

// GOOD: Batch upload
PipelineProcessor pipeline(4);
for (int i = 0; i < N; ++i) {
    pipeline.processHost(images[i]);
}
results = pipeline.getResults();
```

### 2. Use Pinned (Page-Locked) Memory

```cpp
// Pinned memory enables async transfers
cudaHostAlloc(&pinnedPtr, size, cudaHostAllocDefault);

// Mini-OpenCV handles this internally via PipelineProcessor
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

The optimal number of CUDA streams depends on your GPU:

```cpp
// Rule of thumb: streams = number of SMs / 4
// RTX 4090 (128 SMs): 32 streams
// RTX 3080 (68 SMs): 17 streams
// A100 (108 SMs): 27 streams

// But usually 4-8 streams show best results:
PipelineProcessor pipeline(4);  // Good default
```

### Benchmark Results

| Streams | 10 images (512×512) | Speedup | Efficiency |
|---------|---------------------|---------|------------|
| 1 | 120 ms | 1.0x | 100% |
| 2 | 75 ms | 1.6x | 80% |
| 4 | 52 ms | 2.3x | 58% |
| 8 | 45 ms | 2.7x | 34% |
| 16 | 42 ms | 2.9x | 18% |

*Tested on RTX 4080 with gaussian blur pipeline*

### Stream Ordering

```cpp
// Operations in same stream are ordered automatically
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    GpuImage temp1, temp2;
    // These execute sequentially
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

### Example: Fused Operations

```cpp
// BAD: Separate kernel launches
GpuImage temp1, temp2;
PixelOperator::invert(input, temp1, stream);
PixelOperator::adjustBrightness(temp1, temp2, 50, stream);
ColorSpace::toGrayscale(temp2, output, stream);

// GOOD: Single fused operation (if implemented)
// PixelOperator::invertBrightnessGrayscale(input, output, 50, stream);
```

### Built-in Fused Operations

| Fused Operation | Description | Speedup |
|-----------------|-------------|---------|
| `histogramEqualize` | Equalize + normalize | 1.3x |
| `sobelEdgeDetection` | Sobel X + Y + magnitude | 1.4x |

## Algorithm Selection

### Convolution Kernel Sizes

| Kernel Size | Relative Speed | Use Case |
|-------------|---------------|----------|
| 3×3 | 1.0x | Fast blur, sharpening |
| 5×5 | 2.2x | Standard blur |
| 7×7 | 4.1x | Heavy blur |
| 9×9 | 6.8x | Maximum blur |

*Larger kernels need more registers and shared memory*

### Interpolation Methods

| Method | Quality | Speed | Use Case |
|--------|---------|-------|----------|
| Nearest | Low | Fastest | Preview, thumbnails |
| Bilinear | Good | Fast | General purpose |
| Bicubic | Better | Slow | High quality resize |

Mini-OpenCV currently supports nearest and bilinear.

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
cudaDeviceSynchronize();  // Ensure GPU is done
std::cout << "Time: " << timer.elapsedMs() << " ms" << std::endl;
```

### Nsight Systems

```bash
# Profile with Nsight Systems
nsys profile -o report.qdrep ./your_program

# View in Nsight Systems GUI
nsys-ui report.qdrep
```

### Key Metrics to Monitor

| Metric | Target | Action if Poor |
|--------|--------|----------------|
| GPU Utilization | >80% | Increase batch size |
| Memory Bandwidth | >70% | Check data locality |
| Kernel Occupancy | >60% | Check block size |
| PCIe Bandwidth | <20% data transfer | Batch transfers |

## Comparison with OpenCV

### Performance Ratio (GPU vs CPU)

| Operation | OpenCV CPU | Mini-OpenCV GPU | Speedup |
|-----------|------------|-----------------|---------|
| Gaussian Blur (5×5) | 12 ms | 0.3 ms | 40x |
| Sobel Edge | 8 ms | 0.2 ms | 40x |
| Histogram Equalize | 15 ms | 0.5 ms | 30x |
| Resize (bilinear) | 6 ms | 0.15 ms | 40x |
| RGB→Grayscale | 2 ms | 0.05 ms | 40x |

*Measured on 1920×1080 image, Intel i9-12900K vs RTX 4080*

### When to Use Each

| Use Case | Recommendation |
|----------|----------------|
| Real-time video | Mini-OpenCV (GPU) |
| Batch processing | Mini-OpenCV (Pipeline) |
| Single image, latency critical | OpenCV (CPU) - no GPU warmup |
| Embedded systems | OpenCV (CPU) - no GPU available |

## Best Practices

### 1. Warmup

```cpp
// First CUDA call includes JIT compilation overhead
GpuImage warmup = processor.gaussianBlur(dummy, 5, 1.5f);
cudaDeviceSynchronize();  // Force compilation
// Subsequent calls are faster
```

### 2. Image Size Alignment

```cpp
// Align image dimensions to 32 for optimal memory access
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

## Troubleshooting Performance Issues

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| Low GPU utilization | Small images or low batch size | Increase batch size or use tiling |
| PCIe bound | Too many host-device transfers | Use PipelineProcessor or batch uploads |
| Kernel latency | Many small operations | Fuse kernels or use larger tiles |
| Memory errors | Image too large | Reduce resolution or process in tiles |

## Next Steps

- [API Reference](api/) - Detailed function documentation
- [Examples](../examples/) - Working code samples
- Run benchmarks: `cmake --build build --target gpu_image_benchmark`

---

*For performance questions, see [FAQ](faq) or open a discussion*
