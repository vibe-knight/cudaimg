#pragma once

#include "gpu_image/device_buffer.hpp"
#include <cstddef>
#include <vector>
#include <memory>

namespace gpu_image {

// GPU 上的图像表示
struct GpuImage {
    DeviceBuffer buffer;
    int width = 0;
    int height = 0;
    int channels = 0;  // 1 for grayscale, 3 for RGB, 4 for RGBA
    
    // 计算每行字节数
    size_t pitch() const { return static_cast<size_t>(width) * channels; }
    
    // 计算总字节数
    size_t totalBytes() const { 
        return static_cast<size_t>(width) * height * channels; 
    }
    
    // 检查是否有效
    bool isValid() const {
        return buffer.isValid() && width > 0 && height > 0 && 
               (channels == 1 || channels == 3 || channels == 4);
    }
    
    // 获取像素数量
    size_t pixelCount() const {
        return static_cast<size_t>(width) * height;
    }
};

// Host 端图像数据
struct HostImage {
    std::vector<unsigned char> data;
    int width = 0;
    int height = 0;
    int channels = 0;
    
    // 访问像素
    unsigned char& at(int x, int y, int c) {
        return data[(static_cast<size_t>(y) * width + x) * channels + c];
    }
    
    const unsigned char& at(int x, int y, int c) const {
        return data[(static_cast<size_t>(y) * width + x) * channels + c];
    }
    
    // 计算总字节数
    size_t totalBytes() const {
        return static_cast<size_t>(width) * height * channels;
    }
    
    // 检查是否有效
    bool isValid() const {
        return !data.empty() && width > 0 && height > 0 &&
               (channels == 1 || channels == 3 || channels == 4) &&
               data.size() == totalBytes();
    }
};

// 图像辅助函数
namespace ImageUtils {
    // 创建 GpuImage
    GpuImage createGpuImage(int width, int height, int channels);
    
    // 从 HostImage 创建 GpuImage
    GpuImage uploadToGpu(const HostImage& hostImage);
    
    // 从 GpuImage 下载到 HostImage
    HostImage downloadFromGpu(const GpuImage& gpuImage);
    
    // 异步版本
    void uploadToGpuAsync(const HostImage& hostImage, GpuImage& gpuImage, 
                          cudaStream_t stream);
    void downloadFromGpuAsync(const GpuImage& gpuImage, HostImage& hostImage,
                              cudaStream_t stream);
    
    // 创建空的 HostImage
    HostImage createHostImage(int width, int height, int channels);
    
    // 验证图像参数
    bool validateImageParams(int width, int height, int channels);
}

} // namespace gpu_image
