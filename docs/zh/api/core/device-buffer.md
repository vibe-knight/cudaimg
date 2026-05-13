# DeviceBuffer

RAII GPU 内存管理。

## 概述

`DeviceBuffer` 使用 RAII 模式提供自动 GPU 内存分配和释放。

## 构造函数

```cpp
explicit DeviceBuffer(size_t size);
```

在 GPU 上分配 `size` 字节。

## 析构函数

```cpp
~DeviceBuffer();
```

自动调用 `cudaFree()`。

## 方法

### data

```cpp
void* data();
const void* data() const;
```

返回 GPU 内存指针。

### size

```cpp
size_t size() const;
```

返回分配的字节数。

## 移动语义

```cpp
DeviceBuffer buf1(1024);
DeviceBuffer buf2 = std::move(buf1);  // buf1 现在为空
```

禁止拷贝以防止意外的深拷贝。

## 示例

```cpp
// 分配 GPU 内存
DeviceBuffer buffer(1920 * 1080 * 3);  // 4K RGB 图像

// 配合 CUDA 内核使用
myKernel<<<grid, block>>>(buffer.data());

// buffer 超出作用域时自动释放内存
```

## 内部实现

```cpp
class DeviceBuffer {
public:
    explicit DeviceBuffer(size_t size);
    ~DeviceBuffer();
    
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;
    
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;
    
private:
    void* d_data_;
    size_t size_;
};
```

## 另见

- [GpuImage](./gpu-image) - 使用 DeviceBuffer 的图像容器