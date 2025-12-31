#pragma once

#include <cuda_runtime.h>
#include <cstddef>

namespace gpu_image {

// 管理单个 GPU 内存缓冲区的生命周期
class DeviceBuffer {
public:
    // 默认构造函数（空缓冲区）
    DeviceBuffer() : devicePtr_(nullptr), size_(0) {}
    
    // 构造函数：分配指定大小的 Device 内存
    explicit DeviceBuffer(size_t size);
    
    // 析构函数：自动释放 Device 内存
    ~DeviceBuffer();
    
    // 禁用拷贝
    DeviceBuffer(const DeviceBuffer&) = delete;
    DeviceBuffer& operator=(const DeviceBuffer&) = delete;
    
    // 启用移动语义
    DeviceBuffer(DeviceBuffer&& other) noexcept;
    DeviceBuffer& operator=(DeviceBuffer&& other) noexcept;
    
    // 从 Host 复制数据到 Device
    void copyFromHost(const void* hostPtr, size_t size);
    
    // 从 Device 复制数据到 Host
    void copyToHost(void* hostPtr, size_t size) const;
    
    // 异步版本（使用指定的 stream）
    void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
    void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;
    
    // 获取 Device 指针
    void* data() { return devicePtr_; }
    const void* data() const { return devicePtr_; }
    
    // 模板版本
    template<typename T>
    T* dataAs() { return static_cast<T*>(devicePtr_); }
    
    template<typename T>
    const T* dataAs() const { return static_cast<const T*>(devicePtr_); }
    
    // 获取缓冲区大小
    size_t size() const { return size_; }
    
    // 检查是否有效
    bool isValid() const { return devicePtr_ != nullptr && size_ > 0; }
    
    // 释放内存
    void release();

private:
    void* devicePtr_;
    size_t size_;
};

} // namespace gpu_image
