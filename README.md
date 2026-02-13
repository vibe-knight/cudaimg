# GPU Image Processing Library

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

基于 CUDA 的高性能图像处理库，类似于 OpenCV 的迷你版本。

## 功能特性

- **像素级操作**: 反色、灰度化、亮度调整
- **卷积操作**: 高斯模糊、Sobel 边缘检测（使用 Shared Memory 优化）
- **直方图**: 计算和均衡化（使用原子操作和并行规约）
- **图像缩放**: 双线性插值、最近邻插值
- **形态学操作**: 腐蚀、膨胀、开运算、闭运算、梯度、顶帽、黑帽
- **阈值处理**: 全局阈值、自适应阈值、Otsu 自动阈值
- **颜色空间**: RGB/HSV/YUV 转换、通道分离与合并
- **几何变换**: 旋转、翻转、仿射变换、透视变换、裁剪、填充
- **滤波器**: 中值滤波、双边滤波、盒式滤波、锐化、拉普拉斯
- **图像算术**: 加法、减法、乘法、混合、加权和、绝对差
- **流水线处理**: 使用 CUDA Streams 实现异步并行处理

## 系统要求

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 兼容编译器
- NVIDIA GPU (Compute Capability 7.5+)

## 构建

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 构建选项

- `BUILD_TESTS`: 构建测试 (默认 ON)
- `BUILD_EXAMPLES`: 构建示例程序 (默认 ON)

```bash
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
```

## 使用示例

### 基础使用

```cpp
#include "gpu_image/gpu_image_processing.hpp"

using namespace gpu_image;

int main() {
    ImageProcessor processor;
    
    // 创建测试图像
    HostImage hostImage = ImageUtils::createHostImage(256, 256, 3);
    // ... 填充图像数据 ...
    
    // 上传到 GPU
    GpuImage gpuImage = processor.loadFromHost(hostImage);
    
    // 处理
    GpuImage inverted = processor.invert(gpuImage);
    GpuImage blurred = processor.gaussianBlur(gpuImage, 5, 1.5f);
    GpuImage edges = processor.sobelEdgeDetection(gpuImage);
    
    // 下载结果
    HostImage result = processor.downloadImage(inverted);
    
    return 0;
}
```

### 流水线处理

```cpp
#include "gpu_image/gpu_image_processing.hpp"

using namespace gpu_image;

int main() {
    PipelineProcessor pipeline(4);  // 4 个 CUDA streams
    
    // 添加处理步骤
    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        PixelOperator::adjustBrightness(img, temp, 20, stream);
        img = std::move(temp);
    });
    
    pipeline.addStep([](GpuImage& img, cudaStream_t stream) {
        GpuImage temp;
        ConvolutionEngine::gaussianBlur(img, temp, 3, 1.0f, stream);
        img = std::move(temp);
    });
    
    // 批量处理
    std::vector<HostImage> inputs = /* ... */;
    std::vector<HostImage> outputs = pipeline.processBatchHost(inputs);
    
    return 0;
}
```

## 运行测试

```bash
cd build
ctest --output-on-failure
```

## 运行基准测试

```bash
./bin/benchmark
```

## 项目结构

```
├── include/gpu_image/     # 头文件
│   ├── cuda_error.hpp     # CUDA 错误处理
│   ├── device_buffer.hpp  # GPU 内存管理
│   ├── gpu_image.hpp      # 图像数据结构
│   ├── pixel_operator.hpp # 像素操作
│   ├── convolution_engine.hpp  # 卷积操作
│   ├── histogram_calculator.hpp # 直方图
│   ├── image_resizer.hpp  # 图像缩放
│   ├── morphology.hpp     # 形态学操作
│   ├── threshold.hpp      # 阈值处理
│   ├── color_space.hpp    # 颜色空间转换
│   ├── geometric.hpp      # 几何变换
│   ├── filters.hpp        # 滤波器和图像算术
│   ├── image_processor.hpp # 高级 API
│   └── pipeline_processor.hpp # 流水线处理
├── src/                   # 源文件
├── tests/                 # 单元测试
├── examples/              # 示例程序
├── benchmarks/            # 性能基准测试
└── CMakeLists.txt
```

## API 参考

### ImageProcessor

主要的用户接口类，提供同步的图像处理方法。

| 方法 | 描述 |
|------|------|
| `loadFromHost()` | 从 Host 内存加载图像到 GPU |
| `downloadImage()` | 从 GPU 下载图像到 Host |
| `invert()` | 图像反色 |
| `toGrayscale()` | RGB 转灰度 |
| `adjustBrightness()` | 亮度调整 |
| `gaussianBlur()` | 高斯模糊 |
| `sobelEdgeDetection()` | Sobel 边缘检测 |
| `histogram()` | 计算直方图 |
| `resize()` | 图像缩放 |

### Geometric

几何变换类，提供各种空间变换操作。

| 方法 | 描述 |
|------|------|
| `rotate()` | 任意角度旋转（双线性插值） |
| `rotate90()` | 90度倍数旋转（高效） |
| `flip()` | 图像翻转（水平/垂直/双向） |
| `affineTransform()` | 仿射变换 |
| `perspectiveTransform()` | 透视变换 |
| `crop()` | 图像裁剪 |
| `pad()` | 图像填充 |

### Filters

滤波器类，提供各种图像滤波操作。

| 方法 | 描述 |
|------|------|
| `medianFilter()` | 中值滤波（去噪） |
| `bilateralFilter()` | 双边滤波（保边去噪） |
| `boxFilter()` | 盒式滤波（均值滤波） |
| `sharpen()` | 锐化滤波 |
| `laplacian()` | 拉普拉斯滤波（边缘增强） |

### ImageArithmetic

图像算术操作类。

| 方法 | 描述 |
|------|------|
| `add()` | 图像加法 |
| `subtract()` | 图像减法 |
| `multiply()` | 图像乘法 |
| `blend()` | Alpha 混合 |
| `addWeighted()` | 加权和 |
| `absDiff()` | 绝对差 |
| `addScalar()` | 标量加法 |
| `multiplyScalar()` | 标量乘法 |

### PipelineProcessor

流水线处理器，支持异步批量处理。

| 方法 | 描述 |
|------|------|
| `addStep()` | 添加处理步骤 |
| `processHost()` | 处理单张 Host 图像 |
| `processBatchHost()` | 批量处理 Host 图像 |
| `synchronize()` | 同步所有操作 |

## 许可证

MIT License
