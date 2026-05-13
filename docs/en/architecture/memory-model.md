# Memory Model

Understanding GPU memory management in Mini-OpenCV.

## Overview

Mini-OpenCV uses a RAII-based memory model for automatic GPU memory management.

## DeviceBuffer

The `DeviceBuffer` class provides RAII-style GPU memory management:

```cpp
class DeviceBuffer {
public:
    explicit DeviceBuffer(size_t size);
    ~DeviceBuffer();  // Automatically calls cudaFree
    
    void* data();
    size_t size() const;
    
private:
    void* d_data_;
    size_t size_;
};
```

### Key Features

- **Automatic Deallocation**: Memory freed when object goes out of scope
- **Move Semantics**: Efficient transfer of ownership
- **No Copy**: Prevents accidental deep copies

## GpuImage

`GpuImage` wraps a `DeviceBuffer` with image metadata:

```cpp
class GpuImage {
public:
    int width() const;
    int height() const;
    int channels() const;
    size_t bufferSize() const;
    
    DeviceBuffer& buffer();
    
private:
    int width_, height_, channels_;
    std::unique_ptr<DeviceBuffer> buffer_;
};
```

## Memory Pool

For performance-critical applications, memory pools reduce allocation overhead:

```cpp
class MemoryPool {
public:
    DeviceBuffer* allocate(size_t size);
    void release(DeviceBuffer* buffer);
    
private:
    std::vector<std::unique_ptr<DeviceBuffer>> pool_;
};
```

## Best Practices

1. **Reuse Buffers**: Keep `GpuImage` objects alive for multiple operations
2. **Batch Operations**: Process multiple images in a single pipeline
3. **Use Pipelines**: `PipelineProcessor` reuses internal buffers

## Next Steps

- [CUDA Streams](./cuda-streams) - Async execution
- [Design Decisions](./design-decisions) - Architecture choices
