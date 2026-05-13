#pragma once

#include "gpu_image/core/execution_context.hpp"
#include "gpu_image/core/gpu_image.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace gpu_image {

// Forward declarations
class ImageOperator;
class OperatorPipeline;

/// OperatorTraits: Metadata for operator types
/// 操作符特性：操作符类型的元数据
struct OperatorTraits {
  std::string name;
  bool inPlaceCapable;
  bool changesDimensions;
  bool changesChannels;
};

/// ImageOperator: Base interface for all image operators
/// 图像操作符：所有图像操作符的基础接口
///
/// This interface enables:
/// - Polymorphic operator handling
/// - Pipeline composition
/// - Runtime operator discovery
/// - Mocking for tests
///
/// Depth: Callers interact with a unified interface; specific operator
/// implementations are hidden behind this abstraction.
class ImageOperator {
public:
  virtual ~ImageOperator() = default;

  /// Apply the operation to an input image
  /// 对输入图像应用操作
  /// @param input Input image
  /// @param ctx Execution context (stream, memory policy)
  /// @return Output image
  [[nodiscard]] virtual GpuImage apply(const GpuImage& input,
                                        ExecutionContext& ctx) = 0;

  /// Apply operation in-place (if supported)
  /// 原地应用操作（如果支持）
  virtual void applyInPlace(GpuImage& image, ExecutionContext& ctx) {
    throw std::runtime_error("In-place operation not supported");
  }

  /// Get operator traits/metadata
  /// 获取操作符特性/元数据
  [[nodiscard]] virtual OperatorTraits traits() const = 0;

  /// Clone the operator
  /// 克隆操作符
  [[nodiscard]] virtual std::unique_ptr<ImageOperator> clone() const = 0;

  /// Check if in-place operation is supported
  /// 检查是否支持原地操作
  [[nodiscard]] bool canApplyInPlace() const { return traits().inPlaceCapable; }
};

/// UnaryOperator: Base for single-input operators
/// 一元操作符：单输入操作符的基类
template <typename Derived>
class UnaryOperator : public ImageOperator {
public:
  GpuImage apply(const GpuImage& input, ExecutionContext& ctx) override {
    GpuImage output;
    static_cast<Derived*>(this)->execute(input, output, ctx);
    return output;
  }

  std::unique_ptr<ImageOperator> clone() const override {
    return std::make_unique<Derived>(*static_cast<const Derived*>(this));
  }
};

// ===== Concrete Operator Classes =====

/// InvertOperator: Color inversion
/// 反转操作符：颜色反转
class InvertOperator : public UnaryOperator<InvertOperator> {
public:
  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    PixelOperator::invert(input, output, ctx.stream());
  }

  void applyInPlace(GpuImage& image, ExecutionContext& ctx) override {
    PixelOperator::invertInPlace(image, ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"invert", true, false, false};
  }
};

/// GrayscaleOperator: RGB to grayscale conversion
/// 灰度操作符：RGB 转灰度
class GrayscaleOperator : public UnaryOperator<GrayscaleOperator> {
public:
  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    PixelOperator::toGrayscale(input, output, ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"grayscale", false, false, true}; // Changes channels 3->1
  }
};

/// BrightnessOperator: Brightness adjustment
/// 亮度操作符：亮度调整
class BrightnessOperator : public UnaryOperator<BrightnessOperator> {
public:
  explicit BrightnessOperator(int offset = 0) : offset_(offset) {}

  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    PixelOperator::adjustBrightness(input, output, offset_, ctx.stream());
  }

  void applyInPlace(GpuImage& image, ExecutionContext& ctx) override {
    PixelOperator::adjustBrightnessInPlace(image, offset_, ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"brightness", true, false, false};
  }

  void setOffset(int offset) { offset_ = offset; }
  int offset() const { return offset_; }

private:
  int offset_;
};

/// GaussianBlurOperator: Gaussian blur
/// 高斯模糊操作符
class GaussianBlurOperator : public UnaryOperator<GaussianBlurOperator> {
public:
  explicit GaussianBlurOperator(int kernelSize = 5, float sigma = 1.0f)
      : kernelSize_(kernelSize), sigma_(sigma) {}

  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    ConvolutionEngine::gaussianBlur(input, output, kernelSize_, sigma_,
                                    ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"gaussian_blur", false, false, false};
  }

  void setKernelSize(int size) { kernelSize_ = size; }
  void setSigma(float sigma) { sigma_ = sigma; }
  int kernelSize() const { return kernelSize_; }
  float sigma() const { return sigma_; }

private:
  int kernelSize_;
  float sigma_;
};

/// SobelOperator: Sobel edge detection
/// Sobel 操作符：边缘检测
class SobelOperator : public UnaryOperator<SobelOperator> {
public:
  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    ConvolutionEngine::sobelEdgeDetection(input, output, ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"sobel", false, false, true}; // Output is single channel
  }
};

