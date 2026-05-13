# CUDA Streams

Asynchronous execution using CUDA streams.

## Overview

CUDA streams enable overlapping computation and data transfer for higher throughput.

## Stream Basics

A CUDA stream is a sequence of operations that execute in order:

```cpp
cudaStream_t stream;
cudaStreamCreate(&stream);

// Async operations
cudaMemcpyAsync(d_dst, h_src, size, cudaMemcpyHostToDevice, stream);
kernel<<<grid, block, 0, stream>>>(...);
cudaMemcpyAsync(h_dst, d_src, size, cudaMemcpyDeviceToHost, stream);

cudaStreamDestroy(stream);
```

## Multi-Stream Pipeline

Mini-OpenCV uses multiple streams for pipelining:

```cpp
class StreamManager {
public:
    cudaStream_t getStream();
    void synchronize();
    
private:
    std::vector<cudaStream_t> streams_;
    size_t nextStream_ = 0;
};
```

## PipelineProcessor

The `PipelineProcessor` class handles multi-stream execution:

```cpp
PipelineProcessor processor(4);  // 4 streams

// Process multiple images concurrently
for (auto& image : images) {
    auto stream = processor.getStream();
    processor.gaussianBlur(image, 5, 1.5f, stream);
}

processor.synchronize();
```

## Performance Benefits

| Scenario | Single Stream | Multi-Stream | Improvement |
|----------|---------------|--------------|-------------|
| 10 images | 50 ms | 35 ms | **30%** |
| 100 images | 500 ms | 280 ms | **44%** |

## Best Practices

1. **Use 2-4 streams**: More streams have diminishing returns
2. **Batch similar operations**: Group same kernel calls
3. **Synchronize at boundaries**: Only sync when necessary

## Next Steps

- [Design Decisions](./design-decisions) - Why we chose this approach
- [Benchmarks](../benchmarks/) - Performance data
