# ImageProcessor

GPU 图像操作的主入口。

## 构造函数

```cpp
ImageProcessor();
```

创建带有默认 CUDA 流的新处理器。

## 图像传输

### loadFromHost

```cpp
GpuImage loadFromHost(const HostImage& host);
```

将图像数据从主机上传到 GPU 内存。

**参数：**
- `host`: 主机图像数据

**返回：** 数据在 GPU 上的 `GpuImage`

### downloadImage

```cpp
HostImage downloadImage(const GpuImage& gpu);
```

将图像数据从 GPU 下载到主机内存。

**参数：**
- `gpu`: GPU 图像

**返回：** 数据在 CPU 上的 `HostImage`

## 像素操作

### grayscale

```cpp
GpuImage grayscale(const GpuImage& input);
```

将 RGB 图像转换为灰度图。

### invert

```cpp
GpuImage invert(const GpuImage& input);
```

反转图像颜色。

### brightness

```cpp
GpuImage brightness(const GpuImage& input, float factor);
```

调整图像亮度。

## 卷积

### gaussianBlur

```cpp
GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
```

应用高斯模糊。

**参数：**
- `kernelSize`: 必须为奇数，≥3
- `sigma`: 标准差，>0

### sobelEdgeDetection

```cpp
GpuImage sobelEdgeDetection(const GpuImage& input);
```

使用 Sobel 算子检测边缘。

## 几何变换

### resize

```cpp
GpuImage resize(const GpuImage& input, int width, int height);
```

使用双线性插值缩放图像。

### rotate

```cpp
GpuImage rotate(const GpuImage& input, float angle);
```

按角度（度）旋转图像。

## 阈值处理

### threshold

```cpp
GpuImage threshold(const GpuImage& input, float value);
```

应用二值阈值。

## 示例

```cpp
ImageProcessor processor;

HostImage input = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(input);

GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(blurred);
GpuImage result = processor.threshold(edges, 128);

HostImage output = processor.downloadImage(result);
ImageIO::save("output.jpg", output);
```