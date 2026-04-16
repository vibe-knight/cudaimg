#pragma once

#include <cuda_runtime.h>
#include <mutex>
#include <queue>
#include <vector>

namespace gpu_image {

// CUDA Stream 管理器单例类
class StreamManager {
public:
  static StreamManager& instance();

  // 禁用拷贝和移动
  StreamManager(const StreamManager&) = delete;
  StreamManager& operator=(const StreamManager&) = delete;

  // 获取一个可用的 stream
  cudaStream_t acquireStream();

  // 释放 stream 回池
  void releaseStream(cudaStream_t stream);

  // 同步所有 streams
  void synchronizeAll();

  // 同步指定 stream
  void synchronize(cudaStream_t stream);

  // 获取默认 stream 数量
  int getStreamCount() const { return static_cast<int>(streams_.size()); }

private:
  StreamManager();
  ~StreamManager();

  void createStreams(int count);
  void destroyStreams();

  std::vector<cudaStream_t> streams_;
  std::queue<cudaStream_t> availableStreams_;
  std::mutex mutex_;

  static constexpr int DEFAULT_STREAM_COUNT = 4;
};

// RAII Stream 包装器
class ScopedStream {
public:
  ScopedStream() : stream_(StreamManager::instance().acquireStream()) {}
  ~ScopedStream() { StreamManager::instance().releaseStream(stream_); }

  ScopedStream(const ScopedStream&) = delete;
  ScopedStream& operator=(const ScopedStream&) = delete;

  cudaStream_t get() const { return stream_; }
  operator cudaStream_t() const { return stream_; }

private:
  cudaStream_t stream_;
};

} // namespace gpu_image
