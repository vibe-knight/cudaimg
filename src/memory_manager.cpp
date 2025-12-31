#include "gpu_image/memory_manager.hpp"
#include "gpu_image/cuda_error.hpp"
#include <cuda_runtime.h>

namespace gpu_image {

MemoryManager& MemoryManager::instance() {
    static MemoryManager instance;
    return instance;
}

MemoryManager::MemoryManager() = default;

MemoryManager::~MemoryManager() {
    clearPool();
}

size_t MemoryManager::alignSize(size_t size) {
    const size_t alignment = 256;
    return (size + alignment - 1) & ~(alignment - 1);
}

DeviceBuffer MemoryManager::allocate(size_t size) {
    if (size == 0) {
        return DeviceBuffer();
    }
    
    size_t alignedSize = alignSize(size);
    
    if (poolEnabled_) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = memoryPool_.find(alignedSize);
        if (it != memoryPool_.end() && !it->second.empty()) {
            void* ptr = it->second.back();
            it->second.pop_back();
            poolSize_ -= alignedSize;
            
            // 创建一个管理该指针的 DeviceBuffer
            DeviceBuffer buffer;
            // 使用移动赋值来设置内部状态
            // 注意：这里需要一个特殊的构造方式
            // 我们直接分配新的，因为 DeviceBuffer 不支持从原始指针构造
            cudaFree(ptr); // 释放池中的指针
            return DeviceBuffer(size); // 重新分配
        }
    }
    
    // 从 CUDA 分配新内存
    DeviceBuffer buffer(size);
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        totalAllocated_ += alignedSize;
        if (totalAllocated_ > peakUsage_) {
            peakUsage_ = totalAllocated_;
        }
    }
    
    return buffer;
}

void MemoryManager::deallocate(DeviceBuffer&& buffer) {
    if (!buffer.isValid()) {
        return;
    }
    
    // 由于 DeviceBuffer 的 RAII 设计，直接让它析构即可
    // 内存池功能在这个简化版本中不完全实现
    // buffer 会在函数结束时自动释放
}

void MemoryManager::clearPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pair : memoryPool_) {
        for (void* ptr : pair.second) {
            cudaFree(ptr);
        }
    }
    memoryPool_.clear();
    poolSize_ = 0;
}

MemoryStats MemoryManager::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return MemoryStats{totalAllocated_, poolSize_, peakUsage_};
}

} // namespace gpu_image
