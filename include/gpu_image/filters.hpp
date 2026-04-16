#pragma once

#include "gpu_image/gpu_image.hpp"
#include <cuda_runtime.h>

namespace gpu_image {

// 滤波器类
class Filters {
public:
  // 中值滤波
  static void medianFilter(const GpuImage& input, GpuImage& output,
                           int kernelSize = 3, cudaStream_t stream = nullptr);

  // 双边滤波
  static void bilateralFilter(const GpuImage& input, GpuImage& output,
                              int kernelSize = 5, float sigmaSpace = 10.0f,
                              float sigmaColor = 50.0f,
                              cudaStream_t stream = nullptr);

  // 均值滤波（盒式滤波）
  static void boxFilter(const GpuImage& input, GpuImage& output,
                        int kernelSize = 3, cudaStream_t stream = nullptr);

  // 锐化滤波
  static void sharpen(const GpuImage& input, GpuImage& output,
                      float strength = 1.0f, cudaStream_t stream = nullptr);

  // 拉普拉斯滤波（边缘增强）
  static void laplacian(const GpuImage& input, GpuImage& output,
                        cudaStream_t stream = nullptr);
};

// 图像算术操作类
class ImageArithmetic {
public:
  // 图像加法
  static void add(const GpuImage& src1, const GpuImage& src2, GpuImage& output,
                  cudaStream_t stream = nullptr);

  // 图像减法
  static void subtract(const GpuImage& src1, const GpuImage& src2,
                       GpuImage& output, cudaStream_t stream = nullptr);

  // 图像乘法（逐像素）
  static void multiply(const GpuImage& src1, const GpuImage& src2,
                       GpuImage& output, float scale = 1.0f,
                       cudaStream_t stream = nullptr);

  // 图像混合 (alpha blending)
  static void blend(const GpuImage& src1, const GpuImage& src2,
                    GpuImage& output,
                    float alpha, // src1 权重
                    cudaStream_t stream = nullptr);

  // 加权和
  static void addWeighted(const GpuImage& src1, float alpha,
                          const GpuImage& src2, float beta, GpuImage& output,
                          float gamma = 0.0f, cudaStream_t stream = nullptr);

  // 绝对差
  static void absDiff(const GpuImage& src1, const GpuImage& src2,
                      GpuImage& output, cudaStream_t stream = nullptr);

  // 标量加法
  static void addScalar(const GpuImage& input, GpuImage& output,
                        unsigned char value, cudaStream_t stream = nullptr);

  // 标量乘法
  static void multiplyScalar(const GpuImage& input, GpuImage& output,
                             float scale, cudaStream_t stream = nullptr);
};

} // namespace gpu_image
