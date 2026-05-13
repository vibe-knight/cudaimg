# Convolution Operators

GPU-accelerated convolution operations.

## gaussianBlur

```cpp
GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
```

Applies Gaussian blur with specified kernel size and sigma.

**Parameters:**
- `input`: Input image
- `kernelSize`: Kernel size (must be odd, ≥3)
- `sigma`: Standard deviation (>0)

**Optimization:** Uses shared memory tiling for efficient memory access.

## sobelEdgeDetection

```cpp
GpuImage sobelEdgeDetection(const GpuImage& input);
```

Detects edges using Sobel operator.

**Returns:** Edge magnitude image.

## customConvolution

```cpp
GpuImage convolve(const GpuImage& input, const float* kernel, int kernelSize);
```

Applies custom convolution kernel.

**Parameters:**
- `kernel`: Flattened kernel weights (size × size elements)
- `kernelSize`: Kernel dimension

## Performance

| Kernel | 4K Image | Speedup vs CPU |
|--------|----------|----------------|
| 5×5 Gaussian | 1.2 ms | 37.7× |
| 3×3 Sobel | 0.9 ms | 42.3× |
| 7×7 Custom | 2.1 ms | 31.1× |

[Back to API](../)
