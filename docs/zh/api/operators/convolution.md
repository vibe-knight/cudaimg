# 卷积算子

GPU 加速的卷积操作。

## gaussianBlur

```cpp
GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
```

应用指定内核大小和 sigma 的高斯模糊。

**参数：**
- `input`: 输入图像
- `kernelSize`: 内核大小（必须为奇数，≥3）
- `sigma`: 标准差（>0）

**优化：** 使用共享内存分块实现高效内存访问。

## sobelEdgeDetection

```cpp
GpuImage sobelEdgeDetection(const GpuImage& input);
```

使用 Sobel 算子检测边缘。

**返回：** 边缘幅值图像。

## customConvolution

```cpp
GpuImage convolve(const GpuImage& input, const float* kernel, int kernelSize);
```

应用自定义卷积内核。

**参数：**
- `kernel`: 展平的内核权重（size × size 个元素）
- `kernelSize`: 内核维度

## 性能

| 内核 | 4K 图像 | 相对 CPU 加速比 |
|------|---------|----------------|
| 5×5 高斯 | 1.2 ms | 37.7× |
| 3×3 Sobel | 0.9 ms | 42.3× |
| 7×7 自定义 | 2.1 ms | 31.1× |

[返回 API](../)