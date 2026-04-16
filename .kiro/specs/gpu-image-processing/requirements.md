# Requirements Document

[← Back to Specs](.) | [Design](design.md) | [Tasks](tasks.md)

---

## Table of Contents

- [Introduction](#introduction)
- [Glossary](#glossary)
- [Requirements](#requirements)
  - [Requirement 1: Image Data Management](#requirement-1-image-data-management)
  - [Requirement 2: Pixel-Level Operations](#requirement-2-pixel-level-operations)
  - [Requirement 3: Convolution Operations](#requirement-3-convolution-operations)
  - [Requirement 4: Histogram Statistics](#requirement-4-histogram-statistics)
  - [Requirement 5: Image Scaling](#requirement-5-image-scaling)
  - [Requirement 6: Pipeline Processing](#requirement-6-pipeline-processing)
  - [Requirement 7: Error Handling & Resource Management](#requirement-7-error-handling--resource-management)
- [Non-Functional Requirements](#non-functional-requirements)
- [Traceability Matrix](#traceability-matrix)

---

## Introduction

本项目旨在构建一个基于 GPU 加速的高性能图像处理库，类似于 OpenCV 的迷你版本，但底层全部使用 CUDA 加速。该库将帮助开发者理解异构计算的核心概念，包括 Host-Device 数据传输、并行算法（Map、Reduce、Stencil）以及 GPU 内存优化技术。

### Project Goals

| Goal | Description |
|------|-------------|
| **High Performance** | GPU-accelerated image processing with CUDA |
| **OpenCV-like API** | Familiar API design for easy adoption |
| **Modular Architecture** | Clean separation between memory, compute, and API layers |
| **Educational Value** | Clear code structure for learning GPU computing |

---

## Glossary

| Term | Definition |
|------|------------|
| **Host** | CPU and its memory space |
| **Device** | GPU and its memory space |
| **Kernel** | Parallel function executed on GPU |
| **Shared_Memory** | On-chip high-speed cache shared by threads in a block |
| **Atomic_Operation** | Operation ensuring data consistency under concurrent access |
| **CUDA_Stream** | CUDA stream for async operations and pipeline processing |
| **Image_Processor** | Core module for scheduling GPU computation |
| **Memory_Manager** | Memory manager for Host-Device data transfer |
| **Pixel_Operator** | Pixel-level operation module |
| **Convolution_Engine** | Convolution engine for filter operations |
| **Histogram_Calculator** | Histogram calculator for statistical operations |

---

## Requirements

### Requirement 1: Image Data Management

**User Story:** 作为开发者，我希望能够方便地在 CPU 和 GPU 之间传输图像数据，以便进行 GPU 加速处理。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 1.1 | 用户加载一张图像 | Memory_Manager SHALL 将图像数据从 Host 内存复制到 Device 内存 |
| 1.2 | GPU 处理完成 | Memory_Manager SHALL 将结果数据从 Device 内存复制回 Host 内存 |
| 1.3 | 用户请求释放资源 | Memory_Manager SHALL 释放所有已分配的 Device 内存 |
| 1.4 | 内存分配失败 | Memory_Manager SHALL 返回明确的错误信息并保持系统稳定状态 |

---

### Requirement 2: Pixel-Level Operations

**User Story:** 作为开发者，我希望能够对图像执行基础的像素级操作，以便进行简单的图像变换。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 2.1 | 用户请求图像反色操作 | Pixel_Operator SHALL 将每个像素值变换为 (255 - 原值) |
| 2.2 | 用户请求灰度化操作 | Pixel_Operator SHALL 使用加权公式 (0.299*R + 0.587*G + 0.114*B) 将 RGB 图像转换为灰度图像 |
| 2.3 | 用户请求亮度调整操作 | Pixel_Operator SHALL 将每个像素值加上指定的偏移量并裁剪到 [0, 255] 范围内 |
| 2.4 | 处理大尺寸图像 | Pixel_Operator SHALL 利用 GPU 并行性实现比 CPU 串行处理更快的执行速度 |

---

### Requirement 3: Convolution Operations

**User Story:** 作为开发者，我希望能够对图像执行卷积操作，以便实现模糊和边缘检测等效果。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 3.1 | 用户请求高斯模糊操作 | Convolution_Engine SHALL 使用指定大小的高斯核对图像进行卷积 |
| 3.2 | 用户请求 Sobel 边缘检测 | Convolution_Engine SHALL 分别计算水平和垂直方向的梯度并合成边缘强度图 |
| 3.3 | 执行卷积操作 | Convolution_Engine SHALL 使用 Shared_Memory 优化卷积核的访存模式 |
| 3.4 | 处理图像边界像素 | Convolution_Engine SHALL 使用零填充或镜像填充策略处理边界条件 |

---

### Requirement 4: Histogram Statistics

**User Story:** 作为开发者，我希望能够计算图像的直方图，以便分析图像的亮度分布。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 4.1 | 用户请求计算直方图 | Histogram_Calculator SHALL 返回包含 256 个 bin 的灰度直方图数组 |
| 4.2 | 多个线程同时更新同一 bin | Histogram_Calculator SHALL 使用 Atomic_Operation 保证计数正确性 |
| 4.3 | 计算完成 | Histogram_Calculator SHALL 使用 Parallel Reduction 技术合并各线程块的局部直方图 |

---

### Requirement 5: Image Scaling

**User Story:** 作为开发者，我希望能够对图像进行缩放操作，以便调整图像尺寸。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 5.1 | 用户请求缩放图像 | Image_Processor SHALL 使用双线性插值算法计算目标像素值 |
| 5.2 | 目标像素位置对应源图像的非整数坐标 | Image_Processor SHALL 使用周围四个像素进行加权插值 |
| 5.3 | 缩放比例小于 1 | Image_Processor SHALL 正确处理下采样情况 |
| 5.4 | 缩放比例大于 1 | Image_Processor SHALL 正确处理上采样情况 |

---

### Requirement 6: Pipeline Processing

**User Story:** 作为开发者，我希望能够使用流水线方式处理多张图像，以便最大化 GPU 利用率。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 6.1 | 用户启用流水线模式 | Image_Processor SHALL 使用 CUDA_Stream 实现异步操作 |
| 6.2 | 流水线运行中 | Image_Processor SHALL 同时执行上传、处理和下载操作 |
| 6.3 | 处理多张图像 | Image_Processor SHALL 通过流水线重叠隐藏数据传输延迟 |
| 6.4 | 用户请求同步 | Image_Processor SHALL 等待所有流中的操作完成 |

---

### Requirement 7: Error Handling & Resource Management

**User Story:** 作为开发者，我希望库能够正确处理错误并管理资源，以便编写健壮的应用程序。

#### Acceptance Criteria

| ID | Condition | Expected Behavior |
|----|-----------|-------------------|
| 7.1 | CUDA 运行时返回错误 | Image_Processor SHALL 捕获错误并提供有意义的错误消息 |
| 7.2 | 对象销毁时 | Memory_Manager SHALL 自动释放所有关联的 GPU 资源 |
| 7.3 | 用户传入无效参数 | Image_Processor SHALL 在执行前验证参数并返回错误 |
| 7.4 | 发生错误 | Image_Processor SHALL 保持系统处于一致状态，不泄漏资源 |

---

## Non-Functional Requirements

### Performance Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-1 | GPU pixel operations | > 10x faster than CPU for images > 1MP |
| NFR-2 | Memory transfer overhead | < 10% of total processing time |
| NFR-3 | Pipeline efficiency | > 80% GPU utilization |

### Quality Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-4 | Test coverage | > 80% for core modules |
| NFR-5 | Code documentation | All public APIs documented |
| NFR-6 | Build portability | Support Linux, Windows |

### Compatibility Requirements

| ID | Requirement | Target |
|----|-------------|--------|
| NFR-7 | CUDA version | 11.0+ |
| NFR-8 | C++ standard | C++17 |
| NFR-9 | GPU architecture | SM 75+ (Turing and later) |

---

## Traceability Matrix

### Requirements to Properties

| Requirement | Property | Test Type |
|-------------|----------|-----------|
| 1.1, 1.2 | Data Transfer Round-Trip | Property Test |
| 2.1 | Invert Involution | Property Test |
| 2.2 | Grayscale Formula | Property Test |
| 2.3 | Brightness Range | Property Test |
| 3.1, 3.2 | Convolution Consistency | Property Test |
| 3.4 | Boundary Handling | Property Test |
| 4.1 | Histogram Sum | Property Test |
| 5.1 | Scaling Approximate Round-Trip | Property Test |
| 6.4 | Pipeline Confluence | Property Test |
| 7.x | Error Handling | Unit Test |

### Requirements to Components

| Requirement | Primary Component | Secondary Components |
|-------------|-------------------|---------------------|
| 1.x | MemoryManager | DeviceBuffer, StreamManager |
| 2.x | PixelOperator | GpuImage, DeviceBuffer |
| 3.x | ConvolutionEngine | GpuImage, DeviceBuffer |
| 4.x | HistogramCalculator | GpuImage, DeviceBuffer |
| 5.x | ImageResizer | GpuImage, DeviceBuffer |
| 6.x | PipelineProcessor | StreamManager, ImageProcessor |
| 7.x | CudaException | All components |

---

## Related Documents

- [Design Document](design.md) - Architecture and implementation details
- [Tasks Document](tasks.md) - Implementation task checklist
