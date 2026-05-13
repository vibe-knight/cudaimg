# GpuImage

GPU memory image container.

## Overview

`GpuImage` wraps GPU memory with image metadata, providing a safe and efficient way to manage image data on the GPU.

## Properties

```cpp
int width() const;      // Image width in pixels
int height() const;     // Image height in pixels
int channels() const;   // Number of color channels (1, 3, or 4)
size_t size() const;    // Total bytes
bool empty() const;     // True if no data
```

## Creation

GpuImage objects are typically created by `ImageProcessor`:

```cpp
ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);
```

## Memory Management

### RAII Pattern

Memory is automatically freed when `GpuImage` goes out of scope:

```cpp
{
    GpuImage gpu = processor.loadFromHost(host);
    // Use gpu...
}  // Memory freed here
```

### Move Semantics

```cpp
GpuImage gpu1 = processor.loadFromHost(host);
GpuImage gpu2 = std::move(gpu1);  // gpu1 is now empty
```

## Usage with Operations

```cpp
GpuImage gpu = processor.loadFromHost(host);

// Operations return new GpuImage
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);

// Download to host
HostImage result = processor.downloadImage(blurred);
```

## Internal Structure

```cpp
class GpuImage {
private:
    int width_;
    int height_;
    int channels_;
    std::unique_ptr<DeviceBuffer> buffer_;
};
```

## See Also

- [DeviceBuffer](./device-buffer) - GPU memory management
- [ImageProcessor](./image-processor) - Main API
