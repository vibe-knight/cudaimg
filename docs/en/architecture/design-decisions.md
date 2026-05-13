# Design Decisions

Key architectural decisions and their rationale.

## Decision 1: RAII Memory Management

**Status**: Accepted

**Context**: GPU memory must be explicitly allocated and freed.

**Decision**: Use RAII pattern with `DeviceBuffer` class.

**Consequences**:
- ✅ No memory leaks
- ✅ Exception-safe
- ✅ Clear ownership semantics

## Decision 2: Three-Layer Architecture

**Status**: Accepted

**Context**: Need balance between simplicity and flexibility.

**Decision**: Separate into Application, Operator, and Infrastructure layers.

**Consequences**:
- ✅ Clear separation of concerns
- ✅ Easy to add new operators
- ✅ Testable components

## Decision 3: Shared Memory Tiling for Convolution

**Status**: Accepted

**Context**: Convolution is memory-bandwidth bound.

**Decision**: Use shared memory tiling to cache image data.

**Consequences**:
- ✅ 10× faster than naive implementation
- ⚠️ Kernel size limited to ~15×15

## Decision 4: Texture Memory for Resize

**Status**: Accepted

**Context**: Image resize needs interpolation.

**Decision**: Use CUDA texture memory with hardware interpolation.

**Consequences**:
- ✅ Free hardware bilinear interpolation
- ✅ Cached texture reads
- ⚠️ Limited to 2D images

## Decision 5: Atomic Operations for Histogram

**Status**: Accepted

**Context**: Histogram requires counting across threads.

**Decision**: Use atomic operations with shared memory reduction.

**Consequences**:
- ✅ Correct parallel histogram
- ⚠️ Some atomic contention

## See Also

- [Architecture Overview](./overview)
- [Memory Model](./memory-model)
- [CUDA Streams](./cuda-streams)
