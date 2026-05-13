# Architecture Refactoring Summary

**Date:** 2026-05-13
**Scope:** Deep architectural improvements for testability and AI-navigability

## Overview

This refactoring addresses five key architectural issues identified through depth analysis:

1. **Shallow ImageProcessor** → Deep module with integrated execution policy
2. **Scattered output allocation** → Centralized ImageAllocator
3. **Exposed CUDA streams** → Hidden behind ExecutionPolicy abstraction
4. **ImageUtils location confusion** → Properly modularized
5. **No operator composability** → ImageOperator interface + OperatorPipeline

## Files Changed

### New Files (7)

| File | Purpose |
|------|---------|
| `include/gpu_image/core/execution_context.hpp` | ExecutionPolicy + ImageAllocator + ExecutionContext |
| `include/gpu_image/core/image_utils.hpp` | Modularized image utilities |
| `include/gpu_image/operators/image_operator.hpp` | Operator interface + pipeline + registry |
| `src/core/execution_context.cpp` | Implementation |
| `src/core/image_utils.cpp` | Implementation |
| `tests/core/test_execution_context.cpp` | New architecture tests |
| `docs/adr/0001-*.md` | Architecture decision record |

### Modified Files (12)

| File | Changes |
|------|---------|
| `include/gpu_image/core/gpu_image.hpp` | Removed ImageUtils namespace (moved to own header) |
| `include/gpu_image/core/kernel_helpers.hpp` | Updated to use new ImageUtils header |
| `include/gpu_image/processing/image_processor.hpp` | Complete rewrite with deep module design |
| `include/gpu_image/gpu_image_processing.hpp` | Added new headers |
| `src/processing/image_processor.cpp` | Complete rewrite |
| `src/operators/*.cu` (9 files) | Added `image_utils.hpp` include |
| `tests/processing/test_pipeline.cpp` | Updated API call |
| `CMakeLists.txt` | Added new source and test files |

### New Documentation (2)

| File | Purpose |
|------|---------|
| `CONTEXT.md` | Domain model and architectural depth analysis |
| `docs/adr/0001-*.md` | Architecture decision record |

## API Changes

### New APIs (Additive, Non-Breaking)

```cpp
// ExecutionPolicy - three modes
ExecutionPolicy::sync();
ExecutionPolicy::async();
ExecutionPolicy::batch();

// ExecutionContext - unified interface
ExecutionContext ctx(ExecutionPolicy::async());
GpuImage output = ctx.allocateOutput(input);
ctx.ensureOutputSize(input, output);
ctx.synchronize();

// ImageProcessor with modes
ImageProcessor processor(ImageProcessor::Mode::Async);
processor.setMemoryPooling(true);
bool complete = processor.isComplete();

// PipelineBuilder - fluent API
auto result = PipelineBuilder(processor)
    .start(input)
    .grayscale()
    .blur(5, 1.5f)
    .sobel()
    .execute();

// ImageOperator interface
class MyOperator : public UnaryOperator<MyOperator> {
  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx);
  OperatorTraits traits() const;
};

// OperatorPipeline - chain operators
OperatorPipeline pipeline;
pipeline.then<GrayscaleOperator>()
        .then<GaussianBlurOperator>(5, 1.0f)
        .then<SobelOperator>();
GpuImage result = pipeline.apply(input, ctx);

// OperatorRegistry - runtime creation
auto op = OperatorRegistry::instance().create("invert");
```

### Changed APIs (Minor)

| Old | New | Notes |
|-----|-----|-------|
| `processor.downloadImage(img)` | `processor.download(img)` | Renamed for consistency |

### Preserved APIs (Backward Compatible)

All existing Operator static methods remain unchanged:
```cpp
PixelOperator::invert(input, output, stream);
ConvolutionEngine::gaussianBlur(input, output, kernelSize, sigma, stream);
// ... all others unchanged
```

## Depth Analysis

### Before vs After

| Module | Before | After | Improvement |
|--------|--------|-------|-------------|
| ImageProcessor | 1:1 delegation | 20+ ops + context + pipeline | **High** |
| Stream handling | cudaStream_t exposed | ExecutionPolicy enum | **High** |
| Memory allocation | Scattered in 9 operators | ImageAllocator singleton | **High** |
| Operator composability | None | Interface + Pipeline | **High** |
| Testability | Must use GPU | Can mock ImageOperator | **High** |

### Deletion Test Results

| Module | If Deleted | Verdict |
|--------|------------|---------|
| ExecutionContext | Callers must manage streams/pools manually across N sites | **Deep** ✅ |
| ImageAllocator | Allocation logic spreads to N operators | **Deep** ✅ |
| OperatorPipeline | Chain logic duplicated in every caller | **Deep** ✅ |
| ImageProcessor (old) | Almost no complexity lost | **Shallow** (fixed) ✅ |

## Testing

### New Test Coverage

- `ExecutionContextTest` (12 tests)
  - ExecutionPolicy modes and move semantics
  - ExecutionContext creation and allocation
  - ImageAllocator singleton and pooling
  - ImageProcessor modes and completion

- `OperatorPipelineTest` (15 tests)
  - Pipeline construction and execution
  - Operator chaining
  - Registry and factory functions
  - PipelineBuilder fluent API
  - Operator traits and parameters

### Test Surface Analysis

| Layer | Testable Without GPU? | How |
|-------|----------------------|-----|
| ImageOperator interface | ✅ Yes | Mock `apply()` method |
| OperatorPipeline | ✅ Yes | Mock operators in chain |
| ExecutionPolicy | ✅ Yes | Test mode/ownership |
| ImageAllocator | ⚠️ Partial | Can test logic, not actual CUDA |
| Concrete operators | ❌ No | Require GPU execution |

## Migration Guide

### For Existing Code

No immediate changes required. The new layer is **additive**:

```cpp
// Old code still works
GpuImage output;
PixelOperator::invert(input, output);
cudaDeviceSynchronize();

// New code (optional)
ImageProcessor processor;
GpuImage output = processor.invert(input);
```

### For New Code

Prefer the high-level APIs:

```cpp
// Instead of manual stream management
cudaStream_t stream;
cudaStreamCreate(&stream);
PixelOperator::invert(input, output, stream);
cudaStreamSynchronize(stream);
cudaStreamDestroy(stream);

// Use ExecutionPolicy
ExecutionContext ctx(ExecutionPolicy::async());
PixelOperator::invert(input, output, ctx.stream());
ctx.synchronize();
```

## Future Opportunities

1. **Async operation queuing**: ExecutionContext could queue operations for batch execution
2. **Memory pool statistics**: Add `getStats()` to ImageAllocator for profiling
3. **Operator serialization**: Save/load pipelines from JSON
4. **Custom operator plugins**: Load operators from shared libraries via Registry
5. **Automatic kernel selection**: Choose optimal kernel based on image size

## References

- [CONTEXT.md](../CONTEXT.md) - Domain model
- [ADR-0001](./docs/adr/0001-execution-context-and-operator-abstraction.md) - Decision record
- [LANGUAGE.md](~/.claude/skills/improve-codebase-architecture/LANGUAGE.md) - Architecture vocabulary
