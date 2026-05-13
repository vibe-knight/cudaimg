# GpuImage

GPU 内存图像容器。

## 概述

`GpuImage` 将 GPU 内存与图像元数据封装，提供安全高效的方式管理 GPU 上的图像数据。

## 属性

```cpp
int width() const;      // 图像宽度（像素）
int height() const;     // 图像高度（像素）
int channels() const;   // 颜色通道数（1、3 或 4）
size_t size() const;    // 总字节数
bool empty() const;     // 无数据时为 true
```

## 创建

GpuImage 对象通常由 `ImageProcessor` 创建：

```cpp
ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);
```

## 内存管理

### RAII 模式

当 `GpuImage` 超出作用域时自动释放内存：

```cpp
{
    GpuImage gpu = processor.loadFromHost(host);
    // 使用 gpu...
}  // 内存在此释放
```

### 移动语义

```cpp
GpuImage gpu1 = processor.loadFromHost(host);
GpuImage gpu2 = std::move(gpu1);  // gpu1 现在为空
```

## 配合操作使用

```cpp
GpuImage gpu = processor.loadFromHost(host);

// 操作返回新的 GpuImage
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);

// 下载到主机
HostImage result = processor.downloadImage(blurred);
```

## 内部结构

```cpp
class GpuImage {
private:
    int width_;
    int height_;
    int channels_;
    std::unique_ptr<DeviceBuffer> buffer_;
};
```

## 另见

- [DeviceBuffer](./device-buffer) - GPU 内存管理
- [ImageProcessor](./image-processor) - 主 API