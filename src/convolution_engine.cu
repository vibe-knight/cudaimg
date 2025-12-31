#include "gpu_image/convolution_engine.hpp"
#include "gpu_image/cuda_error.hpp"
#include <cmath>
#include <stdexcept>

namespace gpu_image {

// 常量内存用于存储卷积核
__constant__ float d_kernel[49];  // 最大支持 7x7 卷积核

// 使用 Shared Memory 的卷积 Kernel
template<int BLOCK_SIZE, int MAX_KERNEL_SIZE>
__global__ void convolveKernelShared(
    const unsigned char* input, unsigned char* output,
    int width, int height, int channels, int kernelSize, int borderMode) {
    
    // Shared memory 大小：block + halo
    const int halo = kernelSize / 2;
    const int sharedSize = BLOCK_SIZE + 2 * halo;
    
    extern __shared__ float sharedMem[];
    
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    int x = blockIdx.x * BLOCK_SIZE + tx;
    int y = blockIdx.y * BLOCK_SIZE + ty;
    
    // 对每个通道处理
    for (int c = 0; c < channels; ++c) {
        // 加载数据到 shared memory（包括 halo 区域）
        for (int dy = ty; dy < sharedSize; dy += BLOCK_SIZE) {
            for (int dx = tx; dx < sharedSize; dx += BLOCK_SIZE) {
                int srcX = blockIdx.x * BLOCK_SIZE + dx - halo;
                int srcY = blockIdx.y * BLOCK_SIZE + dy - halo;
                
                float value = 0.0f;
                
                // 边界处理
                if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                    value = input[(srcY * width + srcX) * channels + c];
                } else if (borderMode == 1) {  // Mirror
                    int mirrorX = srcX < 0 ? -srcX : (srcX >= width ? 2 * width - srcX - 2 : srcX);
                    int mirrorY = srcY < 0 ? -srcY : (srcY >= height ? 2 * height - srcY - 2 : srcY);
                    mirrorX = max(0, min(mirrorX, width - 1));
                    mirrorY = max(0, min(mirrorY, height - 1));
                    value = input[(mirrorY * width + mirrorX) * channels + c];
                } else if (borderMode == 2) {  // Replicate
                    int clampX = max(0, min(srcX, width - 1));
                    int clampY = max(0, min(srcY, height - 1));
                    value = input[(clampY * width + clampX) * channels + c];
                }
                // borderMode == 0 (Zero) 保持 value = 0
                
                sharedMem[dy * sharedSize + dx] = value;
            }
        }
        
        __syncthreads();
        
        // 执行卷积
        if (x < width && y < height) {
            float sum = 0.0f;
            
            for (int ky = 0; ky < kernelSize; ++ky) {
                for (int kx = 0; kx < kernelSize; ++kx) {
                    int sx = tx + kx;
                    int sy = ty + ky;
                    sum += sharedMem[sy * sharedSize + sx] * d_kernel[ky * kernelSize + kx];
                }
            }
            
            // 裁剪到 [0, 255]
            sum = fminf(fmaxf(sum, 0.0f), 255.0f);
            output[(y * width + x) * channels + c] = static_cast<unsigned char>(sum + 0.5f);
        }
        
        __syncthreads();
    }
}

// 简单卷积 Kernel（不使用 shared memory，用于小图像或调试）
__global__ void convolveKernelSimple(
    const unsigned char* input, unsigned char* output,
    int width, int height, int channels, int kernelSize) {
    
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= width || y >= height) return;
    
    int halo = kernelSize / 2;
    
    for (int c = 0; c < channels; ++c) {
        float sum = 0.0f;
        
        for (int ky = 0; ky < kernelSize; ++ky) {
            for (int kx = 0; kx < kernelSize; ++kx) {
                int srcX = x + kx - halo;
                int srcY = y + ky - halo;
                
                float value = 0.0f;
                if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                    value = input[(srcY * width + srcX) * channels + c];
                }
                
                sum += value * d_kernel[ky * kernelSize + kx];
            }
        }
        
        sum = fminf(fmaxf(sum, 0.0f), 255.0f);
        output[(y * width + x) * channels + c] = static_cast<unsigned char>(sum + 0.5f);
    }
}

// Sobel 边缘检测 Kernel
__global__ void sobelKernel(
    const unsigned char* input, unsigned char* output,
    int width, int height, int channels) {
    
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x >= width || y >= height) return;
    
    // Sobel 核
    const float sobelX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    const float sobelY[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    
    // 对于多通道图像，先转换为灰度
    float grayValues[9];
    
    for (int ky = -1; ky <= 1; ++ky) {
        for (int kx = -1; kx <= 1; ++kx) {
            int srcX = x + kx;
            int srcY = y + ky;
            
            float gray = 0.0f;
            if (srcX >= 0 && srcX < width && srcY >= 0 && srcY < height) {
                if (channels >= 3) {
                    int idx = (srcY * width + srcX) * channels;
                    gray = 0.299f * input[idx] + 0.587f * input[idx + 1] + 0.114f * input[idx + 2];
                } else {
                    gray = input[(srcY * width + srcX) * channels];
                }
            }
            grayValues[(ky + 1) * 3 + (kx + 1)] = gray;
        }
    }
    
    // 计算梯度
    float gx = 0.0f, gy = 0.0f;
    for (int i = 0; i < 9; ++i) {
        gx += grayValues[i] * sobelX[i];
        gy += grayValues[i] * sobelY[i];
    }
    
    // 计算梯度幅值
    float magnitude = sqrtf(gx * gx + gy * gy);
    magnitude = fminf(magnitude, 255.0f);
    
    // 输出为单通道
    output[y * width + x] = static_cast<unsigned char>(magnitude + 0.5f);
}

