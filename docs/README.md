---
layout: default
title: Documentation
description: Mini-OpenCV Documentation - Complete guide for GPU-accelerated image processing
---

# Mini-OpenCV Documentation

Welcome to the Mini-OpenCV documentation. This is a comprehensive guide for the high-performance CUDA-based image processing library.

## 📚 Documentation Structure

### Getting Started

| Guide | Description | Estimated Time |
|-------|-------------|----------------|
| [Quick Start](setup/quickstart.md) | Build, test, and run your first program | 5 minutes |
| [Installation](setup/installation.md) | Complete setup and configuration guide | 15 minutes |

### Core Concepts

| Guide | Description |
|-------|-------------|
| [Architecture Overview](architecture/architecture.md) | Three-layer design and internal workings |
| [Performance Optimization](tutorials/performance.md) | Best practices for maximum throughput |

### Practical Guides

| Guide | Description | Level |
|-------|-------------|-------|
| [Basic Usage](tutorials/examples/basic-usage.md) | Load, process, and save images | Beginner |
| [Pipeline Processing](tutorials/examples/pipeline-processing.md) | Batch async processing | Intermediate |
| [API Reference](api/) | Complete module documentation | All levels |

### Reference

| Resource | Description |
|----------|-------------|
| [FAQ](tutorials/faq.md) | Frequently asked questions |
| [API Reference](api/) | Complete API documentation |
| [Examples](tutorials/examples/) | Code examples and tutorials |

## 🔗 Specifications

For technical requirements and design documents, see:

| Document | Description |
|----------|-------------|
| [Product Requirements](../specs/product/gpu-image-processing-requirements.md) | Feature definitions and acceptance criteria |
| [Architecture Design](../specs/rfc/0001-gpu-image-processing-design.md) | Technical design and architecture |
| [Implementation Tasks](../specs/rfc/0001-gpu-image-processing-tasks.md) | Implementation task checklist |

## 🚀 Quick Navigation

### By Task

| I want to... | Go to... |
|--------------|----------|
| Install the library | [Installation Guide](setup/installation.md) |
| Process my first image | [Quick Start](setup/quickstart.md) |
| Understand the design | [Architecture](architecture/architecture.md) |
| Optimize performance | [Performance Guide](tutorials/performance.md) |
| Find function reference | [API Reference](api/) |
| Get help with errors | [FAQ](tutorials/faq.md) |

### By Module

| Module | Purpose |
|--------|---------|
| [ImageProcessor](api/image_processor.md) | High-level synchronous API |
| [PipelineProcessor](api/pipeline_processor.md) | Async batch processing |
| [ConvolutionEngine](api/convolution_engine.md) | Convolution operations |
| [Geometric](api/geometric.md) | Geometric transformations |
| [Filters](api/filters.md) | Image filtering |
| [ColorSpace](api/color_space.md) | Color space conversions |

## 🌐 Language Selection

- **English** (this page)
- [简体中文 (Chinese)](README.zh-CN.md)

## 📖 Version Information

**Current Version:** 2.0.0

See [Changelog](../CHANGELOG.md) for version history.

## 💡 Getting Help

- **Found a bug?** [Open an issue](https://github.com/LessUp/mini-opencv/issues)
- **Have a question?** [Start a discussion](https://github.com/LessUp/mini-opencv/discussions)
- **Want to contribute?** See [Contributing Guide](../CONTRIBUTING.md)

---

*Last updated: 2026-04-17*
