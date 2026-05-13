# 形态学算子

数学形态学操作。

## erode

```cpp
GpuImage erode(const GpuImage& input, int kernelSize);
```

应用腐蚀操作（缩小白色区域）。

## dilate

```cpp
GpuImage dilate(const GpuImage& input, int kernelSize);
```

应用膨胀操作（扩大白色区域）。

## morphologyOpen

```cpp
GpuImage morphologyOpen(const GpuImage& input, int kernelSize);
```

开运算：先腐蚀后膨胀。去除小的亮点。

## morphologyClose

```cpp
GpuImage morphologyClose(const GpuImage& input, int kernelSize);
```

闭运算：先膨胀后腐蚀。填充小的暗洞。

## morphologyGradient

```cpp
GpuImage morphologyGradient(const GpuImage& input, int kernelSize);
```

形态学梯度：膨胀与腐蚀的差值。

## 性能

| 操作 | 4K 图像 | 加速比 |
|------|---------|--------|
| 腐蚀 5×5 | 1.5 ms | 25.0× |
| 膨胀 5×5 | 1.5 ms | 25.0× |
| 开运算 5×5 | 3.0 ms | 25.0× |

[返回 API](../)