// ConvolutionEngine 实现
void ConvolutionEngine::convolve(const GpuImage& input, GpuImage& output,
                                  const float* kernel, int kernelSize,
                                  BorderMode borderMode, cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    if (kernelSize < 1 || kernelSize > 7 || kernelSize % 2 == 0) {
        throw std::invalid_argument("Kernel size must be odd and between 1 and 7");
    }
    if (kernel == nullptr) {
        throw std::invalid_argument("Kernel is null");
    }
    
    // 确保输出图像大小正确
    if (output.width != input.width || output.height != input.height ||
        output.channels != input.channels) {
        output = ImageUtils::createGpuImage(input.width, input.height, input.channels);
    }
    
    // 复制卷积核到常量内存
    CUDA_CHECK(cudaMemcpyToSymbol(d_kernel, kernel, 
                                   kernelSize * kernelSize * sizeof(float)));
    
    dim3 block(16, 16);
    dim3 grid((input.width + block.x - 1) / block.x,
              (input.height + block.y - 1) / block.y);
    
    int borderModeInt = static_cast<int>(borderMode);
    
    // 使用简单版本（更稳定）
    convolveKernelSimple<<<grid, block, 0, stream>>>(
        input.buffer.dataAs<unsigned char>(),
        output.buffer.dataAs<unsigned char>(),
        input.width, input.height, input.channels, kernelSize
    );
    
    CUDA_CHECK(cudaGetLastError());
}

void ConvolutionEngine::gaussianBlur(const GpuImage& input, GpuImage& output,
                                      int kernelSize, float sigma,
                                      cudaStream_t stream) {
    if (kernelSize < 1 || kernelSize > 7 || kernelSize % 2 == 0) {
        throw std::invalid_argument("Kernel size must be odd and between 1 and 7");
    }
    
    std::vector<float> kernel = generateGaussianKernel(kernelSize, sigma);
    convolve(input, output, kernel.data(), kernelSize, BorderMode::Zero, stream);
}

void ConvolutionEngine::sobelEdgeDetection(const GpuImage& input, GpuImage& output,
                                            cudaStream_t stream) {
    if (!input.isValid()) {
        throw std::invalid_argument("Invalid input image");
    }
    
    // 输出为单通道
    if (output.width != input.width || output.height != input.height ||
        output.channels != 1) {
        output = ImageUtils::createGpuImage(input.width, input.height, 1);
    }
    
    dim3 block(16, 16);
    dim3 grid((input.width + block.x - 1) / block.x,
              (input.height + block.y - 1) / block.y);
    
    sobelKernel<<<grid, block, 0, stream>>>(
        input.buffer.dataAs<unsigned char>(),
        output.buffer.dataAs<unsigned char>(),
        input.width, input.height, input.channels
    );
    
    CUDA_CHECK(cudaGetLastError());
}

std::vector<float> ConvolutionEngine::generateGaussianKernel(int size, float sigma) {
    if (size < 1 || size % 2 == 0) {
        throw std::invalid_argument("Kernel size must be positive and odd");
    }
    
    std::vector<float> kernel(size * size);
    int half = size / 2;
    float sum = 0.0f;
    
    for (int y = -half; y <= half; ++y) {
        for (int x = -half; x <= half; ++x) {
            float value = std::exp(-(x * x + y * y) / (2.0f * sigma * sigma));
            kernel[(y + half) * size + (x + half)] = value;
            sum += value;
        }
    }
    
    // 归一化
    for (float& v : kernel) {
        v /= sum;
    }
    
    return kernel;
}

std::vector<float> ConvolutionEngine::generateGaussianKernel1D(int size, float sigma) {
    if (size < 1 || size % 2 == 0) {
        throw std::invalid_argument("Kernel size must be positive and odd");
    }
    
    std::vector<float> kernel(size);
    int half = size / 2;
    float sum = 0.0f;
    
    for (int x = -half; x <= half; ++x) {
        float value = std::exp(-(x * x) / (2.0f * sigma * sigma));
        kernel[x + half] = value;
        sum += value;
    }
    
    // 归一化
    for (float& v : kernel) {
        v /= sum;
    }
    
    return kernel;
}

void ConvolutionEngine::separableConvolve(const GpuImage& input, GpuImage& output,
                                           const float* rowKernel, const float* colKernel,
                                           int kernelSize, cudaStream_t stream) {
    // 简化实现：使用两次 1D 卷积
    // 这里暂时使用 2D 卷积作为替代
    std::vector<float> kernel2D(kernelSize * kernelSize);
    for (int y = 0; y < kernelSize; ++y) {
        for (int x = 0; x < kernelSize; ++x) {
            kernel2D[y * kernelSize + x] = rowKernel[x] * colKernel[y];
        }
    }
    convolve(input, output, kernel2D.data(), kernelSize, BorderMode::Zero, stream);
}

} // namespace gpu_image