/// ResizeOperator: Image resizing
/// 调整大小操作符
class ResizeOperator : public UnaryOperator<ResizeOperator> {
public:
  /// Default constructor - use setDimensions() or setScale() before execute
  ResizeOperator() : width_(0), height_(0), scaleX_(0), scaleY_(0),
                     useScale_(false) {}

  /// Create by specifying absolute dimensions
  /// 通过指定绝对尺寸创建
  /// @param width Target width in pixels
  /// @param height Target height in pixels
  explicit ResizeOperator(int width, int height)
      : width_(width), height_(height), scaleX_(0), scaleY_(0),
        useScale_(false) {}

  /// Named constructor: Create by specifying scale factors
  /// 命名构造函数：通过指定比例因子创建
  /// @param scaleX Horizontal scale factor (e.g., 0.5 for half size)
  /// @param scaleY Vertical scale factor (e.g., 0.5 for half size)
  /// @return ResizeOperator configured for scale-based resizing
  static ResizeOperator byScale(float scaleX, float scaleY) {
    return ResizeOperator(scaleX, scaleY, true);
  }

  /// Named constructor: Create by specifying dimensions
  /// 命名构造函数：通过指定尺寸创建
  /// @param width Target width in pixels
  /// @param height Target height in pixels
  /// @return ResizeOperator configured for absolute resizing
  static ResizeOperator byDimensions(int width, int height) {
    return ResizeOperator(width, height);
  }

  void execute(const GpuImage& input, GpuImage& output, ExecutionContext& ctx) {
    if (useScale_) {
      ImageResizer::resizeByScale(input, output, scaleX_, scaleY_,
                                  ctx.stream());
    } else {
      ImageResizer::resize(input, output, width_, height_, ctx.stream());
    }
  }

  OperatorTraits traits() const override {
    return {"resize", false, true, false}; // Changes dimensions
  }

  /// Set target dimensions (switches to dimension mode)
  /// 设置目标尺寸（切换到尺寸模式）
  void setDimensions(int width, int height) {
    width_ = width;
    height_ = height;
    useScale_ = false;
  }

  /// Set scale factors (switches to scale mode)
  /// 设置比例因子（切换到比例模式）
  void setScale(float scaleX, float scaleY) {
    scaleX_ = scaleX;
    scaleY_ = scaleY;
    useScale_ = true;
  }

  /// Check if using scale mode
  /// 检查是否使用比例模式
  bool isScaleMode() const { return useScale_; }

  /// Get target width (only valid in dimension mode)
  int width() const { return width_; }

  /// Get target height (only valid in dimension mode)
  int height() const { return height_; }

  /// Get horizontal scale factor (only valid in scale mode)
  float scaleX() const { return scaleX_; }

  /// Get vertical scale factor (only valid in scale mode)
  float scaleY() const { return scaleY_; }

private:
  // Private constructor for scale mode
  ResizeOperator(float scaleX, float scaleY, bool /*useScale*/)
      : width_(0), height_(0), scaleX_(scaleX), scaleY_(scaleY),
        useScale_(true) {}

  int width_, height_;
  float scaleX_, scaleY_;
  bool useScale_;
};

/// HistogramEqualizeOperator: Histogram equalization
/// 直方图均衡化操作符
class HistogramEqualizeOperator
    : public UnaryOperator<HistogramEqualizeOperator> {
public:
  void execute(const GpuImage& input, GpuImage& output,
               ExecutionContext& ctx) {
    HistogramCalculator::equalize(input, output, ctx.stream());
  }

  OperatorTraits traits() const override {
    return {"histogram_equalize", false, false, false};
  }
};

// ===== Operator Pipeline =====

/// OperatorPipeline: Chain of operators
/// 操作符管道：操作符链
///
/// Enables fluent composition of multiple operators.
/// Intermediate buffers are automatically managed.
///
/// Example:
/// @code
/// auto pipeline = OperatorPipeline()
///     .then<GrayscaleOperator>()
///     .then<GaussianBlurOperator>(5, 1.5f)
///     .then<SobelOperator>();
///
/// ExecutionContext ctx(ExecutionPolicy::async());
/// GpuImage result = pipeline.process(input, ctx);
/// @endcode
class OperatorPipeline : public ImageOperator {
public:
  OperatorPipeline() = default;

  /// Add an operator to the pipeline
  /// 向管道添加操作符
  template <typename Op, typename... Args>
  OperatorPipeline& then(Args&&... args) {
    operators_.push_back(std::make_unique<Op>(std::forward<Args>(args)...));
    return *this;
  }

  /// Add an existing operator (takes ownership)
  /// 添加现有操作符（获取所有权）
  OperatorPipeline& then(std::unique_ptr<ImageOperator> op) {
    operators_.push_back(std::move(op));
    return *this;
  }

