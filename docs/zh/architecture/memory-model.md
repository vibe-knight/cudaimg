# 内存模型

了解 Mini-OpenCV 中的 GPU 内存管理。

## 概述

Mini-OpenCV 使用基于 RAII 的内存模型实现自动 GPU 内存管理。

## DeviceBuffer

`DeviceBuffer` 类提供 RAII 风格的 GPU 内存管理：

```cpp
class DeviceBuffer {
public:
    explicit DeviceBuffer(size_t size);
    ~DeviceBuffer();  // 自动调用 cudaFree
    
    void* data();
    size_t size() const;
    
private:
    void* d_data_;
    size_t size_;
};
```

### 关键特性

- **自动释放**: 对象超出作用域时自动释放内存
- **移动语义**: 高效的所有权转移
- **禁止拷贝**: 防止意外的深拷贝

## GpuImage

`GpuImage` 将 `DeviceBuffer` 与图像元数据封装：

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

## 内存池

对于性能关键应用，内存池减少分配开销：

```cpp
class MemoryPool {
public:
    DeviceBuffer* allocate(size_t size);
    void release(DeviceBuffer* buffer);
    
private:
    std::vector<std::unique_ptr<DeviceBuffer>> pool_;
};
```

## 最佳实践

1. **复用缓冲区**: 保持 `GpuImage` 对象存活以执行多次操作
2. **批量操作**: 在单个流水线中处理多张图像
3. **使用流水线**: `PipelineProcessor` 复用内部缓冲区

## 下一步

- [CUDA 流](./cuda-streams) - 异步执行
- [设计决策](./design-decisions) - 架构选择