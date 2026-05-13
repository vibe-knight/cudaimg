# Mini-OpenCV Domain Model

**Status:** Active
**Updated:** 2026-05-13

## Core Domain Concepts

### Image (GpuImage / HostImage)

An image is a rectangular grid of pixels with associated metadata.

**Properties:**
- `width`: Horizontal dimension in pixels
- `height`: Vertical dimension in pixels
- `channels`: Color depth (1=gray, 3=RGB, 4=RGBA)
- `buffer`: Memory storage (DeviceBuffer for GPU, vector for Host)

**Invariants:**
- Valid images have width > 0, height > 0, channels in {1, 3, 4}
- Buffer size must equal width × height × channels

### Operator

An operator transforms one image into another.

**Interface:**
- `apply(input, ctx) -> output`: Transform input to output
- `applyInPlace(image, ctx)`: Transform in-place (optional)
- `traits()`: Metadata about behavior

**Categories:**
- **Pixel**: Point-wise operations (invert, brightness, grayscale)
- **Convolution**: Kernel-based filtering (blur, sobel, edge detection)
- **Morphology**: Structuring element operations (erode, dilate)
- **Geometric**: Spatial transformations (resize, rotate, flip)
- **Histogram**: Statistical operations (histogram, equalize)
- **ColorSpace**: Color conversions (RGB↔HSV, RGB↔YUV)
- **Threshold**: Binarization (global, adaptive, Otsu)
- **Filters**: Non-linear filtering (median, bilateral)

### ExecutionContext

A context bundles execution policy and memory management.

**Responsibilities:**
- Stream management (sync/async/batch modes)
- Output buffer allocation
- Memory pooling control

**Interface:**
- `stream()`: Get CUDA stream for kernel launches
- `allocateOutput(input)`: Create matching output buffer
- `ensureOutputSize(input, output)`: Resize output if needed
- `synchronize()`: Wait for async operations
- `release(image)`: Return buffer to pool

### ExecutionPolicy

A policy defines how operations are executed.

**Modes:**
- **Sync**: Auto-synchronize after each operation
- **Async**: Caller controls synchronization
- **Batch**: Queue operations, sync all at once

**Depth Benefit:**
Callers use simple enums instead of understanding `cudaStream_t`.

### ImageAllocator

Centralized memory management for images.

**Interface:**
- `allocate(width, height, channels)`: Create new image
- `ensureSize(input, output)`: Match output to input
- `release(image)`: Return to pool

**Depth Benefit:**
All allocation logic concentrated in one module.

## Architectural Depth

### Deep Modules (High Leverage)

| Module | Interface | Implementation | Depth |
|--------|-----------|----------------|-------|
| `ExecutionContext` | 5 methods | Stream + pool + policy | High |
| `ImageProcessor` | 20+ operations | All operators + context | High |
| `OperatorPipeline` | `then<T>()`, `apply()` | N-operator chain | High |
| `ImageAllocator` | 3 methods | Pool + allocation | Medium-High |

### Shallow Modules (Low Leverage)

| Module | Interface | Implementation | Depth |
|--------|-----------|----------------|-------|
| Individual operators | 1-2 static methods | Thin kernel wrapper | Low |
| `ImageUtils` | Utility namespace | Passthrough to allocator | Low |

### Depth Principle Applied

1. **Deletion Test**: If `ExecutionContext` is deleted, callers must manage streams, pools, and sync manually — complexity explodes across N call sites. ✅ Deep module.

2. **Interface is Test Surface**: `ImageOperator::apply()` can be mocked. Tests verify pipeline logic without GPU execution. ✅ Testable seam.

3. **Two Adapters = Real Seam**: `ExecutionPolicy::Sync` and `ExecutionPolicy::Async` are two adapters — the seam is real. ✅ Extensible.

## Module Boundaries

```
┌─────────────────────────────────────────────────────────────┐
│                    User Application                          │
│                  ImageProcessor / Pipeline                   │
├─────────────────────────────────────────────────────────────┤
│              Operator Abstraction Layer                      │
│        ImageOperator  ·  OperatorPipeline  ·  Registry       │
├─────────────────────────────────────────────────────────────┤
│              Concrete Operators                              │
│  Invert  ·  Grayscale  ·  Blur  ·  Sobel  ·  Resize  ·  ... │
├─────────────────────────────────────────────────────────────┤
│              Execution Infrastructure                        │
│  ExecutionContext  ·  ExecutionPolicy  ·  ImageAllocator    │
├─────────────────────────────────────────────────────────────┤
│              Core Infrastructure                             │
│  DeviceBuffer  ·  GpuImage/HostImage  ·  MemoryManager      │
└─────────────────────────────────────────────────────────────┘
```

## Naming Conventions

| Concept | Pattern | Example |
|---------|---------|---------|
| Operator classes | `*Operator` | `InvertOperator`, `GaussianBlurOperator` |
| Operator factories | `make*` | `makeInvert()`, `makeGaussianBlur(5, 1.0f)` |
| Execution modes | `Mode::*` | `Mode::Sync`, `Mode::Async` |
| Context methods | verb | `allocateOutput()`, `synchronize()` |
| Pipeline methods | `then<T>()` | `pipeline.then<GrayscaleOperator>()` |

## Design Decisions

1. **ExecutionContext over raw streams**: Callers should never need to understand `cudaStream_t`.

2. **Operator interface for extensibility**: New operators implement `ImageOperator` and automatically work in pipelines.

3. **Memory pooling is opt-in**: Backward compatibility with existing code that expects fresh allocations.

4. **PipelineBuilder for fluent API**: `PipelineBuilder(processor).start(input).grayscale().blur().execute()` is more readable than nested function calls.

5. **OperatorRegistry for dynamic creation**: Enables runtime operator discovery (useful for scripting/plugins).
