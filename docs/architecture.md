---
layout: default
title: Architecture
description: Deep dive into Mini-OpenCV architecture - three-layer design, memory management, and CUDA stream concurrency.
---

# Architecture Overview

Mini-OpenCV is designed with a three-layer architecture that separates concerns between user-facing APIs, operator implementations, and infrastructure services. This document explains the design principles and how they affect your usage.

## Three-Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      APPLICATION LAYER                           │
│                                                                  │
│   ┌──────────────────┐    ┌──────────────────┐                  │
│   │ ImageProcessor   │    │ PipelineProcessor│                  │
│   │ (Synchronous)    │    │ (Asynchronous)   │                  │
│   └────────┬─────────┘    └────────┬─────────┘                  │
│            │                       │                             │
│            ▼                       ▼                             │
│   ┌─────────────────────────────────────────┐                    │
│   │     Unified: gpu_image_processing      │                    │
│   └─────────────────────────────────────────┘                    │
├─────────────────────────────────────────────────────────────────┤
│                      OPERATOR LAYER                              │
│                                                                  │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │PixelOperator │  │Convolution   │  │HistogramCalc │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │ImageResizer  │  │Morphology    │  │Threshold     │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │ColorSpace    │  │Geometric     │  │Filters       │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐                            │
│   │ImageArith    │  │DeviceBuffer  │                            │
│   └──────────────┘  └──────────────┘                            │
├─────────────────────────────────────────────────────────────────┤
│                   INFRASTRUCTURE LAYER                           │
│                                                                  │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │ DeviceBuffer │  │MemoryManager │  │StreamManager │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│   │   GpuImage   │  │  HostImage   │  │   ImageIO    │          │
│   └──────────────┘  └──────────────┘  └──────────────┘          │
│   ┌──────────────┐                                               │
│   │  CudaError   │                                               │
│   └──────────────┘                                               │
└─────────────────────────────────────────────────────────────────┘
```

### Layer Responsibilities

#### 1. Application Layer

High-level APIs for end users:

| Component | Purpose | Sync/Async |
|-----------|---------|------------|
| `ImageProcessor` | One-stop image processing | Synchronous |
| `PipelineProcessor` | Multi-step batch processing | Asynchronous |

**Usage Example:**
```cpp
// Synchronous - simple operations
ImageProcessor processor;
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);

// Asynchronous - batch processing with streams
PipelineProcessor pipeline(4);
pipeline.addStep([](GpuImage& img, cudaStream_t s) {
    ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, s);
});
```

#### 2. Operator Layer

Low-level CUDA kernel implementations:

| Module | Operators | Key Techniques |
|--------|-----------|----------------|
| `PixelOperator` | Invert, grayscale, brightness | Per-pixel parallel |
| `ConvolutionEngine` | Gaussian, Sobel, custom | Shared memory tiling |
| `HistogramCalculator` | Calculation, equalization | Atomic ops + reduction |
| `ImageResizer` | Bilinear, nearest-neighbor | Texture memory |
| `Morphology` | Erosion, dilation, open/close | Structuring elements |
| `Threshold` | Global, adaptive, Otsu | Histogram-driven |
| `ColorSpace` | RGB/HSV/YUV | Matrix operations |
| `Geometric` | Rotate, flip, affine | Bilinear interpolation |
| `Filters` | Median, bilateral, box | Edge-preserving |

Operator modules are designed to be **stateless** and **stream-aware**:

```cpp
// Stateless - no internal state kept between calls
namespace ConvolutionEngine {
    void gaussianBlur(const GpuImage& src, GpuImage& dst, 
                      int kernelSize, float sigma, 
                      cudaStream_t stream = nullptr);
}

