---
layout: default
title: ImageProcessor
description: ImageProcessor API 参考 - 同步高级图像处理接口。
---

# ImageProcessor

高级同步 API，用于图像处理操作。为常见任务提供简单接口。

## 头文件

```cpp
#include "gpu_image/image_processor.hpp"
```

## 类概览

```cpp
namespace gpu_image {

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor() = default;

    // 主机 <-> 设备传输
    GpuImage loadFromHost(const HostImage& hostImage);
    HostImage downloadImage(const GpuImage& gpuImage);

    // 像素操作
    GpuImage invert(const GpuImage& image);
    GpuImage toGrayscale(const GpuImage& image);
    GpuImage adjustBrightness(const GpuImage& image, int delta);

    // 卷积
    GpuImage gaussianBlur(const GpuImage& image, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& image);

    // 直方图
    std::vector<int> histogram(const GpuImage& image);
    GpuImage histogramEqualize(const GpuImage& image);

    // 几何变换
    GpuImage resize(const GpuImage& image, int dstWidth, int dstHeight);
    
    // 滤波器便捷方法
    GpuImage medianFilter(const GpuImage& image, int kernelSize);
    GpuImage bilateralFilter(const GpuImage& image, int d, float sigmaColor, float sigmaSpace);
};

}
```

## 构造函数

### ImageProcessor()

默认构造函数。初始化内部资源。

```cpp
ImageProcessor processor;  // 简单！
```

## 传输操作

### loadFromHost()

将主机图像上传到 GPU 内存。

```cpp
GpuImage loadFromHost(const HostImage& hostImage);
```

**参数：**
- `hostImage` - 主机内存中的源图像

**返回：** GPU 图像副本

**异常：**
- `CudaException` - 内存分配失败时
- `std::invalid_argument` - 图像无效时

**示例：**
```cpp
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
```

### downloadImage()

将 GPU 图像下载到主机内存。

```cpp
HostImage downloadImage(const GpuImage& gpuImage);
```

**参数：**
- `gpuImage` - GPU 内存中的源图像

**返回：** 主机图像副本

**异常：**
- `CudaException` - 下载失败时

**示例：**
```cpp
HostImage result = processor.downloadImage(processedGpu);
ImageIO::save("output.jpg", result);
```

## 像素操作

### invert()

反转所有像素值（255 - value）。

```cpp
GpuImage invert(const GpuImage& image);
```

**参数：**
- `image` - 源图像

**返回：** 反转后的图像

**示例：**
```cpp
GpuImage inverted = processor.invert(original);
```

### toGrayscale()

使用亮度公式将 RGB/RGBA 转换为灰度。

```cpp
GpuImage toGrayscale(const GpuImage& image);
```

**参数：**
- `image` - 源图像（RGB 或 RGBA）

**返回：** 灰度图像（1 通道）

**公式：** `gray = 0.299*R + 0.587*G + 0.114*B`

**示例：**
```cpp
GpuImage gray = processor.toGrayscale(colorImage);
```

### adjustBrightness()

通过对每个像素添加 delta 来调整亮度。

```cpp
GpuImage adjustBrightness(const GpuImage& image, int delta);
```

**参数：**
- `image` - 源图像
- `delta` - 亮度调整值（-255 到 255）

**返回：** 亮度调整后的图像

**示例：**
```cpp
GpuImage brighter = processor.adjustBrightness(image, 50);
GpuImage darker = processor.adjustBrightness(image, -50);
```

## 卷积

### gaussianBlur()

应用高斯模糊滤波。

```cpp
GpuImage gaussianBlur(const GpuImage& image, int kernelSize, float sigma);
```

**参数：**
- `image` - 源图像
- `kernelSize` - 卷积核大小（必须为奇数，>= 3）
- `sigma` - 标准差（必须 > 0）

**返回：** 模糊后的图像

**异常：**
- `std::invalid_argument` - kernelSize 为偶数或 < 3 时
- `std::invalid_argument` - sigma <= 0 时

**示例：**
```cpp
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);
```

