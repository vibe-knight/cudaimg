#pragma once

#include "gpu_image/core/execution_context.hpp"
#include "gpu_image/core/gpu_image.hpp"
#include "gpu_image/operators/convolution_engine.hpp"
#include "gpu_image/operators/histogram_calculator.hpp"
#include "gpu_image/operators/image_resizer.hpp"
#include "gpu_image/operators/pixel_operator.hpp"
#include <array>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace gpu_image {

/// ImageProcessor: High-level image processing facade with real depth
/// 图像处理器：具有真正深度的高级图像处理门面
///
/// This module provides a unified interface for image processing operations,
/// with integrated memory management and execution policy support.
///
/// Key features:
/// - Automatic memory pooling (configurable)
/// - Unified sync/async execution model
/// - Output buffer reuse for reduced allocation overhead
/// - Pipeline-style operation chaining
///
/// Depth: Callers interact with simple method calls; all complexity of
/// stream management, buffer allocation, and synchronization is hidden.
class ImageProcessor {
public:
  /// Processing mode configuration
  /// 处理模式配置
  enum class Mode {
    Sync,   ///< Synchronous: auto-sync after each operation
    Async,  ///< Asynchronous: caller controls sync
    Batch   ///< Batch: queue operations, sync all at once
  };

  /// Create a processor with default sync mode
  /// 创建默认同步模式的处理器
  ImageProcessor();

  /// Create a processor with specified mode
  /// 创建指定模式的处理器
  explicit ImageProcessor(Mode mode);

  /// Create a processor with custom execution policy
  /// 创建自定义执行策略的处理器
  explicit ImageProcessor(ExecutionPolicy policy);

  ~ImageProcessor();

  // ===== Configuration =====

  /// Enable or disable memory pooling
  /// 启用或禁用内存池
  void setMemoryPooling(bool enabled);

  /// Check if memory pooling is enabled
  bool isMemoryPoolingEnabled() const;

  /// Set execution mode
  /// 设置执行模式
  void setMode(Mode mode);

  /// Get current execution mode
  Mode mode() const { return mode_; }

  /// Get the underlying execution context
  ExecutionContext& context() { return context_; }
  const ExecutionContext& context() const { return context_; }

  // ===== Image Loading/Unloading =====

  /// Load image from raw memory to GPU
  /// 从原始内存加载图像到 GPU
  GpuImage loadFromMemory(const unsigned char* data, int width, int height,
                          int channels);

  /// Load image from HostImage to GPU
  /// 从 HostImage 加载图像到 GPU
  GpuImage loadFromHost(const HostImage& hostImage);

  /// Download GPU image to host
  /// 下载 GPU 图像到主机
  HostImage download(const GpuImage& image);

  /// Download GPU image to pre-allocated buffer
  /// 下载 GPU 图像到预分配缓冲区
  void downloadToBuffer(const GpuImage& image, unsigned char* buffer,
                        size_t bufferSize);

  // ===== Pixel Operations =====

  /// Invert image colors
  /// 反转图像颜色
  GpuImage invert(const GpuImage& input);

  /// Convert to grayscale
  /// 转换为灰度图
  GpuImage toGrayscale(const GpuImage& input);

  /// Adjust brightness
  /// 调整亮度
  GpuImage adjustBrightness(const GpuImage& input, int offset);

  /// In-place invert
  /// 原地反转
  void invertInPlace(GpuImage& image);

  /// In-place brightness adjustment
  /// 原地调整亮度
  void adjustBrightnessInPlace(GpuImage& image, int offset);

  // ===== Convolution Operations =====

  /// Gaussian blur
  /// 高斯模糊
  GpuImage gaussianBlur(const GpuImage& input, int kernelSize = 5,
                        float sigma = 1.0f);

  /// Sobel edge detection
  /// Sobel 边缘检测
  GpuImage sobelEdgeDetection(const GpuImage& input);

  /// General convolution
  /// 通用卷积
  GpuImage convolve(const GpuImage& input, const float* kernel, int kernelSize);

  // ===== Histogram Operations =====

  /// Calculate histogram (grayscale)
  /// 计算直方图（灰度）
  std::array<int, 256> histogram(const GpuImage& input);

