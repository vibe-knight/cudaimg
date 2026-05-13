# Geometric Operators

Geometric transformation operations.

## resize

```cpp
GpuImage resize(const GpuImage& input, int width, int height);
```

Resizes image using bilinear interpolation.

## rotate

```cpp
GpuImage rotate(const GpuImage& input, float angle);
```

Rotates image by specified angle in degrees.

## flipHorizontal

```cpp
GpuImage flipHorizontal(const GpuImage& input);
```

Flips image horizontally.

## flipVertical

```cpp
GpuImage flipVertical(const GpuImage& input);
```

Flips image vertically.

## affine

```cpp
GpuImage affine(const GpuImage& input, const float matrix[6]);
```

Applies affine transformation.

**Parameters:**
- `matrix`: 2×3 transformation matrix (row-major)

## Performance

| Operation | 4K Image | Speedup |
|-----------|----------|---------|
| Resize 2× | 0.6 ms | 30.5× |
| Rotate | 0.5 ms | 28.0× |
| Flip | 0.1 ms | 21.0× |

[Back to API](../)
