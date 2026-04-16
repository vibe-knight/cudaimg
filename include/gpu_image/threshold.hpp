#pragma once

#include "gpu_image/gpu_image.hpp"
#include <cuda_runtime.h>

namespace gpu_image {

// 阈值类型
enum class ThresholdType {
  Binary,    // 二值化: dst = (src > thresh) ? maxVal : 0
  BinaryInv, // 反二值化: dst = (src > thresh) ? 0 : maxVal
  Truncate,  // 截断: dst = (src > thresh) ? thresh : src
  ToZero,    // 归零: dst = (src > thresh) ? src : 0
  ToZeroInv  // 反归零: dst = (src > thresh) ? 0 : src
};

// 自适应阈值方法
enum class AdaptiveMethod {
  MeanC,    // 均值
  GaussianC // 高斯加权
};

// 阈值操作类
class Threshold {
public:
  // 全局阈值
  static void threshold(const GpuImage& input, GpuImage& output,
                        unsigned char thresh, unsigned char maxVal = 255,
                        ThresholdType type = ThresholdType::Binary,
                        cudaStream_t stream = nullptr);

  // 自适应阈值
  static void adaptiveThreshold(const GpuImage& input, GpuImage& output,
                                unsigned char maxVal, AdaptiveMethod method,
                                ThresholdType type, int blockSize, int C,
                                cudaStream_t stream = nullptr);

  // Otsu 自动阈值
  static unsigned char otsuThreshold(const GpuImage& input,
                                     cudaStream_t stream = nullptr);

  // 使用 Otsu 方法进行二值化
  static void otsuBinarize(const GpuImage& input, GpuImage& output,
                           unsigned char maxVal = 255,
                           cudaStream_t stream = nullptr);
};

} // namespace gpu_image
