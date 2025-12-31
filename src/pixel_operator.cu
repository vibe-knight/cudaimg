#include "gpu_image/pixel_operator.hpp"
#include "gpu_image/cuda_error.hpp"
#include <stdexcept>

namespace gpu_image {

// CUDA Kernels
__global__ void invertKernel(const unsigned char* input, unsigned char* output,
                              int width, int height, int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        int idx = (y * width + x) * channels;
        for (int c = 0; c < channels; ++c) {
            output[idx + c] = 255 - input[idx + c];
        }
    }
}

__global__ void invertInPlaceKernel(unsigned char* data, int width, int height, 
                                     int channels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        int idx = (y * width + x) * channels;
        for (int c = 0; c < channels; ++c) {
            data[idx + c] = 255 - data[idx + c];
        }
    }
}

__global__ void toGrayscaleKernel(const unsigned char* input, unsigned char* output,
                                   int width, int height, int inputChannels) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        int inIdx = (y * width + x) * inputChannels;
        int outIdx = y * width + x;
        
        // 使用标准灰度转换公式: 0.299*R + 0.587*G + 0.114*B
        float gray = 0.299f * input[inIdx] + 
                     0.587f * input[inIdx + 1] + 
                     0.114f * input[inIdx + 2];
        
        output[outIdx] = static_cast<unsigned char>(min(max(gray + 0.5f, 0.0f), 255.0f));
    }
}

__global__ void adjustBrightnessKernel(const unsigned char* input, 
                                        unsigned char* output,
                                        int width, int height, int channels,
                                        int offset) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        int idx = (y * width + x) * channels;
        for (int c = 0; c < channels; ++c) {
            int value = input[idx + c] + offset;
            output[idx + c] = static_cast<unsigned char>(min(max(value, 0), 255));
        }
    }
}

__global__ void adjustBrightnessInPlaceKernel(unsigned char* data,
                                               int width, int height, int channels,
                                               int offset) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        int idx = (y * width + x) * channels;
        for (int c = 0; c < channels; ++c) {
            int value = data[idx + c] + offset;
            data[idx + c] = static_cast<unsigned char>(min(max(value, 0), 255));
        }
    }
}

// 辅助函数：计算 grid 和 block 大小
static void calculateGridBlock(int width, int height, dim3& grid, dim3& block) {
    block = dim3(16, 16);
    grid = dim3((width + block.x - 1) / block.x, 
                (height + block.y - 1) / block.y);
}

