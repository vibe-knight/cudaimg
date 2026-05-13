# ADR-0001: Execution Context and Operator Abstraction

**Status:** Accepted
**Date:** 2026-05-13

## Context

The original mini-opencv architecture had several structural issues:

1. **Shallow ImageProcessor**: Every method was a pure passthrough — `Operator::method(); cudaDeviceSynchronize(); return output;`. No state, no memory management, no execution policy.

2. **Scattered output allocation**: Every operator duplicated the pattern:
   ```cpp
   if (output.width != input.width || ...) {
     output = ImageUtils::createGpuImage(...);
   }
   ```

3. **CUDA streams exposed to callers**: Every operator method had `cudaStream_t stream = nullptr` — callers needed CUDA knowledge for async operation.

4. **ImageUtils location confusion**: Declared in `gpu_image.hpp`, implemented in `pixel_operator.cu`, creating circular dependency feelings.

5. **No operator composability**: Operators were static method collections — no polymorphism, no pipelines, no runtime discovery.

## Decision

### 1. Introduce ExecutionContext (ExecutionPolicy + ImageAllocator)

Create a unified module that bundles execution strategy and memory management:

- **ExecutionPolicy**: Three modes (Sync/Async/Batch) hiding `cudaStream_t`
- **ImageAllocator**: Centralized allocation with optional pooling
- **ExecutionContext**: Composition of both, primary interface for operators

### 2. Modularize ImageUtils

Extract `ImageUtils` from `gpu_image.hpp` into its own header/implementation pair:
- `include/gpu_image/core/image_utils.hpp`
- `src/core/image_utils.cpp`

Delegate allocation to `ImageAllocator`, removing direct `DeviceBuffer` construction.

### 3. Deepen ImageProcessor

Transform from passthrough facade to real deep module:
- Integrated `ExecutionContext` for stream and memory management
- Mode-aware auto-sync (Sync mode auto-syncs, Async/Batch don't)
- `PipelineBuilder` for fluent operation chaining
- `pipeline()` method for functional-style composition

### 4. Introduce ImageOperator Interface

Create an operator abstraction layer:
- `ImageOperator`: Base interface with `apply()`, `applyInPlace()`, `traits()`
- `UnaryOperator<Derived>`: CRTP base for single-input operators
- `OperatorPipeline`: Chain of operators with automatic intermediate buffer management
- `OperatorRegistry`: Runtime operator creation by name
- Factory functions: `makeInvert()`, `makeGaussianBlur()`, etc.

## Consequences

### Positive

- **Callers don't need CUDA knowledge**: `ExecutionPolicy::sync()` vs `cudaStream_t`
- **Memory pooling is centralized**: One `ImageAllocator` controls all allocation
- **Operators are composable**: `OperatorPipeline().then<A>().then<B>()`
- **Operators are mockable**: Tests can inject mock `ImageOperator` implementations
- **Pipeline tests work without GPU**: Mock the `apply()` method

### Negative

- **ABI surface increased**: New classes add complexity to the header surface
- **Virtual dispatch overhead**: `ImageOperator::apply()` adds vtable indirection (negligible for GPU operations)
- **Backward compatibility**: Existing code using `PixelOperator::invert(input, output)` still works — the new layer is additive

### Risk Mitigation

- Old Operator static methods remain unchanged — this is additive, not breaking
- `ImageUtils` namespace still works as before, just delegates to `ImageAllocator`
- `ExecutionPolicy` defaults to Sync mode — existing sync behavior preserved

## Module Depth Analysis

| Module | Before Depth | After Depth | Reason |
|--------|-------------|-------------|--------|
| ImageProcessor | Shallow (1:1 delegation) | Deep (20+ ops + context + pipeline) | Real behavior behind simple interface |
| ImageUtils | Medium (scattered) | Deep (centralized allocation) | All allocation through one module |
| Operator layer | Flat (static methods) | Deep (interface + composition) | Polymorphism enables pipelines |
| Stream handling | Exposed (cudaStream_t) | Hidden (ExecutionPolicy) | 3 modes vs CUDA primitives |

## References

- [CONTEXT.md](../../CONTEXT.md) - Domain model and architectural depth analysis
- [LANGUAGE.md](~/.claude/skills/improve-codebase-architecture/LANGUAGE.md) - Architecture vocabulary
