# Advanced Topics

Advanced usage patterns and optimization techniques.

## Memory Management

### Buffer Reuse

Reuse `GpuImage` objects to avoid reallocation:

```cpp
// Bad: Allocates new memory each time
for (int i = 0; i < 100; i++) {
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
}

// Good: Reuse buffer
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
for (int i = 0; i < 99; i++) {
    processor.gaussianBlur(gpu, 5, 1.5f, result);
}
```

### Memory Pool

For high-throughput applications:

```cpp
MemoryPool pool;
DeviceBuffer* buffer = pool.allocate(size);
// ... use buffer ...
pool.release(buffer);
```

## CUDA Streams

### Multi-Stream Processing

```cpp
// Create streams
std::vector<cudaStream_t> streams(4);
for (auto& s : streams) {
    cudaStreamCreate(&s);
}

// Process in parallel
for (size_t i = 0; i < images.size(); i++) {
    auto stream = streams[i % streams.size()];
    processor.gaussianBlur(images[i], 5, 1.5f, stream);
}

// Synchronize all streams
for (auto& s : streams) {
    cudaStreamSynchronize(s);
}
```

## Kernel Optimization

### Shared Memory Tiling

For custom kernels, use shared memory:

```cpp
__global__ void myKernel(float* output, const float* input, int width) {
    extern __shared__ float sharedMem[];
    
    // Load to shared memory
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    
    sharedMem[tid] = input[gid];
    __syncthreads();
    
    // Compute from shared memory
    // ...
}
```

### Texture Memory

For interpolation-heavy operations:

```cpp
texture<float, 2, cudaReadModeElementType> texRef;

__global__ void resizeKernel(float* output, int outWidth, int outHeight) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < outWidth && y < outHeight) {
        float u = (float)x / outWidth;
        float v = (float)y / outHeight;
        output[y * outWidth + x] = tex2D(texRef, u, v);
    }
}
```

## Performance Tips

1. **Batch operations**: Process multiple images together
2. **Use streams**: Overlap compute and transfer
3. **Reuse buffers**: Avoid repeated allocations
4. **Choose right kernel size**: Smaller kernels are faster
5. **Profile first**: Use Nsight to identify bottlenecks

## Debugging

### Error Checking

```cpp
// Enable error checking
#define GPU_IMAGE_CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA error: %s\n", cudaGetErrorString(err)); \
            exit(1); \
        } \
    } while(0)
```

### Memory Tracking

```cpp
// Track memory usage
size_t free, total;
cudaMemGetInfo(&free, &total);
printf("GPU Memory: %zu MB free / %zu MB total\n", free/1024/1024, total/1024/1024);
```

## Next Steps

- [FAQ](./faq) - Common questions
- [Architecture](../architecture/overview) - System design
