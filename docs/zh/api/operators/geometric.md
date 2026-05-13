# 几何算子

几何变换操作。

## resize

```cpp
GpuImage resize(const GpuImage& input, int width, int height);
```

使用双线性插值缩放图像。

## rotate

```cpp
GpuImage rotate(const GpuImage& input, float angle);
```

按指定角度（度）旋转图像。

## flipHorizontal

```cpp
GpuImage flipHorizontal(const GpuImage& input);
```

水平翻转图像。

## flipVertical

```cpp
GpuImage flipVertical(const GpuImage& input);
```

垂直翻转图像。

## affine

```cpp
GpuImage affine(const GpuImage& input, const float matrix[6]);
```

应用仿射变换。

**参数：**
- `matrix`: 2×3 变换矩阵（行优先）

## 性能

| 操作 | 4K 图像 | 加速比 |
|------|---------|--------|
| 缩放 2× | 0.6 ms | 30.5× |
| 旋转 | 0.5 ms | 28.0× |
| 翻转 | 0.1 ms | 21.0× |

[返回 API](../)