### sobelEdgeDetection()

使用 Sobel 算子检测边缘。

```cpp
GpuImage sobelEdgeDetection(const GpuImage& image);
```

**参数：**
- `image` - 源图像

**返回：** 边缘幅值图像（灰度）

**示例：**
```cpp
GpuImage edges = processor.sobelEdgeDetection(image);
```

## 直方图

### histogram()

计算灰度直方图。

```cpp
std::vector<int> histogram(const GpuImage& image);
```

**参数：**
- `image` - 源图像（自动转换为灰度）

**返回：** 256 个 bin 的直方图（值 0-255）

**示例：**
```cpp
std::vector<int> hist = processor.histogram(image);
// hist[0] = 黑色像素计数
// hist[255] = 白色像素计数
```

### histogramEqualize()

应用直方图均衡化以增强对比度。

```cpp
GpuImage histogramEqualize(const GpuImage& image);
```

**参数：**
- `image` - 源图像

**返回：** 均衡化后的图像

**示例：**
```cpp
GpuImage enhanced = processor.histogramEqualize(lowContrastImage);
```

## 几何变换

### resize()

使用双线性插值调整图像大小。

```cpp
GpuImage resize(const GpuImage& image, int dstWidth, int dstHeight);
```

**参数：**
- `image` - 源图像
- `dstWidth` - 目标宽度
- `dstHeight` - 目标高度

**返回：** 调整大小后的图像

**示例：**
```cpp
GpuImage thumbnail = processor.resize(image, 256, 256);
GpuImage upscaled = processor.resize(image, 1920, 1080);
```

## 滤波器

### medianFilter()

应用中值滤波以降低噪声。

```cpp
GpuImage medianFilter(const GpuImage& image, int kernelSize);
```

**参数：**
- `image` - 源图像
- `kernelSize` - 卷积核大小（3 或 5）

**返回：** 滤波后的图像

**示例：**
```cpp
GpuImage denoised = processor.medianFilter(noisyImage, 3);
```

### bilateralFilter()

应用双边滤波（保边平滑）。

```cpp
GpuImage bilateralFilter(const GpuImage& image, int d, float sigmaColor, float sigmaSpace);
```

**参数：**
- `image` - 源图像
- `d` - 像素邻域直径
- `sigmaColor` - 色彩空间滤波 sigma
- `sigmaSpace` - 坐标空间滤波 sigma

**返回：** 滤波后的图像

**示例：**
```cpp
GpuImage smooth = processor.bilateralFilter(image, 5, 50.0f, 50.0f);
```

## 完整示例

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    try {
        ImageProcessor processor;
        
        // 加载图像
        HostImage host = ImageIO::load("photo.jpg");
        if (!host.isValid()) {
            std::cerr << "加载图像失败" << std::endl;
            return 1;
        }
        
        // 上传到 GPU
        GpuImage gpu = processor.loadFromHost(host);
        
        // 处理
        GpuImage grayscale = processor.toGrayscale(gpu);
        GpuImage edges = processor.sobelEdgeDetection(grayscale);
        GpuImage blurred = processor.gaussianBlur(edges, 3, 1.0f);
        
        // 下载并保存
        HostImage result = processor.downloadImage(blurred);
        ImageIO::save("output.jpg", result);
        
        std::cout << "处理完成！" << std::endl;
        
    } catch (const CudaException& e) {
        std::cerr << "CUDA 错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## 线程安全

`ImageProcessor` **非线程安全**。每线程创建一个实例：

```cpp
// 正确：每个线程有自己的处理器
void worker(int id, const HostImage& input) {
    ImageProcessor processor;
    // 处理...
}
```

## 性能说明

- 每个方法在返回前与设备同步
- 对于批处理，使用 `PipelineProcessor`
- 内存传输是主要瓶颈

## 另请参阅

- [PipelineProcessor](pipeline_processor) - 用于批处理/异步处理
- [PixelOperator](pixel_operator) - 底层像素操作
- [ConvolutionEngine](convolution_engine) - 底层卷积
