---
layout: default
title: 文档
nav_order: 3
has_children: true
description: GPU Image Processing 完整文档 - CUDA 加速图像处理库指南
---

# 文档

GPU Image Processing 库的完整指南。

## 快速开始

新手从这里开始：

| 指南 | 描述 | 时间 |
|:-----|:-----|:-----|
| [快速开始]({{ site.baseurl }}/setup/quickstart) | 构建、测试并运行第一个程序 | 5 分钟 |
| [安装指南]({{ site.baseurl }}/setup/installation) | 完整安装和配置说明 | 15 分钟 |

## 核心概念

理解库的设计：

| 指南 | 描述 |
|:-----|:-----|
| [架构说明]({{ site.baseurl }}/architecture/architecture) | 三层架构设计和内部原理 |
| [性能优化]({{ site.baseurl }}/tutorials/performance) | 最大吞吐量的最佳实践 |

## 教程

逐步指南：

| 指南 | 级别 | 描述 |
|:-----|:-----|:-----|
| [基础用法]({{ site.baseurl }}/tutorials/examples/basic-usage) | 初级 | 加载、处理和保存图像 |
| [流水线处理]({{ site.baseurl }}/tutorials/examples/pipeline-processing) | 中级 | 批量异步处理 |

## API 参考

完整模块文档：

| 模块 | 描述 |
|:-----|:-----|
| [ImageProcessor]({{ site.baseurl }}/api/image_processor) | 高级同步 API |
| [PipelineProcessor]({{ site.baseurl }}/api/pipeline_processor) | 异步批处理 |
| [ConvolutionEngine]({{ site.baseurl }}/api/convolution_engine) | 卷积运算 |
| [Filters]({{ site.baseurl }}/api/filters) | 图像滤波 |
| [Geometric]({{ site.baseurl }}/api/geometric) | 几何变换 |
| [ColorSpace]({{ site.baseurl }}/api/color_space) | 色彩空间转换 |
| [所有模块]({{ site.baseurl }}/api/) | 完整 API 索引 |

## 参考

| 资源 | 描述 |
|:-----|:-----|
| [常见问题]({{ site.baseurl }}/tutorials/faq) | 常见问题解答 |
| [更新日志](https://github.com/LessUp/mini-opencv/blob/main/CHANGELOG.md) | 版本历史 |
| [贡献指南](https://github.com/LessUp/mini-opencv/blob/main/CONTRIBUTING.md) | 如何贡献 |

---

## 语言选择

- [English (英文)]({{ site.baseurl }}/docs)
- **简体中文** (本页)

---

*最后更新: 2026-04-22*
