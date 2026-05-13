---
layout: home
---

<div class="home-hero">
  <div class="hero-badge">
    <span class="badge-dot"></span>
    Production Ready · CUDA 12.x · C++17
  </div>
  
  <h1 class="hero-title">
    <span class="gradient">GPU-Accelerated</span> Image Processing
  </h1>
  
  <p class="hero-subtitle">
    High-performance CUDA library for computer vision.<br/>
    Achieve <strong>30-50× speedup</strong> over CPU OpenCV.
  </p>
  
  <div class="hero-actions">
    <a href="./setup/quickstart" class="btn-primary">
      ⚡ Quick Start
    </a>
    <a href="./architecture/overview" class="btn-secondary">
      📖 Architecture
    </a>
    <a href="https://github.com/LessUp/mini-opencv" class="btn-secondary" target="_blank">
      ⭐ GitHub
    </a>
  </div>
  
  <div class="hero-stats">
    <div class="stat">
      <div class="stat-value">30-50×</div>
      <div class="stat-label">Speedup</div>
    </div>
    <div class="stat">
      <div class="stat-value">9+</div>
      <div class="stat-label">Operators</div>
    </div>
    <div class="stat">
      <div class="stat-value">CC 7.5+</div>
      <div class="stat-label">GPU Arch</div>
    </div>
    <div class="stat">
      <div class="stat-value">MIT</div>
      <div class="stat-label">License</div>
    </div>
  </div>
</div>

## Why Mini-OpenCV?

<div class="feature-grid">
  <div class="feature-card">
    <div class="feature-icon">⚡</div>
    <h3>High Performance</h3>
    <p>CUDA-accelerated operators achieve 30-50× speedup over CPU implementations using shared memory tiling and multi-stream execution.</p>
    <a href="./benchmarks/">View Benchmarks →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🧠</div>
    <h3>Smart Memory</h3>
    <p>Zero-copy optimization minimizes host-device transfers. Memory pool reuse reduces allocation overhead.</p>
    <a href="./architecture/memory-model">Learn More →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🔧</div>
    <h3>Easy Integration</h3>
    <p>Simple C++17 API with automatic memory management. Drop-in replacement for performance-critical code paths.</p>
    <a href="./setup/quickstart">Get Started →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">📊</div>
    <h3>Optimized Kernels</h3>
    <p>Shared memory tiling, atomic operations, texture memory, and warp-level primitives for maximum throughput.</p>
    <a href="./architecture/overview">Architecture →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">🧪</div>
    <h3>Well Tested</h3>
    <p>Comprehensive GoogleTest suite with correctness properties and performance benchmarks.</p>
    <a href="./tutorials/examples">Examples →</a>
  </div>
  
  <div class="feature-card">
    <div class="feature-icon">📖</div>
    <h3>Full Documentation</h3>
    <p>Complete API reference, architecture guides, and tutorials in English and Chinese.</p>
    <a href="./api/">API Docs →</a>
  </div>
</div>

## Performance Comparison

| Operation | OpenCV CPU | Mini-OpenCV GPU | Speedup |
|-----------|:----------:|:---------------:|:-------:|
| Gaussian Blur (4K) | 45.2 ms | 1.2 ms | **37.7×** |
| Sobel Edge (4K) | 38.1 ms | 0.9 ms | **42.3×** |
| Bilateral Filter (4K) | 180.5 ms | 4.8 ms | **37.6×** |
| Histogram Equalization (4K) | 12.3 ms | 0.3 ms | **41.0×** |

*Tested on RTX 4090 vs Intel i9-13900K, 3840×2160 images*

## Quick Start

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

// Create processor and load image
ImageProcessor processor;
GpuImage gpu = processor.loadFromHost(hostImage);

// Apply operations (all GPU-accelerated)
GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(gpu);

// Download result
HostImage result = processor.downloadImage(edges);
```

## Learn More

<div class="docs-grid">
  <a href="./architecture/overview" class="doc-card">
    <div class="doc-icon">🏗️</div>
    <h3>Architecture</h3>
    <p>Three-layer design with CUDA kernel optimizations.</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./api/" class="doc-card">
    <div class="doc-icon">📚</div>
    <h3>API Reference</h3>
    <p>Complete API documentation with examples.</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./benchmarks/" class="doc-card">
    <div class="doc-icon">📊</div>
    <h3>Benchmarks</h3>
    <p>Performance data and optimization techniques.</p>
    <span class="doc-arrow">→</span>
  </a>
  
  <a href="./references/" class="doc-card">
    <div class="doc-icon">📄</div>
    <h3>References</h3>
    <p>Academic papers and related projects.</p>
    <span class="doc-arrow">→</span>
  </a>
</div>