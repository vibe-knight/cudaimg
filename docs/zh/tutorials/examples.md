# 示例代码

常见用例的代码示例。

## 基本操作

### 高斯模糊

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    ImageProcessor processor;
    
    HostImage input = ImageIO::load("input.jpg");
    GpuImage gpu = processor.loadFromHost(input);
    
    // 应用 5x5 高斯模糊，sigma=1.5
    GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
    
    HostImage output = processor.downloadImage(blurred);
    ImageIO::save("blurred.jpg", output);
}
```

### 边缘检测

```cpp
// Sobel 边缘检测
GpuImage edges = processor.sobelEdgeDetection(gpu);

// 先转灰度效果更好
GpuImage gray = processor.grayscale(gpu);
GpuImage edges = processor.sobelEdgeDetection(gray);
```

### 图像缩放

```cpp
// 缩放到指定尺寸
GpuImage resized = processor.resize(gpu, 1920, 1080);

// 按比例缩放
int newWidth = gpu.width() * 2;
int newHeight = gpu.height() * 2;
GpuImage doubled = processor.resize(gpu, newWidth, newHeight);
```

## 图像处理流水线

```cpp
// 多步骤处理
GpuImage gray = processor.grayscale(gpu);
GpuImage blurred = processor.gaussianBlur(gray, 5, 1.0f);
GpuImage edges = processor.sobelEdgeDetection(blurred);
GpuImage thresholded = processor.threshold(edges, 128);
```

## 批量处理

```cpp
// 处理多张图像
std::vector<std::string> files = {"img1.jpg", "img2.jpg", "img3.jpg"};

for (const auto& file : files) {
    HostImage input = ImageIO::load(file);
    GpuImage gpu = processor.loadFromHost(input);
    GpuImage processed = processor.gaussianBlur(gpu, 5, 1.5f);
    HostImage output = processor.downloadImage(processed);
    
    std::string outFile = "processed_" + file;
    ImageIO::save(outFile, output);
}
```

## 异步处理

```cpp
// 使用 PipelineProcessor 进行异步操作
PipelineProcessor pipeline(4);  // 4 个 CUDA 流

std::vector<GpuImage> images;
for (auto& img : images) {
    auto stream = pipeline.getStream();
    pipeline.gaussianBlur(img, 5, 1.5f, stream);
}

pipeline.synchronize();
```

## 形态学操作

```cpp
// 腐蚀和膨胀
GpuImage eroded = processor.erode(gpu, 5);  // 5x5 内核
GpuImage dilated = processor.dilate(gpu, 5);

// 开运算（先腐蚀后膨胀）
GpuImage opened = processor.morphologyOpen(gpu, 5);

// 闭运算（先膨胀后腐蚀）
GpuImage closed = processor.morphologyClose(gpu, 5);
```

## 色彩空间转换

```cpp
// RGB 转 HSV
GpuImage hsv = processor.rgbToHsv(gpu);

// RGB 转 YUV
GpuImage yuv = processor.rgbToYuv(gpu);
```

## 直方图操作

```cpp
// 计算直方图
std::vector<int> hist = processor.histogram(gpu);

// 直方图均衡化
GpuImage equalized = processor.histogramEqualization(gpu);
```

## 更多示例

参见仓库中的 `examples/` 目录：

- `basic_example.cpp` - 基本操作
- `pipeline_example.cpp` - 多步骤处理
- `batch_example.cpp` - 批量处理
- `benchmark_example.cpp` - 性能测试