  /// Process image through the pipeline
  /// 通过管道处理图像
  ///
  /// @param input Input image (if invalid, returns invalid image)
  /// @param ctx Execution context
  /// @return Output image (invalid if input was invalid or pipeline empty)
  GpuImage apply(const GpuImage& input, ExecutionContext& ctx) override {
    // Handle empty pipeline - return input as-is
    if (operators_.empty()) {
      return input;
    }

    // Handle invalid input - return invalid output
    if (!input.isValid()) {
      return GpuImage{};  // Return default-constructed (invalid) image
    }

    GpuImage current = input;

    for (auto& op : operators_) {
      GpuImage next = op->apply(current, ctx);

      // Recycle intermediate buffers to pool
      // Note: Don't recycle the original input
      if (current.isValid() && &current != &input) {
        ctx.recycleToPool(std::move(current));
      }

      current = std::move(next);
    }

    return current;
  }

  OperatorTraits traits() const override {
    OperatorTraits result{"pipeline", false, false, false};
    for (const auto& op : operators_) {
      auto t = op->traits();
      result.name += (result.name.empty() ? "" : " -> ") + t.name;
      result.changesDimensions = result.changesDimensions || t.changesDimensions;
      result.changesChannels = result.changesChannels || t.changesChannels;
    }
    return result;
  }

  std::unique_ptr<ImageOperator> clone() const override {
    auto pipeline = std::make_unique<OperatorPipeline>();
    for (const auto& op : operators_) {
      pipeline->operators_.push_back(op->clone());
    }
    return pipeline;
  }

  /// Get number of operators in pipeline
  [[nodiscard]] size_t size() const { return operators_.size(); }

  /// Check if pipeline is empty
  [[nodiscard]] bool empty() const { return operators_.empty(); }

  /// Clear all operators
  void clear() { operators_.clear(); }

private:
  std::vector<std::unique_ptr<ImageOperator>> operators_;
};

// ===== Operator Registry (for dynamic creation) =====

/// OperatorRegistry: Central registry for operator types
/// 操作符注册表：操作符类型的中央注册表
///
/// Enables runtime operator discovery and creation by name.
class OperatorRegistry {
public:
  using Creator = std::unique_ptr<ImageOperator> (*)();

  static OperatorRegistry& instance() {
    static OperatorRegistry registry;
    return registry;
  }

  /// Register an operator type
  /// 注册操作符类型
  template <typename Op>
  void registerOperator(const std::string& name) {
    creators_[name] = []() { return std::make_unique<Op>(); };
  }

  /// Create an operator by name
  /// 按名称创建操作符
  [[nodiscard]] std::unique_ptr<ImageOperator> create(const std::string& name) const {
    auto it = creators_.find(name);
    if (it == creators_.end()) {
      return nullptr;
    }
    return it->second();
  }

  /// List all registered operator names
  /// 列出所有已注册的操作符名称
  [[nodiscard]] std::vector<std::string> listNames() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : creators_) {
      names.push_back(name);
    }
    return names;
  }

private:
  OperatorRegistry() {
    // Register built-in operators
    registerOperator<InvertOperator>("invert");
    registerOperator<GrayscaleOperator>("grayscale");
    registerOperator<BrightnessOperator>("brightness");
    registerOperator<GaussianBlurOperator>("gaussian_blur");
    registerOperator<SobelOperator>("sobel");
    registerOperator<ResizeOperator>("resize");
    registerOperator<HistogramEqualizeOperator>("histogram_equalize");
  }

  std::unordered_map<std::string, Creator> creators_;
};

// ===== Free Functions for Operator Creation =====

/// Create an invert operator
[[nodiscard]] inline std::unique_ptr<InvertOperator> makeInvert() {
  return std::make_unique<InvertOperator>();
}

/// Create a grayscale operator
[[nodiscard]] inline std::unique_ptr<GrayscaleOperator> makeGrayscale() {
  return std::make_unique<GrayscaleOperator>();
}

/// Create a brightness operator
[[nodiscard]] inline std::unique_ptr<BrightnessOperator> makeBrightness(int offset = 0) {
  return std::make_unique<BrightnessOperator>(offset);
}

/// Create a Gaussian blur operator
[[nodiscard]] inline std::unique_ptr<GaussianBlurOperator>
makeGaussianBlur(int kernelSize = 5, float sigma = 1.0f) {
  return std::make_unique<GaussianBlurOperator>(kernelSize, sigma);
}

/// Create a Sobel operator
[[nodiscard]] inline std::unique_ptr<SobelOperator> makeSobel() {
  return std::make_unique<SobelOperator>();
}

/// Create a resize operator with absolute dimensions
/// 创建绝对尺寸调整操作符
[[nodiscard]] inline std::unique_ptr<ResizeOperator> makeResize(int width, int height) {
  return std::make_unique<ResizeOperator>(
      ResizeOperator::byDimensions(width, height));
}

/// Create a resize operator with scale factors
/// 创建比例调整操作符
[[nodiscard]] inline std::unique_ptr<ResizeOperator> makeResizeByScale(float scaleX,
                                                                        float scaleY) {
  return std::make_unique<ResizeOperator>(
      ResizeOperator::byScale(scaleX, scaleY));
}

/// Create a histogram equalize operator
[[nodiscard]] inline std::unique_ptr<HistogramEqualizeOperator> makeHistogramEqualize() {
  return std::make_unique<HistogramEqualizeOperator>();
}

} // namespace gpu_image
