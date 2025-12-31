#pragma once

#include "gpu_image/gpu_image.hpp"
#include <cuda_runtime.h>
#include <array>

namespace gpu_image {

// 直方图计算器类
class HistogramCalculator {
public:
    static constexpr int NUM_BINS = 256;
    
    // 计算灰度直方图
    // 输入可以是单通道灰度图像或多通道图像（会先转换为灰度）
    static std::array<int, NUM_BINS> calculate(const GpuImage& input,
                                                cudaStream_t stream = nullptr);
    
    // 计算 RGB 各通道直方图
    static std::array<std::array<int, NUM_BINS>, 3> calculateRGB(
        const GpuImage& input, cudaStream_t stream = nullptr);
    
    // 计算单通道直方图（不做灰度转换）
    static std::array<int, NUM_BINS> calculateChannel(const GpuImage& input,
                                                       int channel,
                                                       cudaStream_t stream = nullptr);
    
    // 直方图均衡化
    static void equalize(const GpuImage& input, GpuImage& output,
                         cudaStream_t stream = nullptr);
};

} // namespace gpu_image
