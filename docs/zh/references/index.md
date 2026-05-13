# 参考文献与引用

影响 Mini-OpenCV 设计的学术论文和相关项目。

## 基础论文

### CUDA 编程

::: cite-card
**CUDA C++ 编程指南**
*NVIDIA 文档, 2023*

CUDA 编程模型、内存层次结构和优化技术的官方指南。GPU 编程必读。

[PDF](https://docs.nvidia.com/cuda/pdf/CUDA_C_Programming_Guide.pdf)
:::

### 并行归约

::: cite-card
**CUDA 中并行归约的优化**
*Mark Harris, NVIDIA Developer, 2007*

使用共享内存和 warp shuffle 操作的并行归约模式经典论文。我们直方图实现的基础。

[链接](https://developer.nvidia.com/content/optimizing-parallel-reduction-cuda)
:::

### 卷积优化

::: cite-card
**通过共享内存实现高效 CUDA 卷积**
*Victor Podlozhnyuk, NVIDIA, 2007*

使用共享内存分块实现缓存高效的图像卷积优化。我们卷积内核设计的基础。

[PDF](https://developer.download.nvidia.com/compute/cuda/1.1-Beta/x86_website/Data/Articles/convolutionSeparable.pdf)
:::

### 图像处理

::: cite-card
**数字图像处理**
*Rafael C. Gonzalez, Richard E. Woods*

涵盖图像处理基础的综合性教材：变换、滤波、形态学和分割。

[亚马逊](https://www.amazon.com/Digital-Image-Processing-Rafael-Gonzalez/dp/0133356728)
:::

## 相关项目

| 项目 | 重点 | 语言 | 许可证 |
|------|------|------|--------|
| [OpenCV](https://opencv.org/) | CPU 图像处理 | C++ | Apache 2.0 |
| [CUTLASS](https://github.com/NVIDIA/cutlass) | CUDA 模板库 | C++ | BSD-3 |
| [NPP](https://docs.nvidia.com/cuda/npp/) | NVIDIA 图元库 | C | 专有 |
| [VPI](https://developer.nvidia.com/nvidia-vpi) | 视觉编程接口 | C++ | 专有 |
| [cv::cuda](https://docs.opencv.org/4.x/d2/d3c/group__cudaimgproc.html) | OpenCV CUDA 模块 | C++ | Apache 2.0 |

## 设计灵感

### 内存管理

- **RAII 模式**: 受现代 C++ 实践启发的自动 GPU 内存管理
- **内存池**: 可复用缓冲区减少分配开销

### 内核设计

- **分块卷积**: 来自 NVIDIA CUDA 示例的共享内存优化
- **原子直方图**: 使用原子操作的并行归约

### API 设计

- **流式接口**: 方法链实现清晰代码
- **类型安全**: 图像格式和尺寸的强类型

## 如何引用

如果您在研究或项目中使用 Mini-OpenCV，请引用：

```bibtex
@software{mini_opencv_2024,
  title = {Mini-OpenCV: 高性能 CUDA 图像处理库},
  author = {LessUp},
  year = {2024},
  version = {2.1.0},
  url = {https://github.com/LessUp/mini-opencv},
  note = {CUDA 加速算子，相比 CPU 实现获得 30-50 倍加速}
}
```

## 贡献参考文献

发现了相关的论文或项目？请[提交 Issue](https://github.com/LessUp/mini-opencv/issues) 或发送 Pull Request！