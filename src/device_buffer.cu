#include "gpu_image/cuda_error.hpp"
#include "gpu_image/device_buffer.hpp"
#include <utility>

namespace gpu_image {

DeviceBuffer::DeviceBuffer(size_t size) : devicePtr_(nullptr), size_(size) {
  if (size > 0) {
    CUDA_CHECK(cudaMalloc(&devicePtr_, size));
  }
}

DeviceBuffer DeviceBuffer::fromRaw(void* ptr, size_t size) noexcept {
  DeviceBuffer buf;
  buf.devicePtr_ = ptr;
  buf.size_ = size;
  return buf;
}

DeviceBuffer::~DeviceBuffer() { release(); }

DeviceBuffer::DeviceBuffer(DeviceBuffer&& other) noexcept
    : devicePtr_(other.devicePtr_), size_(other.size_) {
  other.devicePtr_ = nullptr;
  other.size_ = 0;
}

DeviceBuffer& DeviceBuffer::operator=(DeviceBuffer&& other) noexcept {
  if (this != &other) {
    release();
    devicePtr_ = other.devicePtr_;
    size_ = other.size_;
    other.devicePtr_ = nullptr;
    other.size_ = 0;
  }
  return *this;
}

void DeviceBuffer::copyFromHost(const void* hostPtr, size_t copySize) {
  if (hostPtr == nullptr) {
    throw std::invalid_argument("Host pointer is null");
  }
  if (copySize > size_) {
    throw std::invalid_argument("Copy size exceeds buffer size");
  }
  CUDA_CHECK(cudaMemcpy(devicePtr_, hostPtr, copySize, cudaMemcpyHostToDevice));
}

void DeviceBuffer::copyToHost(void* hostPtr, size_t copySize) const {
  if (hostPtr == nullptr) {
    throw std::invalid_argument("Host pointer is null");
  }
  if (copySize > size_) {
    throw std::invalid_argument("Copy size exceeds buffer size");
  }
  CUDA_CHECK(cudaMemcpy(hostPtr, devicePtr_, copySize, cudaMemcpyDeviceToHost));
}

void DeviceBuffer::copyFromHostAsync(const void* hostPtr, size_t copySize,
                                     cudaStream_t stream) {
  if (hostPtr == nullptr) {
    throw std::invalid_argument("Host pointer is null");
  }
  if (copySize > size_) {
    throw std::invalid_argument("Copy size exceeds buffer size");
  }
  CUDA_CHECK(cudaMemcpyAsync(devicePtr_, hostPtr, copySize,
                             cudaMemcpyHostToDevice, stream));
}

void DeviceBuffer::copyToHostAsync(void* hostPtr, size_t copySize,
                                   cudaStream_t stream) const {
  if (hostPtr == nullptr) {
    throw std::invalid_argument("Host pointer is null");
  }
  if (copySize > size_) {
    throw std::invalid_argument("Copy size exceeds buffer size");
  }
  CUDA_CHECK(cudaMemcpyAsync(hostPtr, devicePtr_, copySize,
                             cudaMemcpyDeviceToHost, stream));
}

void DeviceBuffer::release() {
  if (devicePtr_ != nullptr) {
    cudaFree(devicePtr_);
    devicePtr_ = nullptr;
    size_ = 0;
  }
}

std::pair<void*, size_t> DeviceBuffer::detach() noexcept {
  auto result = std::make_pair(devicePtr_, size_);
  devicePtr_ = nullptr;
  size_ = 0;
  return result;
}

} // namespace gpu_image
