# Design: GPU Image Processing Library

**Status:** Archived (Completed)
**Created:** 2024
**Archived:** 2026-04-23

## Architecture

This design uses a layered architecture separating memory management, compute kernels, and high-level APIs.

```
┌─────────────────────────────────────────────────────────────┐
│                    High-Level API Layer                      │
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

## Design Principles

1. **Zero-Copy Optimization**: Minimize Host-Device data transfers
2. **Memory Reuse**: Reduce allocation overhead via memory pool
3. **Async Execution**: Overlap computation and transfer via CUDA Streams
4. **Modular Design**: Independent modules for easy extension and testing

## Key Components

### Memory Management Layer
- `DeviceBuffer`: RAII GPU memory management
- `MemoryManager`: Memory pool singleton
- `StreamManager`: CUDA stream pool

### Processing Layer
- `PixelOperator`: Pixel-level operations
- `ConvolutionEngine`: Convolution with shared memory optimization
- `HistogramCalculator`: Atomic operations + parallel reduction
- `ImageResizer`: Bilinear interpolation

### High-Level API Layer
- `ImageProcessor`: Main user-facing API
- `PipelineProcessor`: Batch async processing

## Full Design Document

See the main design document at [openspec/specs/gpu-image-processing/design.md](../../specs/gpu-image-processing/design.md) for complete details.
