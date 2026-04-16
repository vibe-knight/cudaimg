---
layout: default
title: Architecture
parent: Documentation
nav_order: 3
description: Deep dive into Mini-OpenCV architecture - three-layer design, memory management, and CUDA stream concurrency
---

# Architecture Overview

Mini-OpenCV uses a three-layer architecture separating user APIs, operator implementations, and infrastructure services.

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

**Usage:**
```cpp
// Synchronous
ImageProcessor processor;
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);

// Asynchronous
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

Operators are **stateless** and **stream-aware**:

```cpp
// Stateless - no internal state
namespace ConvolutionEngine {
    void gaussianBlur(const GpuImage& src, GpuImage& dst, 
                      int kernelSize, float sigma, 
                      cudaStream_t stream = nullptr);
}
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

## Memory Management

### RAII Design

```cpp
// Automatic GPU memory management
{
    DeviceBuffer buffer(1024 * 1024);  // 1MB GPU memory
    void* ptr = buffer.data();
}  // Automatically freed when out of scope
```

### Memory Patterns

```cpp
// Pattern: Load-Process-Download
HostImage host = loadImage("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage output = processor.downloadImage(result);
```

### Memory Layout

Images stored in **interleaved format**:

```
RGB Image (width=4, height=3, channels=3):

Memory: [R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3]
        [R4 G4 B4 R5 G5 B5 R6 G6 B6 R7 G7 B7]
        [R8 G8 B8 R9 G9 B9 R10 G10 B10 R11 G11 B11]

Stride: row_stride = width * channels (padded to alignment)
```

## CUDA Stream Concurrency

### Stream Usage

```cpp
// Default stream - synchronous with host
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, nullptr);

// Explicit stream - asynchronous execution
cudaStream_t stream;
cudaStreamCreate(&stream);
ConvolutionEngine::gaussianBlur(src, dst, 5, 1.5f, stream);
cudaStreamSynchronize(stream);
```

### Pipeline Concurrency Model

```cpp
PipelineProcessor pipeline(4);  // 4 concurrent streams

// Internally:
// Stream 0: [Upload] → [Op1] → [Op2] → [Download]
// Stream 1: [Upload] → [Op1] → [Op2] → [Download]
// Stream 2: [Upload] → [Op1] → [Op2] → [Download]
// Stream 3: [Upload] → [Op1] → [Op2] → [Download]
// Timeline overlap maximizes GPU utilization
```

### Stream Safety

Operations are **stream-safe** when:
- Input and output images are different (no in-place)
- Or using stream-specific in-place methods

```cpp
// Safe: different input/output
GpuImage temp;
ConvolutionEngine::gaussianBlur(input, temp, 5, 1.5f, stream);

// Safe: in-place with stream-aware method
PixelOperator::invertInPlace(image, stream);
```

## Error Handling

### Exception Hierarchy

```
std::exception
    └── CudaException (CUDA runtime errors + context)
```

### Patterns

```cpp
// Pattern 1: Let exceptions propagate
try {
    GpuImage result = processor.gaussianBlur(image, 5, -1.0f);
} catch (const CudaException& e) {
    std::cerr << "CUDA error: " << e.what() << std::endl;
}

// Pattern 2: Pre-validation
if (sigma <= 0) {
    throw std::invalid_argument("Sigma must be positive");
}
```

## Design Principles

1. **Separation of Concerns**: Each layer has single responsibility
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

*For architecture questions, see [FAQ](faq.md) or open a discussion*
