# Mini-OpenCV — GPU 图像处理库

[![CI](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml/badge.svg)](https://github.com/LessUp/mini-opencv/actions/workflows/ci.yml)
[![Docs](https://img.shields.io/badge/Docs-GitHub%20Pages-blue?logo=github)](https://lessup.github.io/mini-opencv/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
![CUDA](https://img.shields.io/badge/CUDA-11.0+-76B900?logo=nvidia&logoColor=white)
![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=c%2B%2B&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.18+-064F8C?logo=cmake&logoColor=white)

[English](README.md) | 简体中文

基于 CUDA 的高性能图像处理库 — 覆盖像素操作、卷积、形态学、几何变换、滤波、色彩空间转换与异步流水线处理。所有算子均 GPU 并行实现，提供类 OpenCV 风格的 C++ API。

## 架构总览

```
┌─────────────────────────────────────────────────────────┐
│                    用户应用层                             │
│         ImageProcessor  ·  PipelineProcessor             │
├─────────────────────────────────────────────────────────┤
│                    算子层 (CUDA Kernels)                  │
│  PixelOperator │ ConvolutionEngine │ HistogramCalculator  │
│  ImageResizer  │ Morphology        │ Threshold            │
│  ColorSpace    │ Geometric         │ Filters              │
│  ImageArithmetic                                         │
├─────────────────────────────────────────────────────────┤
│                    基础设施层                             │
│  DeviceBuffer · MemoryManager · StreamManager · CudaError │
│  GpuImage · HostImage · ImageIO (stb)                    │
└─────────────────────────────────────────────────────────┘
```

## 功能特性

| 类别 | 算子 | 优化技术 |
|------|------|----------|
| **像素操作** | 反色、灰度化、亮度调整 | 逐像素并行 |
| **卷积** | 高斯模糊、Sobel 边缘检测、自定义卷积核 | Shared Memory Tiling |
| **直方图** | 计算、均衡化 | 原子操作 + 并行规约 |
| **缩放** | 双线性插值、最近邻插值 | 任意尺寸 |
| **形态学** | 腐蚀、膨胀、开/闭运算、梯度、顶帽、黑帽 | 可自定义结构元素 |
| **阈值** | 全局阈值、自适应阈值、Otsu 自动阈值 | 直方图驱动 |
| **色彩空间** | RGB/HSV/YUV 转换、通道分离/合并 | 批量转换 |
| **几何变换** | 旋转、翻转、仿射/透视变换、裁剪、填充 | 双线性插值 |
| **滤波** | 中值、双边、盒式、锐化、拉普拉斯 | 保边去噪 |
| **图像算术** | 加/减/乘、Alpha 混合、加权和、绝对差 | 标量 & 图像 |
| **流水线** | 多步骤串联、批量异步处理 | 多 CUDA Stream 并发 |

## 系统要求

- CUDA Toolkit 11.0+
- CMake 3.18+
- C++17 兼容编译器
- NVIDIA GPU (Compute Capability 7.5+)

## 构建

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON ..
make -j$(nproc)
```

### 构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_TESTS` | ON | 构建单元测试 (GTest v1.14.0) |
| `BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BUILD_BENCHMARKS` | OFF | 构建基准测试 (Google Benchmark v1.8.3) |
| `GPU_IMAGE_ENABLE_IO` | ON | 启用 stb 图像文件 I/O |

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

## GPU 架构支持

| 架构 | Compute Capability | 代号 |
|------|-------------------|------|
| Turing | SM 75 | RTX 20xx / T4 |
| Ampere | SM 80 / 86 | A100 / RTX 30xx |
| Ada Lovelace | SM 89 | RTX 40xx / L4 |
| Hopper | SM 90 | H100 |

## 项目结构

```
mini-opencv/
├── include/gpu_image/          # 公共头文件（19 个模块）
│   ├── gpu_image_processing.hpp  # 统一入口头文件
│   ├── image_processor.hpp       # 高级同步 API
│   ├── pipeline_processor.hpp    # 流水线异步 API
│   ├── convolution_engine.hpp    # 卷积算子
│   ├── morphology.hpp            # 形态学算子
│   ├── geometric.hpp             # 几何变换
│   ├── filters.hpp               # 滤波 + 图像算术
│   ├── color_space.hpp           # 色彩空间转换
│   ├── threshold.hpp             # 阈值处理
│   ├── device_buffer.hpp         # RAII GPU 内存
│   └── ...                       # cuda_error, gpu_image, stream_manager 等
├── src/                          # CUDA/C++ 源文件（16 个）
├── tests/                        # 单元测试（12 个测试文件）
├── examples/                     # 示例程序
│   ├── basic_example.cpp           # 基础用法
│   └── pipeline_example.cpp        # 流水线用法
├── benchmarks/                   # 性能基准测试
└── CMakeLists.txt                # CMake 构建系统
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

## 工程质量

- **现代 CMake** — target-based 编译选项与 generator expressions，支持 `BUILD_INTERFACE`/`INSTALL_INTERFACE`
- **FetchContent 依赖** — GTest v1.14.0、Google Benchmark v1.8.3、stb（无需手动安装第三方库）
- **CUDA 架构自动检测** — CMake 3.24+ 自动使用 `native`，低版本回退常见架构列表
- **Install 支持** — `gpu_image::gpu_image_processing` CMake 导出目标，可作为依赖库使用
- **版本注入** — 编译期注入 `GPU_IMAGE_VERSION_MAJOR/MINOR/PATCH` 宏
- **CI 流水线** — GitHub Actions 自动化构建 + clang-format 格式检查
- **完整测试** — 12 个测试文件覆盖所有算子模块
- **跨平台编译选项** — GCC/Clang (`-Wall -Wextra -Wpedantic`) + MSVC (`/W4`) 双支持

## 许可证

MIT License
