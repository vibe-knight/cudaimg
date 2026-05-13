# 滤波算子

图像滤波操作。

## medianFilter

```cpp
GpuImage medianFilter(const GpuImage& input, int kernelSize);
```

应用中值滤波进行降噪，同时保持边缘。

## bilateralFilter

```cpp
GpuImage bilateralFilter(const GpuImage& input, float sigmaSpace, float sigmaColor);
```

边缘保持平滑滤波器。

**参数：**
- `sigmaSpace`: 空间范围
- `sigmaColor`: 颜色相似度阈值

## boxFilter

```cpp
GpuImage boxFilter(const GpuImage& input, int kernelSize);
```

应用盒式（均值）滤波。

## sharpen

```cpp
GpuImage sharpen(const GpuImage& input, float strength = 1.0f);
```

使用锐化掩模增强图像清晰度。

## 性能

| 滤波器 | 4K 图像 | 加速比 |
|--------|---------|--------|
| 中值 3×3 | 2.5 ms | 11.4× |
| 双边 | 4.8 ms | 37.6× |
| 盒式 5×5 | 0.8 ms | 31.5× |
| 锐化 | 1.1 ms | 38.3× |

[返回 API](../)