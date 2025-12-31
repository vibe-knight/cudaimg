#pragma once

#include "gpu_image/gpu_image.hpp"
#include <cuda_runtime.h>

namespace gpu_image {

// 像素级操作类
class PixelOperator {
public:
    // 图像反色
    // 对每个像素执行: output[i] = 255 - input[i]
    static void invert(const GpuImage& input, GpuImage& output,
                       cudaStream_t stream = nullptr);
    
    // RGB 转灰度
    // 使用公式: gray = 0.299*R + 0.587*G + 0.114*B
    static void toGrayscale(const GpuImage& input, GpuImage& output,
                            cudaStream_t stream = nullptr);
    
    // 亮度调整
    // 对每个像素执行: output[i] = clamp(input[i] + offset, 0, 255)
    static void adjustBrightness(const GpuImage& input, GpuImage& output,
                                  int offset, cudaStream_t stream = nullptr);
    
    // 原地版本
    static void invertInPlace(GpuImage& image, cudaStream_t stream = nullptr);
    static void adjustBrightnessInPlace(GpuImage& image, int offset,
                                         cudaStream_t stream = nullptr);
};

} // namespace gpu_image
