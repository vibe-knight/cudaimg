#include "gpu_image/image_processor.hpp"
#include "gpu_image/cuda_error.hpp"
#include <cstring>
#include <cuda_runtime.h>
#include <stdexcept>

namespace gpu_image {

ImageProcessor::ImageProcessor() {
  // 初始化 CUDA
  int deviceCount;
  cudaError_t err = cudaGetDeviceCount(&deviceCount);
  if (err != cudaSuccess || deviceCount == 0) {
    throw std::runtime_error("No CUDA devices available");
  }
}

ImageProcessor::~ImageProcessor() {
  // 清理资源
  cudaDeviceSynchronize();
}

GpuImage ImageProcessor::loadFromMemory(const unsigned char* data, int width,
                                        int height, int channels) {
  if (data == nullptr) {
    throw std::invalid_argument("Data pointer is null");
  }
  if (!ImageUtils::validateImageParams(width, height, channels)) {
    throw std::invalid_argument("Invalid image parameters");
  }

  GpuImage image = ImageUtils::createGpuImage(width, height, channels);
  image.buffer.copyFromHost(data, image.totalBytes());

  return image;
}

GpuImage ImageProcessor::loadFromHost(const HostImage& hostImage) {
  return ImageUtils::uploadToGpu(hostImage);
}

HostImage ImageProcessor::downloadImage(const GpuImage& image) {
  return ImageUtils::downloadFromGpu(image);
}

void ImageProcessor::downloadToBuffer(const GpuImage& image,
                                      unsigned char* buffer,
                                      size_t bufferSize) {
  if (buffer == nullptr) {
    throw std::invalid_argument("Buffer is null");
  }
  if (bufferSize < image.totalBytes()) {
    throw std::invalid_argument("Buffer too small");
  }

  image.buffer.copyToHost(buffer, image.totalBytes());
}

GpuImage ImageProcessor::invert(const GpuImage& input) {
  GpuImage output;
  PixelOperator::invert(input, output);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::toGrayscale(const GpuImage& input) {
  GpuImage output;
  PixelOperator::toGrayscale(input, output);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::adjustBrightness(const GpuImage& input, int offset) {
  GpuImage output;
  PixelOperator::adjustBrightness(input, output, offset);
  cudaDeviceSynchronize();
  return output;
}

void ImageProcessor::invertInPlace(GpuImage& image) {
  PixelOperator::invertInPlace(image);
  cudaDeviceSynchronize();
}

void ImageProcessor::adjustBrightnessInPlace(GpuImage& image, int offset) {
  PixelOperator::adjustBrightnessInPlace(image, offset);
  cudaDeviceSynchronize();
}

GpuImage ImageProcessor::gaussianBlur(const GpuImage& input, int kernelSize,
                                      float sigma) {
  GpuImage output;
  ConvolutionEngine::gaussianBlur(input, output, kernelSize, sigma);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::sobelEdgeDetection(const GpuImage& input) {
  GpuImage output;
  ConvolutionEngine::sobelEdgeDetection(input, output);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::convolve(const GpuImage& input, const float* kernel,
                                  int kernelSize) {
  GpuImage output;
  ConvolutionEngine::convolve(input, output, kernel, kernelSize);
  cudaDeviceSynchronize();
  return output;
}

std::array<int, 256> ImageProcessor::histogram(const GpuImage& input) {
  return HistogramCalculator::calculate(input);
}

std::array<std::array<int, 256>, 3>
ImageProcessor::histogramRGB(const GpuImage& input) {
  return HistogramCalculator::calculateRGB(input);
}

GpuImage ImageProcessor::histogramEqualize(const GpuImage& input) {
  GpuImage output;
  HistogramCalculator::equalize(input, output);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::resize(const GpuImage& input, int newWidth,
                                int newHeight) {
  GpuImage output;
  ImageResizer::resize(input, output, newWidth, newHeight);
  cudaDeviceSynchronize();
  return output;
}

GpuImage ImageProcessor::resizeByScale(const GpuImage& input, float scaleX,
                                       float scaleY) {
  GpuImage output;
  ImageResizer::resizeByScale(input, output, scaleX, scaleY);
  cudaDeviceSynchronize();
  return output;
}

void ImageProcessor::synchronize() { CUDA_CHECK(cudaDeviceSynchronize()); }

} // namespace gpu_image
