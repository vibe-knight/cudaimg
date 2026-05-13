# Convolution Performance

Detailed benchmarks for convolution operations.

## Gaussian Blur

### Varying Kernel Size (4K Image)

| Kernel | CPU (ms) | GPU (ms) | Speedup |
|--------|----------|----------|---------|
| 3×3 | 12.5 | 0.4 | **31.3×** |
| 5×5 | 45.2 | 1.2 | **37.7×** |
| 7×7 | 78.4 | 1.8 | **43.6×** |
| 9×9 | 110.2 | 2.4 | **45.9×** |
| 15×15 | 120.5 | 3.8 | **31.7×** |

### Varying Image Size (5×5 Kernel)

| Image | CPU (ms) | GPU (ms) | Speedup |
|-------|----------|----------|---------|
| HD | 3.2 | 0.2 | **16.0×** |
| FHD | 10.5 | 0.5 | **21.0×** |
| 4K | 45.2 | 1.2 | **37.7×** |
| 8K | 180.4 | 4.5 | **40.1×** |

## Sobel Edge Detection

| Image | CPU (ms) | GPU (ms) | Speedup |
|-------|----------|----------|---------|
| HD | 8.1 | 0.3 | **27.0×** |
| FHD | 18.2 | 0.5 | **36.4×** |
| 4K | 38.1 | 0.9 | **42.3×** |
| 8K | 150.2 | 3.2 | **46.9×** |

## Custom Kernels

For 7×7 custom convolution kernel:

| Image | CPU (ms) | GPU (ms) | Speedup |
|-------|----------|----------|---------|
| HD | 15.2 | 0.5 | **30.4×** |
| FHD | 32.4 | 1.0 | **32.4×** |
| 4K | 65.3 | 2.1 | **31.1×** |
| 8K | 260.1 | 8.2 | **31.7×** |

## Optimization Notes

- Shared memory tiling used for all kernels
- Optimal performance with kernel sizes ≤ 15
- Larger kernels use separable convolution when possible

## Back to Benchmarks

[Benchmark Overview](./)
