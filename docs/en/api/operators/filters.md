# Filter Operators

Image filtering operations.

## medianFilter

```cpp
GpuImage medianFilter(const GpuImage& input, int kernelSize);
```

Applies median filter for noise reduction while preserving edges.

## bilateralFilter

```cpp
GpuImage bilateralFilter(const GpuImage& input, float sigmaSpace, float sigmaColor);
```

Edge-preserving smoothing filter.

**Parameters:**
- `sigmaSpace`: Spatial extent
- `sigmaColor`: Color similarity threshold

## boxFilter

```cpp
GpuImage boxFilter(const GpuImage& input, int kernelSize);
```

Applies box (mean) filter.

## sharpen

```cpp
GpuImage sharpen(const GpuImage& input, float strength = 1.0f);
```

Sharpens image using unsharp masking.

## Performance

| Filter | 4K Image | Speedup |
|--------|----------|---------|
| Median 3×3 | 2.5 ms | 11.4× |
| Bilateral | 4.8 ms | 37.6× |
| Box 5×5 | 0.8 ms | 31.5× |
| Sharpen | 1.1 ms | 38.3× |

[Back to API](../)
