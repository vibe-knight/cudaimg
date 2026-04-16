---
layout: default
title: 文档
description: Mini-OpenCV 文档 - GPU 加速图像处理完整指南
---

# Mini-OpenCV 文档

欢迎使用 Mini-OpenCV 文档。这是高性能 CUDA 图像处理库的完整指南。

## 📚 文档结构

### 快速入门

| 指南 | 描述 | 预计时间 |
|------|------|----------|
| [快速入门](setup/quickstart.zh-CN.md) | 构建、测试并运行第一个程序 | 5 分钟 |
| [安装指南](setup/installation.zh-CN.md) | 完整的安装和配置指南 | 15 分钟 |

### 核心概念

| 指南 | 描述 |
|------|------|
| [架构概览](architecture/architecture.zh-CN.md) | 三层架构设计和内部工作原理 |
| [性能优化](tutorials/performance.zh-CN.md) | 最大吞吐量的最佳实践 |

### 实用指南

| 指南 | 描述 | 级别 |
|------|------|------|
| [基础使用](tutorials/examples/basic-usage.zh-CN.md) | 加载、处理和保存图像 | 初级 |
| [流水线处理](tutorials/examples/pipeline-processing.zh-CN.md) | 批量异步处理 | 中级 |
| [API 参考](api.zh-CN/) | 完整的模块文档 | 所有级别 |

### 参考文档

| 资源 | 描述 |
|------|------|
| [常见问题](tutorials/faq.zh-CN.md) | 常见问题解答 |
| [API 参考](api.zh-CN/) | 完整的 API 文档 |
| [示例代码](tutorials/examples/) | 代码示例和教程 |

## 🔗 规范文档

技术需求和设计文档请参阅：

| 文档 | 描述 |
|------|------|
| [产品需求](../specs/product/gpu-image-processing-requirements.md) | 功能定义和验收标准 |
| [架构设计](../specs/rfc/0001-gpu-image-processing-design.md) | 技术设计和架构 |
| [实现任务](../specs/rfc/0001-gpu-image-processing-tasks.md) | 实现任务清单 |

## 🚀 快速导航

### 按任务

| 我想... | 前往... |
|---------|---------|
| 安装库 | [安装指南](setup/installation.zh-CN.md) |
| 处理第一张图像 | [快速入门](setup/quickstart.zh-CN.md) |
| 了解设计 | [架构](architecture/architecture.zh-CN.md) |
| 优化性能 | [性能指南](tutorials/performance.zh-CN.md) |
| 查找函数参考 | [API 参考](api.zh-CN/) |
| 获取错误帮助 | [常见问题](tutorials/faq.zh-CN.md) |

### 按模块

| 模块 | 用途 |
|------|------|
| [ImageProcessor](api.zh-CN/image_processor.md) | 高级同步 API |
| [PipelineProcessor](api.zh-CN/pipeline_processor.md) | 异步批处理 |
| [ConvolutionEngine](api.zh-CN/convolution_engine.md) | 卷积运算 |
| [Geometric](api.zh-CN/geometric.md) | 几何变换 |
| [Filters](api.zh-CN/filters.md) | 图像滤波 |
| [ColorSpace](api.zh-CN/color_space.md) | 色彩空间转换 |

## 🌐 语言选择

- [English (英文)](README.md)
- **简体中文** (本页)

## 📖 版本信息

**当前版本:** 2.0.0

查看 [更新日志](../CHANGELOG.md) 了解版本历史。

## 💡 获取帮助

- **发现 Bug?** [提交 Issue](https://github.com/LessUp/mini-opencv/issues)
- **有问题?** [发起讨论](https://github.com/LessUp/mini-opencv/discussions)
- **想贡献?** 参阅 [贡献指南](../CONTRIBUTING.md)

---

*最后更新: 2026-04-17*
