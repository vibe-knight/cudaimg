#pragma once

#include "gpu_image/convolution_engine.hpp"
#include "gpu_image/gpu_image.hpp"
#include "gpu_image/histogram_calculator.hpp"
#include "gpu_image/image_resizer.hpp"
#include "gpu_image/pixel_operator.hpp"
#include <array>
#include <string>

namespace gpu_image {

// 主要的用户接口类
class ImageProcessor {
public:
  ImageProcessor();
  ~ImageProcessor();

  // 从内存加载图像到 GPU
  GpuImage loadFromMemory(const unsigned char* data, int width, int height,
                          int channels);

  // 从 HostImage 加载
  GpuImage loadFromHost(const HostImage& hostImage);

  // 将 GPU 图像复制到 Host 内存
  HostImage downloadImage(const GpuImage& image);

  // 将 GPU 图像复制到指定缓冲区
  void downloadToBuffer(const GpuImage& image, unsigned char* buffer,
                        size_t bufferSize);

  // 像素操作
  GpuImage invert(const GpuImage& input);
  GpuImage toGrayscale(const GpuImage& input);
  GpuImage adjustBrightness(const GpuImage& input, int offset);

  // 原地像素操作
  void invertInPlace(GpuImage& image);
  void adjustBrightnessInPlace(GpuImage& image, int offset);

  // 卷积操作
  GpuImage gaussianBlur(const GpuImage& input, int kernelSize = 5,
                        float sigma = 1.0f);
  GpuImage sobelEdgeDetection(const GpuImage& input);
  GpuImage convolve(const GpuImage& input, const float* kernel, int kernelSize);

  // 直方图
  std::array<int, 256> histogram(const GpuImage& input);
  std::array<std::array<int, 256>, 3> histogramRGB(const GpuImage& input);
  GpuImage histogramEqualize(const GpuImage& input);

  // 缩放
  GpuImage resize(const GpuImage& input, int newWidth, int newHeight);
  GpuImage resizeByScale(const GpuImage& input, float scaleX, float scaleY);

  // 同步
  void synchronize();
};

} // namespace gpu_image