// Stream-aware - all operations accept optional cudaStream_t
void myOperation(GpuImage& img, cudaStream_t stream = nullptr);
```

#### 3. Infrastructure Layer

Memory and execution management:

| Component | Responsibility |
|-----------|----------------|
| `DeviceBuffer` | RAII GPU memory management |
| `MemoryManager` | Memory pool and allocation tracking |
| `StreamManager` | CUDA stream lifecycle management |
| `GpuImage` | GPU image container |
| `HostImage` | Host memory image container |
| `CudaError` | Exception-based error handling |

## Memory Management Model

### RAII-Based Design

```cpp
// DeviceBuffer automatically manages GPU memory
{
    DeviceBuffer buffer(1024 * 1024);  // Allocate 1MB GPU memory
    // Use buffer...
    void* ptr = buffer.data();
    size_t size = buffer.size();
}  // Automatically freed when buffer goes out of scope
```

### Memory Transfer Patterns

```cpp
// Pattern 1: Load-Process-Download (typical workflow)
HostImage host = loadImage("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage output = processor.downloadImage(result);
saveImage("output.jpg", output);

// Pattern 2: Zero-copy (when using unified memory)
// Not yet supported, planned for future release

// Pattern 3: Batch upload with pinned memory
std::vector<HostImage> batch = loadBatch(100);
std::vector<GpuImage> gpuBatch;
for (auto& host : batch) {
    gpuBatch.push_back(processor.loadFromHost(host));
}
```

### Memory Layout

Images are stored in **interleaved format** (planar for grayscale):

```
RGB Image Layout (width=4, height=3, channels=3):

Memory: [R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3]
        [R4 G4 B4 R5 G5 B5 R6 G6 B6 R7 G7 B7]
        [R8 G8 B8 R9 G9 B9 R10 G10 B10 R11 G11 B11]

Stride: row_stride = width * channels (padded to alignment)
        total_size = row_stride * height
```

## CUDA Stream Concurrency

### Stream Architecture

```cpp
// Default stream (nullptr) - synchronous with host
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, nullptr);

// Explicit stream - asynchronous execution
cudaStream_t stream;
cudaStreamCreate(&stream);
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, stream);
cudaStreamSynchronize(stream);
cudaStreamDestroy(stream);
```

### Pipeline Processor Concurrency Model

```cpp
PipelineProcessor pipeline(4);  // 4 concurrent streams

// Internally:
// Stream 0: [Upload] -> [Op1] -> [Op2] -> [Download]
// Stream 1: [Upload] -> [Op1] -> [Op2] -> [Download]
// Stream 2: [Upload] -> [Op1] -> [Op2] -> [Download]
// Stream 3: [Upload] -> [Op1] -> [Op2] -> [Download]
// Timeline overlap maximizes GPU utilization
```

### Stream Safety

All operator modules are **stream-safe** when:
- Input and output images are different (no in-place)
- Or using stream-specific versions (e.g., `invertInPlace`)

```cpp
// Safe: different input/output
GpuImage temp;
ConvolutionEngine::gaussianBlur(input, temp, 5, 1.5f, stream);
ConvolutionEngine::sobelEdgeDetection(temp, output, stream);

// Safe: in-place with stream-aware method
PixelOperator::invertInPlace(image, stream);
```

## Error Handling Strategy

### Exception Hierarchy

```
std::exception
    └── CudaException (our custom exception)
            └── CUDA runtime errors + context
```

### Error Handling Patterns

```cpp
// Pattern 1: Let exceptions propagate
try {
    GpuImage result = processor.gaussianBlur(image, 5, -1.0f);  // Invalid sigma
} catch (const CudaException& e) {
    std::cerr << "CUDA error: " << e.what() << std::endl;
}

// Pattern 2: Pre-validation
if (sigma <= 0) {
    throw std::invalid_argument("Sigma must be positive");
}
GpuImage result = processor.gaussianBlur(image, kernelSize, sigma);

// Pattern 3: Defensive programming
if (!image.isValid()) {
    return GpuImage();  // Return empty image
}
```

## Design Principles

1. **Separation of Concerns**: Each layer has a single responsibility
2. **Zero-Copy When Possible**: Minimize host-device transfers
3. **Stream-Aware**: All operations support async execution
4. **RAII Memory**: Automatic resource management
5. **Fail-Fast**: Validate inputs early, throw on errors

## Performance Considerations

| Operation | Bottleneck | Optimization |
|-----------|-----------|--------------|
| Kernel Launch | Overhead | Batch operations, fuse kernels |
| Memory Transfer | PCIe bandwidth | Use pinned memory, batch uploads |
| Small Images | Kernel overhead | Process multiple images per kernel |
| Large Images | Memory capacity | Tile processing, streaming |

## Next Steps

- [Performance Guide](performance) - Practical optimization tips
- [API Reference](api/) - Detailed module documentation
- [Examples](../examples/) - Code samples

---

*For questions about architecture, see [FAQ](faq) or open a discussion* 
