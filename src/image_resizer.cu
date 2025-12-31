#include "gpu_image/image_resizer.hpp"
#include "gpu_image/cuda_error.hpp"
#include <stdexcept>
#include <algorithm>

namespace gpu_image {

// 双线性插值 device 函数
__device__ float bilinearInterpolate(
    const unsigned char* src, int srcWidth, int srcHeight, int channels,
    float srcX, float srcY, int channel) {
    
    // 计算四个相邻像素的坐标
    int x0 = static_cast<int>(floorf(srcX));
    int y0 = static_cast<int>(floorf(srcY));
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    // 裁剪到有效范围
    x0 = max(0, min(x0, srcWidth - 1));
    y0 = max(0, min(y0, srcHeight - 1));
    x1 = max(0, min(x1, srcWidth - 1));
    y1 = max(0, min(y1, srcHeight - 1));
    
    // 计算插值权重
    float dx = srcX - floorf(srcX);
    float dy = srcY - floorf(srcY);
    
    // 获取四个像素值
    float v00 = src[(y0 * srcWidth + x0) * channels + channel];
    float v10 = src[(y0 * srcWidth + x1) * channels + channel];
    float v01 = src[(y1 * srcWidth + x0) * channels + channel];
    float v11 = src[(y1 * srcWidth + x1) * channels + channel];
    
    // 双线性插值
    float v0 = v00 * (1.0f - dx) + v10 * dx;
    float v1 = v01 * (1.0f - dx) + v11 * dx;
    
    return v0 * (1.0f - dy) + v1 * dy;
}

// 最近邻插值 device 函数
__device__ unsigned char nearestNeighborInterpolate(
    const unsigned char* src, int srcWidth, int srcHeight, int channels,
    float srcX, float srcY, int channel) {
    
    int x = static_cast<int>(roundf(srcX));
    int y = static_cast<int>(roundf(srcY));
    
    x = max(0, min(x, srcWidth - 1));
    y = max(0, min(y, srcHeight - 1));
    
    return src[(y * srcWidth + x) * channels + channel];
}

// 双线性插值缩放 Kernel
__global__ void resizeBilinearKernel(
    const unsigned char* input, unsigned char* output,
    int srcWidth, int srcHeight, int dstWidth, int dstHeight, int channels) {
    
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= dstWidth || y >= dstHeight) return;
    
    // 计算源图像中的对应位置
    float srcX = (x + 0.5f) * srcWidth / dstWidth - 0.5f;
    float srcY = (y + 0.5f) * srcHeight / dstHeight - 0.5f;
    
    // 确保在有效范围内
    srcX = fmaxf(0.0f, fminf(srcX, srcWidth - 1.0f));
    srcY = fmaxf(0.0f, fminf(srcY, srcHeight - 1.0f));
    
    for (int c = 0; c < channels; ++c) {
        float value = bilinearInterpolate(input, srcWidth, srcHeight, channels,
                                          srcX, srcY, c);
        value = fminf(fmaxf(value, 0.0f), 255.0f);
        output[(y * dstWidth + x) * channels + c] = static_cast<unsigned char>(value + 0.5f);
    }
}

// 最近邻插值缩放 Kernel
__global__ void resizeNearestKernel(
    const unsigned char* input, unsigned char* output,
    int srcWidth, int srcHeight, int dstWidth, int dstHeight, int channels) {
    
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= dstWidth || y >= dstHeight) return;
    
    float srcX = (x + 0.5f) * srcWidth / dstWidth - 0.5f;
    float srcY = (y + 0.5f) * srcHeight / dstHeight - 0.5f;
    
    for (int c = 0; c < channels; ++c) {
        unsigned char value = nearestNeighborInterpolate(
            input, srcWidth, srcHeight, channels, srcX, srcY, c);
        output[(y * dstWidth + x) * channels + c] = value;
    }
}

// ImageResizer 实现
void ImageResizer::resize(const GpuImage& input, GpuImage& output,
                          int newWidth, int newHeight,
                          InterpolationMode mode, cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    if (newWidth <= 0 || newHeight <= 0) {
        throw std::invalid_argument("Invalid output dimensions");
    }
    
    // 确保输出图像大小正确
    if (output.width != newWidth || output.height != newHeight ||
        output.channels != input.channels) {
        output = ImageUtils::createGpuImage(newWidth, newHeight, input.channels);
    }
    
    dim3 block(16, 16);
    dim3 grid((newWidth + block.x - 1) / block.x,
              (newHeight + block.y - 1) / block.y);
    
    switch (mode) {
        case InterpolationMode::NearestNeighbor:
            resizeNearestKernel<<<grid, block, 0, stream>>>(
                input.buffer.dataAs<unsigned char>(),
                output.buffer.dataAs<unsigned char>(),
                input.width, input.height, newWidth, newHeight, input.channels
            );
            break;
            
        case InterpolationMode::Bilinear:
        case InterpolationMode::Bicubic:  // 暂时使用双线性代替
        default:
            resizeBilinearKernel<<<grid, block, 0, stream>>>(
                input.buffer.dataAs<unsigned char>(),
                output.buffer.dataAs<unsigned char>(),
                input.width, input.height, newWidth, newHeight, input.channels
            );
            break;
    }
    
    CUDA_CHECK(cudaGetLastError());
}

void ImageResizer::resizeByScale(const GpuImage& input, GpuImage& output,
                                  float scaleX, float scaleY,
                                  InterpolationMode mode, cudaStream_t stream) {
    if (scaleX <= 0 || scaleY <= 0) {
        throw std::invalid_argument("Scale factors must be positive");
    }
    
    int newWidth = static_cast<int>(input.width * scaleX + 0.5f);
    int newHeight = static_cast<int>(input.height * scaleY + 0.5f);
    
    newWidth = std::max(1, newWidth);
    newHeight = std::max(1, newHeight);
    
    resize(input, output, newWidth, newHeight, mode, stream);
}

void ImageResizer::resizeFit(const GpuImage& input, GpuImage& output,
                              int maxWidth, int maxHeight,
                              InterpolationMode mode, cudaStream_t stream) {
    if (maxWidth <= 0 || maxHeight <= 0) {
        throw std::invalid_argument("Max dimensions must be positive");
    }
    
    float scaleX = static_cast<float>(maxWidth) / input.width;
    float scaleY = static_cast<float>(maxHeight) / input.height;
    float scale = std::min(scaleX, scaleY);
    
    int newWidth = static_cast<int>(input.width * scale + 0.5f);
    int newHeight = static_cast<int>(input.height * scale + 0.5f);
    
    newWidth = std::max(1, std::min(newWidth, maxWidth));
    newHeight = std::max(1, std::min(newHeight, maxHeight));
    
    resize(input, output, newWidth, newHeight, mode, stream);
}

} // namespace gpu_image
