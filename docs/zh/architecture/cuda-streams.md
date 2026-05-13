# CUDA 流

使用 CUDA 流实现异步执行。

## 概述

CUDA 流可以重叠计算和数据传输，提高吞吐量。

## 流基础

CUDA 流是按顺序执行的操作序列：

```cpp
cudaStream_t stream;
cudaStreamCreate(&stream);

// 异步操作
cudaMemcpyAsync(d_dst, h_src, size, cudaMemcpyHostToDevice, stream);
kernel<<<grid, block, 0, stream>>>(...);
cudaMemcpyAsync(h_dst, d_src, size, cudaMemcpyDeviceToHost, stream);

cudaStreamDestroy(stream);
```

## 多流流水线

Mini-OpenCV 使用多个流实现流水线：

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

`PipelineProcessor` 类处理多流执行：

```cpp
PipelineProcessor processor(4);  // 4 个流

// 并发处理多张图像
for (auto& image : images) {
    auto stream = processor.getStream();
    processor.gaussianBlur(image, 5, 1.5f, stream);
}

processor.synchronize();
```

## 性能收益

| 场景 | 单流 | 多流 | 提升 |
|------|------|------|------|
| 10 张图像 | 50 ms | 35 ms | **30%** |
| 100 张图像 | 500 ms | 280 ms | **44%** |

## 最佳实践

1. **使用 2-4 个流**: 更多流收益递减
2. **批量相似操作**: 分组相同内核调用
3. **边界同步**: 仅在必要时同步

## 下一步

- [设计决策](./design-decisions) - 为什么选择这种方式
- [性能基准](../benchmarks/) - 性能数据