---
layout: home
---

<div class="home-hero">
  <div class="hero-badge">
    <span class="badge-dot"></span>
    生产就绪 · CUDA 12.x · C++17
  </div>
  
  <h1 class="hero-title">
    <span class="gradient">GPU 加速</span>图像处理库
  </h1>
  
  <p class="hero-subtitle">
    高性能 CUDA 计算机视觉库。<br/>
    相比 CPU OpenCV 实现 <strong>30-50 倍加速</strong>。
  </p>
  
  <div class="hero-actions">
    <a href="./setup/quickstart" class="btn-primary">
      ⚡ 快速开始
    </a>
    <a href="./architecture/overview" class="btn-secondary">
      📖 架构设计
    </a>
    <a href="https://github.com/LessUp/mini-opencv" class="btn-secondary" target="_blank">
      ⭐ GitHub
    </a>
  </div>
  
  <div class="hero-stats">
    <div class="stat">
      <div class="stat-value">30-50×</div>
      <div class="stat-label">加速比</div>
    </div>
    <div class="stat">
      <div class="stat-value">9+</div>
      <div class="stat-label">算子</div>
    </div>
    <div class="stat">
      <div class="stat-value">CC 7.5+</div>
      <div class="stat-label">GPU 架构</div>
    </div>
    <div class="stat">
      <div class="stat-value">MIT</div>
      <div class="stat-label">许可证</div>
    </div>
  </div>
</div>

## 为什么选择 Mini-OpenCV？

<div class="feature-grid">
  <div class="feature-card">
    <div class="feature-icon">⚡</div>
    <h3>高性能</h3>
    <p>CUDA 加速算子通过共享内存分块和多流执行，相比 CPU 实现获得 30-50 倍加速。</p>
    <a href="./benchmarks/">查看性能数据 →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🧠</div>
    <h3>智能内存</h3>
    <p>零拷贝优化最小化主机-设备传输。内存池复用减少分配开销。</p>
    <a href="./architecture/memory-model">了解更多 →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🔧</div>
    <h3>易于集成</h3>
    <p>简洁的 C++17 API，自动内存管理。可作为性能关键路径的即插即用替代方案。</p>
    <a href="./setup/quickstart">快速开始 →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">📊</div>
    <h3>优化内核</h3>
    <p>共享内存分块、原子操作、纹理内存和 warp 级原语实现最大吞吐量。</p>
    <a href="./architecture/overview">架构详解 →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🧪</div>
    <h3>充分测试</h3>
    <p>完整的 GoogleTest 测试套件，包含正确性属性验证和性能基准测试。</p>
    <a href="./tutorials/examples">示例代码 →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">📖</div>
    <h3>完整文档</h3>
    <p>中英双语的完整 API 参考、架构指南和教程。</p>
    <a href="./api/">API 文档 →</a>
  </div>
</div>

## 性能对比

| 操作 | OpenCV CPU | Mini-OpenCV GPU | 加速比 |
|------|:----------:|:---------------:|:------:|
| 高斯模糊 (4K) | 45.2 ms | 1.2 ms | **37.7×** |
| Sobel 边缘检测 (4K) | 38.1 ms | 0.9 ms | **42.3×** |
| 双边滤波 (4K) | 180.5 ms | 4.8 ms | **37.6×** |
| 直方图均衡化 (4K) | 12.3 ms | 0.3 ms | **41.0×** |

*测试环境：RTX 4090 vs Intel i9-13900K，3840×2160 图像*

## 快速开始

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

// 创建处理器并加载图像
ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);

// 应用操作（全部 GPU 加速）
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);

// 下载结果
HostImage result = processor.downloadImage(edges);
```

## 了解更多

<div class="docs-grid">
  <a href="./architecture/overview" class="doc-card">
    <div class="doc-icon">🏗️</div>
    <h3>架构设计</h3>
    <p>三层架构设计，CUDA 内核优化详解。</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./api/" class="doc-card">
    <div class="doc-icon">📚</div>
    <h3>API 参考</h3>
    <p>完整的 API 文档和示例代码。</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./benchmarks/" class="doc-card">
    <div class="doc-icon">📊</div>
    <h3>性能基准</h3>
    <p>性能数据和优化技术详解。</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./references/" class="doc-card">
    <div class="doc-icon">📄</div>
    <h3>学术引用</h3>
    <p>相关论文和开源项目。</p>
    <span class="doc-arrow">→</span>
  </a>
</div>