#include "gpu_image/core/cuda_error.hpp"
#include "gpu_image/core/image_utils.hpp"
#include "gpu_image/core/kernel_helpers.hpp"
#include "gpu_image/operators/pixel_operator.hpp"
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

__global__ void toGrayscaleKernel(const unsigned char* input,
                                  unsigned char* output, int width, int height,
                                  int inputChannels) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int inIdx = (y * width + x) * inputChannels;
    int outIdx = y * width + x;

    // 使用标准灰度转换公式: 0.299*R + 0.587*G + 0.114*B
    float gray = 0.299f * input[inIdx] + 0.587f * input[inIdx + 1] +
                 0.114f * input[inIdx + 2];

    output[outIdx] =
        static_cast<unsigned char>(min(max(gray + 0.5f, 0.0f), 255.0f));
  }
}

__global__ void adjustBrightnessKernel(const unsigned char* input,
                                       unsigned char* output, int width,
                                       int height, int channels, int offset) {
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

__global__ void adjustBrightnessInPlaceKernel(unsigned char* data, int width,
                                              int height, int channels,
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

// PixelOperator implementation
void PixelOperator::invert(const GpuImage& input, GpuImage& output,
                           cudaStream_t stream) {
  validateInput(input);
  ImageUtils::ensureOutputSize(input, output);

  dim3 grid, block;
  calcGridBlock2D(input.width, input.height, grid, block);

  invertKernel<<<grid, block, 0, stream>>>(
      input.buffer.dataAs<unsigned char>(),
      output.buffer.dataAs<unsigned char>(), input.width, input.height,
      input.channels);

  CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::invertInPlace(GpuImage& image, cudaStream_t stream) {
  validateInput(image);

  dim3 grid, block;
  calcGridBlock2D(image.width, image.height, grid, block);

  invertInPlaceKernel<<<grid, block, 0, stream>>>(
      image.buffer.dataAs<unsigned char>(), image.width, image.height,
      image.channels);

  CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::toGrayscale(const GpuImage& input, GpuImage& output,
                                cudaStream_t stream) {
  validateInput(input);
  if (input.channels < 3) {
    throw std::invalid_argument(
        "Input must have at least 3 channels for grayscale conversion");
  }

  // 输出是单通道
  ImageUtils::ensureOutputSize(output, input.width, input.height, 1);

  dim3 grid, block;
  calcGridBlock2D(input.width, input.height, grid, block);

  toGrayscaleKernel<<<grid, block, 0, stream>>>(
      input.buffer.dataAs<unsigned char>(),
      output.buffer.dataAs<unsigned char>(), input.width, input.height,
      input.channels);

  CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::adjustBrightness(const GpuImage& input, GpuImage& output,
                                     int offset, cudaStream_t stream) {
  validateInput(input);
  ImageUtils::ensureOutputSize(input, output);

  dim3 grid, block;
  calcGridBlock2D(input.width, input.height, grid, block);

  adjustBrightnessKernel<<<grid, block, 0, stream>>>(
      input.buffer.dataAs<unsigned char>(),
      output.buffer.dataAs<unsigned char>(), input.width, input.height,
      input.channels, offset);

  CUDA_CHECK(cudaGetLastError());
}

void PixelOperator::adjustBrightnessInPlace(GpuImage& image, int offset,
                                            cudaStream_t stream) {
  validateInput(image);

  dim3 grid, block;
  calcGridBlock2D(image.width, image.height, grid, block);

  adjustBrightnessInPlaceKernel<<<grid, block, 0, stream>>>(
      image.buffer.dataAs<unsigned char>(), image.width, image.height,
      image.channels, offset);

  CUDA_CHECK(cudaGetLastError());
}

} // namespace gpu_image
