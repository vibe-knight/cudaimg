#pragma once

#include "gpu_image/gpu_image.hpp"
#include <cuda_runtime.h>

namespace gpu_image {

// 颜色空间转换类
class ColorSpace {
public:
  // RGB <-> HSV
  static void rgbToHsv(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
  static void hsvToRgb(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);

  // RGB <-> YUV
  static void rgbToYuv(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
  static void yuvToRgb(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);

  // RGB <-> Lab (CIE L*a*b*)
  static void rgbToLab(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
  static void labToRgb(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);

  // 通道分离
  static void splitChannels(const GpuImage& input, GpuImage& channel0,
                            GpuImage& channel1, GpuImage& channel2,
                            cudaStream_t stream = nullptr);

  // 通道合并
  static void mergeChannels(const GpuImage& channel0, const GpuImage& channel1,
                            const GpuImage& channel2, GpuImage& output,
                            cudaStream_t stream = nullptr);
};

} // namespace gpu_image
