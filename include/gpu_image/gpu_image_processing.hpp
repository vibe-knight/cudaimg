#pragma once

// GPU Image Processing Library
// 基于 CUDA 的高性能图像处理库

#include "gpu_image/cuda_error.hpp"
#include "gpu_image/device_buffer.hpp"
#include "gpu_image/memory_manager.hpp"
#include "gpu_image/stream_manager.hpp"
#include "gpu_image/gpu_image.hpp"
#include "gpu_image/pixel_operator.hpp"
#include "gpu_image/convolution_engine.hpp"
#include "gpu_image/histogram_calculator.hpp"
#include "gpu_image/image_resizer.hpp"
#include "gpu_image/image_processor.hpp"
#include "gpu_image/pipeline_processor.hpp"
#include "gpu_image/morphology.hpp"
#include "gpu_image/threshold.hpp"
#include "gpu_image/color_space.hpp"
#include "gpu_image/geometric.hpp"

namespace gpu_image {

// 库版本信息
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

// 获取版本字符串
inline const char* getVersionString() {
    return "1.0.0";
}

// 检查 CUDA 是否可用
inline bool isCudaAvailable() {
    int deviceCount = 0;
    cudaError_t err = cudaGetDeviceCount(&deviceCount);
    return err == cudaSuccess && deviceCount > 0;
}

// 获取 CUDA 设备信息
inline std::string getDeviceInfo() {
    int deviceCount = 0;
    cudaGetDeviceCount(&deviceCount);
    
    if (deviceCount == 0) {
        return "No CUDA devices available";
    }
    
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, 0);
    
    std::string info = "Device: ";
    info += prop.name;
    info += ", Compute Capability: ";
    info += std::to_string(prop.major) + "." + std::to_string(prop.minor);
    info += ", Memory: ";
    info += std::to_string(prop.totalGlobalMem / (1024 * 1024)) + " MB";
    
    return info;
}

} // namespace gpu_image