  /// Calculate RGB histograms
  /// 计算 RGB 直方图
  std::array<std::array<int, 256>, 3> histogramRGB(const GpuImage& input);

  /// Histogram equalization
  /// 直方图均衡化
  GpuImage histogramEqualize(const GpuImage& input);

  // ===== Geometric Operations =====

  /// Resize to specific dimensions
  /// 调整到指定尺寸
  GpuImage resize(const GpuImage& input, int newWidth, int newHeight);

  /// Resize by scale factors
  /// 按比例因子调整
  GpuImage resizeByScale(const GpuImage& input, float scaleX, float scaleY);

  // ===== Pipeline Operations =====

  /// Apply a pipeline of operations
  /// 应用操作管道
  /// @param input Input image
  /// @param operations Vector of operations to apply in sequence
  /// @return Final result image
  ///
  /// Example:
  /// @code
  /// auto result = processor.pipeline(input, {
  ///   [&](const GpuImage& in) { return processor.toGrayscale(in); },
  ///   [&](const GpuImage& in) { return processor.gaussianBlur(in); },
  ///   [&](const GpuImage& in) { return processor.sobelEdgeDetection(in); }
  /// });
  /// @endcode
  GpuImage pipeline(const GpuImage& input,
                    std::vector<std::function<GpuImage(const GpuImage&)>> operations);

  // ===== Synchronization =====

  /// Synchronize (for async/batch modes)
  /// 同步（用于异步/批处理模式）
  void synchronize();

  /// Check if operations are complete (non-blocking)
  /// 检查操作是否完成（非阻塞）
  bool isComplete() const;

private:
  Mode mode_;
  ExecutionContext context_;
  GpuImage tempBuffer_;  ///< Reusable buffer for intermediate results

  /// Get or create output buffer matching input
  GpuImage& prepareOutput(const GpuImage& input, GpuImage& output);

  /// Auto-sync if in sync mode
  void autoSync();
};

// ===== Builder Pattern for Pipeline Construction =====

/// PipelineBuilder: Fluent interface for building processing pipelines
/// 管道构建器：用于构建处理管道的流式接口
///
/// Example:
/// @code
/// auto result = PipelineBuilder(processor)
///     .start(input)
///     .grayscale()
///     .blur(5, 1.5f)
///     .sobel()
///     .execute();
/// @endcode
///
/// Note: If start() was not called before execute(), returns invalid GpuImage.
/// 注意：如果在 execute() 前未调用 start()，将返回无效的 GpuImage。
class PipelineBuilder {
public:
  explicit PipelineBuilder(ImageProcessor& processor);

  /// Start pipeline with input image
  /// 开始管道，指定输入图像
  PipelineBuilder& start(const GpuImage& input);

  /// Add grayscale conversion
  /// 添加灰度转换
  PipelineBuilder& grayscale();

  /// Add Gaussian blur
  /// 添加高斯模糊
  PipelineBuilder& blur(int kernelSize = 5, float sigma = 1.0f);

  /// Add Sobel edge detection
  /// 添加 Sobel 边缘检测
  PipelineBuilder& sobel();

  /// Add invert operation
  /// 添加反转操作
  PipelineBuilder& invert();

  /// Add brightness adjustment
  /// 添加亮度调整
  PipelineBuilder& brightness(int offset);

  /// Add resize operation
  /// 添加调整大小操作
  PipelineBuilder& resize(int width, int height);

  /// Add histogram equalization
  /// 添加直方图均衡化
  PipelineBuilder& equalize();

  /// Execute pipeline and return result
  /// 执行管道并返回结果
  /// @return Result image (invalid if start() was not called)
  GpuImage execute();

  /// Execute pipeline and download to host
  /// 执行管道并下载到主机
  /// @return Result as HostImage (empty if start() was not called)
  HostImage executeAndDownload();

  /// Check if pipeline has an input set
  /// 检查管道是否已设置输入
  bool hasInput() const { return hasInput_; }

private:
  ImageProcessor& processor_;
  GpuImage current_;
  bool hasInput_ = false;
};

} // namespace gpu_image
