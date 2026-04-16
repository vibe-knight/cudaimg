#include "gpu_image/cuda_error.hpp"
#include "gpu_image/stream_manager.hpp"

namespace gpu_image {

StreamManager& StreamManager::instance() {
  static StreamManager instance;
  return instance;
}

StreamManager::StreamManager() { createStreams(DEFAULT_STREAM_COUNT); }

StreamManager::~StreamManager() { destroyStreams(); }

void StreamManager::createStreams(int count) {
  std::lock_guard<std::mutex> lock(mutex_);

  for (int i = 0; i < count; ++i) {
    cudaStream_t stream;
    CUDA_CHECK(cudaStreamCreate(&stream));
    streams_.push_back(stream);
    availableStreams_.push(stream);
  }
}

void StreamManager::destroyStreams() {
  std::lock_guard<std::mutex> lock(mutex_);

  for (cudaStream_t stream : streams_) {
    cudaStreamDestroy(stream);
  }
  streams_.clear();
  while (!availableStreams_.empty()) {
    availableStreams_.pop();
  }
}

cudaStream_t StreamManager::acquireStream() {
  std::lock_guard<std::mutex> lock(mutex_);

  if (availableStreams_.empty()) {
    // 如果没有可用的 stream，创建一个新的
    cudaStream_t stream;
    CUDA_CHECK(cudaStreamCreate(&stream));
    streams_.push_back(stream);
    return stream;
  }

  cudaStream_t stream = availableStreams_.front();
  availableStreams_.pop();
  return stream;
}

void StreamManager::releaseStream(cudaStream_t stream) {
  std::lock_guard<std::mutex> lock(mutex_);
  availableStreams_.push(stream);
}

void StreamManager::synchronizeAll() {
  std::lock_guard<std::mutex> lock(mutex_);

  for (cudaStream_t stream : streams_) {
    CUDA_CHECK(cudaStreamSynchronize(stream));
  }
}

void StreamManager::synchronize(cudaStream_t stream) {
  CUDA_CHECK(cudaStreamSynchronize(stream));
}

} // namespace gpu_image
