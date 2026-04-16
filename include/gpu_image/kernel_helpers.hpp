#pragma once

#include <cuda_runtime.h>

namespace gpu_image {

// 默认 2D block 大小
inline constexpr int kBlockSize2D = 16;

// 默认 1D block 大小
inline constexpr int kBlockSize1D = 256;

// 计算 2D kernel 的 grid 和 block 配置
inline void calcGridBlock2D(int width, int height, dim3& grid, dim3& block) {
  block = dim3(kBlockSize2D, kBlockSize2D);
  grid =
      dim3(static_cast<unsigned>((width + kBlockSize2D - 1) / kBlockSize2D),
           static_cast<unsigned>((height + kBlockSize2D - 1) / kBlockSize2D));
}

// 计算 1D kernel 的 grid 和 block 配置
inline void calcGridBlock1D(int totalElements, int& gridSize, int& blockSize) {
  blockSize = kBlockSize1D;
  gridSize = (totalElements + blockSize - 1) / blockSize;
}

// 确保输出 GpuImage 与输入同尺寸同通道（若不匹配则重新分配）
inline void ensureOutputSize(const GpuImage& input, GpuImage& output) {
  if (output.width != input.width || output.height != input.height ||
      output.channels != input.channels) {
    output =
        ImageUtils::createGpuImage(input.width, input.height, input.channels);
  }
}

// 确保输出 GpuImage 为指定尺寸和通道
inline void ensureOutputSize(GpuImage& output, int width, int height,
                             int channels) {
  if (output.width != width || output.height != height ||
      output.channels != channels) {
    output = ImageUtils::createGpuImage(width, height, channels);
  }
}

// 验证输入图像有效，否则抛异常
inline void validateInput(const GpuImage& input,
                          const char* context = "Invalid input image") {
  if (!input.isValid()) {
    throw std::invalid_argument(context);
  }
}

// 验证两张图像尺寸匹配
inline void
validateSameSize(const GpuImage& a, const GpuImage& b,
                 const char* context = "Image dimensions must match") {
  if (a.width != b.width || a.height != b.height || a.channels != b.channels) {
    throw std::invalid_argument(context);
  }
}

} // namespace gpu_image
