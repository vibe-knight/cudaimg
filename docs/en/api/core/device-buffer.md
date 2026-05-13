# DeviceBuffer

RAII GPU memory management.

## Overview

`DeviceBuffer` provides automatic GPU memory allocation and deallocation using RAII pattern.

## Constructor

```cpp
explicit DeviceBuffer(size_t size);
```

Allocates `size` bytes on GPU.

## Destructor

```cpp
~DeviceBuffer();
```

Automatically calls `cudaFree()`.

## Methods

### data

```cpp
void* data();
const void* data() const;
```

Returns pointer to GPU memory.

### size

```cpp
size_t size() const;
```

Returns allocated size in bytes.

## Move Semantics

```cpp
DeviceBuffer buf1(1024);
DeviceBuffer buf2 = std::move(buf1);  // buf1 is now empty
```

Copy is disabled to prevent accidental deep copies.

## Example

```cpp
// Allocate GPU memory
DeviceBuffer buffer(1920 * 1080 * 3);  // 4K RGB image

// Use with CUDA kernel
myKernel<<<grid, block>>>(buffer.data());

// Memory automatically freed when buffer goes out of scope
```

## Internal Implementation

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

## See Also

- [GpuImage](./gpu-image) - Image container using DeviceBuffer
