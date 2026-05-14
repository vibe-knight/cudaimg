# Mini-OpenCV Architecture

**Status:** Implemented  
**Updated:** 2026-04-23

## Overview

Mini-OpenCV is a CUDA-based high-performance image processing library providing GPU-accelerated operators for computer vision applications.

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│         ImageProcessor  ·  PipelineProcessor                 │
├─────────────────────────────────────────────────────────────┤
│              Operator Layer (CUDA Kernels)                   │
│  PixelOperator  │  ConvolutionEngine  │  Geometric          │
│  Morphology     │  ColorSpace         │  Filters            │
│  Threshold      │  HistogramCalculator│  ImageResizer       │
├─────────────────────────────────────────────────────────────┤
│                  Infrastructure Layer                        │
│  DeviceBuffer  ·  GpuImage/HostImage  ·  CudaError          │
│  ImageIO       ·  StreamManager                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Layer Responsibilities

### Application Layer

High-level user-facing APIs for common image processing tasks.

| Component | Description |
|-----------|-------------|
| `ImageProcessor` | Main API for single-image operations |
| `PipelineProcessor` | Batch processing with stream-based parallelism |

### Operator Layer

CUDA kernels and operators for specific image processing operations.

| Module | Operations |
|--------|------------|
| `PixelOperator` | Invert, grayscale, brightness |
| `ConvolutionEngine` | Gaussian blur, Sobel, general convolution |
| `HistogramCalculator` | Histogram computation, equalization |
| `ImageResizer` | Bilinear interpolation scaling |
| `Morphology` | Erosion, dilation, opening, closing |
| `Threshold` | Global, adaptive, Otsu thresholding |
| `ColorSpace` | RGB/HSV/YUV conversion |
| `Geometric` | Rotate, flip, affine, perspective |
| `Filters` | Median, bilateral, box, sharpen |

### Infrastructure Layer

Core utilities for GPU memory management and error handling.

| Component | Description |
|-----------|-------------|
| `DeviceBuffer` | RAII GPU memory management |
| `GpuImage` / `HostImage` | Image data structures |
| `StreamManager` | CUDA stream pool for async operations |
| `CudaError` | Error handling utilities |

---

## Technology Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++17, CUDA 14 |
| **Build System** | CMake 3.18+ |
| **CUDA** | 11.0+ (Recommended: 12.x) |
| **Testing** | Google Test v1.14.0 |
| **Benchmarking** | Google Benchmark v1.8.3 |
| **Image I/O** | stb (fetched via CMake) |
| **Documentation** | VitePress 1.5.0 + Mermaid |

---

## Design Principles

1. **Zero-Copy Optimization**: Minimize Host-Device data transfers
2. **Memory Reuse**: Reduce allocation overhead via memory pool
3. **Async Execution**: Overlap computation and transfer via CUDA Streams
4. **Modular Design**: Independent modules for easy extension and testing

---

## Module Index

| Module | Specification |
|--------|---------------|
| GPU Image Processing | [requirements.md](gpu-image-processing/requirements.md) |
| Design Details | [design.md](gpu-image-processing/design.md) |
| API Reference | [api.md](gpu-image-processing/api.md) |

---

## Project Structure

```
mini-opencv/
├── include/gpu_image/          # Public headers
│   ├── core/                   # DeviceBuffer, GpuImage, CudaError
│   ├── operators/              # CUDA operator interfaces
│   ├── processing/             # ImageProcessor, PipelineProcessor
│   └── io/                     # ImageIO
├── src/                        # Implementations
│   ├── core/                   # Infrastructure implementations
│   ├── operators/              # CUDA kernels (.cu files)
│   └── processing/             # High-level API implementations
├── tests/                      # Test suite
├── examples/                   # Example programs
├── benchmarks/                 # Performance benchmarks
├── openspec/                   # Specifications (OpenSpec)
│   ├── specs/                  # Main specifications
│   └── changes/                # Change proposals
└── docs/                       # User documentation
```

---

## Related Documents

- [GPU Image Processing Requirements](gpu-image-processing/requirements.md)
- [GPU Image Processing Design](gpu-image-processing/design.md)
- [GPU Image Processing API](gpu-image-processing/api.md)
