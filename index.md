---
layout: default
title: Mini-OpenCV
---

# Mini-OpenCV

GPU Image Processing Library — CUDA 图像处理库，支持卷积、形态学、几何变换和流水线处理。

## 核心特性

- **卷积算子** — 高斯模糊、Sobel 边缘检测、自定义卷积核
- **形态学算子** — 膨胀、腐蚀、开运算、闭运算
- **几何变换** — 缩放、旋转、仿射变换
- **色彩空间** — RGB/Grayscale/HSV 转换
- **流水线处理** — 串联多个算子组成处理管线
- **CUDA 加速** — 所有算子均 GPU 并行实现

## 快速开始

```bash
# 构建
mkdir build && cd build
cmake ..
make -j$(nproc)

# 运行测试
ctest --output-on-failure
```

## 技术栈

| 类别 | 技术 |
|------|------|
| 语言 | CUDA C++17 |
| 构建 | CMake 3.18+ |
| GPU | SM 70+ (Volta → Hopper) |
| 测试 | Google Test |

## 链接

- [GitHub 仓库](https://github.com/LessUp/mini-opencv)
- [README](README.md)