// PixelOperator 实现
void PixelOperator::invert(const GpuImage& input, GpuImage& output,
                           cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    
    // 确保输出图像大小正确
    if (output.width != input.width || output.height != input.height ||
        output.channels != input.channels) {
        output = ImageUtils::createGpuImage(input.width, input.height, input.channels);
    }
    
    dim3 grid, block;
    calculateGridBlock(input.width, input.height, grid, block);
    
    invertKernel<<<grid, block, 0, stream>>>(
        input.buffer.dataAs<unsigned char>(),
        output.buffer.dataAs<unsigned char>(),
        input.width, input.height, input.channels
    );
    
    CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::invertInPlace(GpuImage& image, cudaStream_t stream) {
    if (!image.isValid()) {
        throw std::invalid_argument("Invalid image");
    }
    
    dim3 grid, block;
    calculateGridBlock(image.width, image.height, grid, block);
    
    invertInPlaceKernel<<<grid, block, 0, stream>>>(
        image.buffer.dataAs<unsigned char>(),
        image.width, image.height, image.channels
    );
    
    CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::toGrayscale(const GpuImage& input, GpuImage& output,
                                cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    if (input.channels < 3) {
        throw std::invalid_argument("Input must have at least 3 channels for grayscale conversion");
    }
    
    // 输出是单通道
    if (output.width != input.width || output.height != input.height ||
        output.channels != 1) {
        output = ImageUtils::createGpuImage(input.width, input.height, 1);
    }
    
    dim3 grid, block;
    calculateGridBlock(input.width, input.height, grid, block);
    
    toGrayscaleKernel<<<grid, block, 0, stream>>>(
        input.buffer.dataAs<unsigned char>(),
        output.buffer.dataAs<unsigned char>(),
        input.width, input.height, input.channels
    );
    
    CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::adjustBrightness(const GpuImage& input, GpuImage& output,
                                      int offset, cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    
    if (output.width != input.width || output.height != input.height ||
        output.channels != input.channels) {
        output = ImageUtils::createGpuImage(input.width, input.height, input.channels);
    }
    
    dim3 grid, block;
    calculateGridBlock(input.width, input.height, grid, block);
    
    adjustBrightnessKernel<<<grid, block, 0, stream>>>(
        input.buffer.dataAs<unsigned char>(),
        output.buffer.dataAs<unsigned char>(),
        input.width, input.height, input.channels, offset
    );
    
    CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::adjustBrightnessInPlace(GpuImage& image, int offset,
                                             cudaStream_t stream) {
    if (!image.isValid()) {
        throw std::invalid_argument("Invalid image");
    }
    
    dim3 grid, block;
    calculateGridBlock(image.width, image.height, grid, block);
    
    adjustBrightnessInPlaceKernel<<<grid, block, 0, stream>>>(
        image.buffer.dataAs<unsigned char>(),
        image.width, image.height, image.channels, offset
    );
    
    CUDA_CHECK(cudaGetLastError());
}

// ImageUtils 实现
namespace ImageUtils {

GpuImage createGpuImage(int width, int height, int channels) {
    if (!validateImageParams(width, height, channels)) {
        throw std::invalid_argument("Invalid image parameters");
    }
    
    GpuImage image;
    image.width = width;
    image.height = height;
    image.channels = channels;
    image.buffer = DeviceBuffer(image.totalBytes());
    
    return image;
}

GpuImage uploadToGpu(const HostImage& hostImage) {
    if (!hostImage.isValid()) {
        throw std::invalid_argument("Invalid host image");
    }
    
    GpuImage gpuImage = createGpuImage(hostImage.width, hostImage.height, 
                                        hostImage.channels);
    gpuImage.buffer.copyFromHost(hostImage.data.data(), hostImage.totalBytes());
    
    return gpuImage;
}

HostImage downloadFromGpu(const GpuImage& gpuImage) {
    if (!gpuImage.isValid()) {
        throw std::invalid_argument("Invalid GPU image");
    }
    
    HostImage hostImage = createHostImage(gpuImage.width, gpuImage.height,
                                           gpuImage.channels);
    gpuImage.buffer.copyToHost(hostImage.data.data(), hostImage.totalBytes());
    
    return hostImage;
}

void uploadToGpuAsync(const HostImage& hostImage, GpuImage& gpuImage,
                      cudaStream_t stream) {
    if (!hostImage.isValid()) {
        throw std::invalid_argument("Invalid host image");
    }
    
    if (gpuImage.width != hostImage.width || gpuImage.height != hostImage.height ||
        gpuImage.channels != hostImage.channels) {
        gpuImage = createGpuImage(hostImage.width, hostImage.height, 
                                   hostImage.channels);
    }
    
    gpuImage.buffer.copyFromHostAsync(hostImage.data.data(), 
                                       hostImage.totalBytes(), stream);
}

void downloadFromGpuAsync(const GpuImage& gpuImage, HostImage& hostImage,
                          cudaStream_t stream) {
    if (!gpuImage.isValid()) {
        throw std::invalid_argument("Invalid GPU image");
    }
    
    if (hostImage.width != gpuImage.width || hostImage.height != gpuImage.height ||
        hostImage.channels != gpuImage.channels) {
        hostImage = createHostImage(gpuImage.width, gpuImage.height,
                                     gpuImage.channels);
    }
    
    gpuImage.buffer.copyToHostAsync(hostImage.data.data(),
                                     hostImage.totalBytes(), stream);
}

HostImage createHostImage(int width, int height, int channels) {
    if (!validateImageParams(width, height, channels)) {
        throw std::invalid_argument("Invalid image parameters");
    }
    
    HostImage image;
    image.width = width;
    image.height = height;
    image.channels = channels;
    image.data.resize(image.totalBytes());
    
    return image;
}

bool validateImageParams(int width, int height, int channels) {
    return width > 0 && height > 0 && 
           (channels == 1 || channels == 3 || channels == 4);
}

} // namespace ImageUtils

} // namespace gpu_image
