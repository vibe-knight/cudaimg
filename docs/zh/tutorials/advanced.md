# 高级主题

高级使用模式和优化技术。

## 内存管理

### 缓冲区复用

复用 `GpuImage` 对象避免重复分配：

```cpp
// 不好的做法：每次都分配新内存
for (int i = 0; i < 100; i++) {
    GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
}

// 好的做法：复用缓冲区
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
for (int i = 0; i < 99; i++) {
    processor.gaussianBlur(gpu, 5, 1.5f, result);
}
```

### 内存池

对于高吞吐量应用：

```cpp
MemoryPool pool;
DeviceBuffer* buffer = pool.allocate(size);
// ... 使用缓冲区 ...
pool.release(buffer);
```

## CUDA 流

### 多流处理

```cpp
// 创建流
std::vector<cudaStream_t> streams(4);
for (auto& s : streams) {
    cudaStreamCreate(&s);
}

// 并行处理
for (size_t i = 0; i < images.size(); i++) {
    auto stream = streams[i % streams.size()];
    processor.gaussianBlur(images[i], 5, 1.5f, stream);
}

// 同步所有流
for (auto& s : streams) {
    cudaStreamSynchronize(s);
}
```

## 内核优化

### 共享内存分块

对于自定义内核，使用共享内存：

```cpp
__global__ void myKernel(float* output, const float* input, int width) {
    extern __shared__ float sharedMem[];
    
    // 加载到共享内存
    int tid = threadIdx.x;
    int gid = blockIdx.x * blockDim.x + threadIdx.x;
    
    sharedMem[tid] = input[gid];
    __syncthreads();
    
    // 从共享内存计算
    // ...
}
```

### 纹理内存

对于插值密集型操作：

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

## 性能技巧

1. **批量操作**: 一起处理多张图像
2. **使用流**: 重叠计算和传输
3. **复用缓冲区**: 避免重复分配
4. **选择合适的内核大小**: 小内核更快
5. **先分析**: 使用 Nsight 识别瓶颈

## 调试

### 错误检查

```cpp
// 启用错误检查
#define GPU_IMAGE_CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA 错误: %s\n", cudaGetErrorString(err)); \
            exit(1); \
        } \
    } while(0)
```

### 内存跟踪

```cpp
// 跟踪内存使用
size_t free, total;
cudaMemGetInfo(&free, &total);
printf("GPU 内存: %zu MB 空闲 / %zu MB 总计\n", free/1024/1024, total/1024/1024);
```

## 下一步

- [常见问题](./faq) - 常见问题解答
- [架构设计](../architecture/overview) - 系统设计