#pragma once

#include "gpu_image/device_buffer.hpp"
#include <cstddef>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace gpu_image {

// 内存使用统计
struct MemoryStats {
  size_t totalAllocated;
  size_t poolSize;
  size_t peakUsage;
};

// 内存管理器单例类
class MemoryManager {
public:
  static MemoryManager& instance();

  // 禁用拷贝和移动
  MemoryManager(const MemoryManager&) = delete;
  MemoryManager& operator=(const MemoryManager&) = delete;

  // 分配 Device 内存（可能从池中获取）
  DeviceBuffer allocate(size_t size);

  // 释放内存回池
  void deallocate(DeviceBuffer&& buffer);

  // 清空内存池
  void clearPool();

  // 获取当前 GPU 内存使用统计
  MemoryStats getStats() const;

  // 配置
  void setMaxPoolSize(size_t maxSize) { maxPoolSize_ = maxSize; }
  void setPoolEnabled(bool enabled) { poolEnabled_ = enabled; }

private:
  MemoryManager();
  ~MemoryManager();

  // 将大小对齐到 256 字节边界
  static size_t alignSize(size_t size);

  std::unordered_map<size_t, std::vector<void*>> memoryPool_;
  mutable std::mutex mutex_;

  size_t totalAllocated_ = 0;
  size_t poolSize_ = 0;
  size_t peakUsage_ = 0;
  size_t maxPoolSize_ = 512 * 1024 * 1024; // 512 MB
  bool poolEnabled_ = true;
};

} // namespace gpu_image
