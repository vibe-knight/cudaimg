# Morphology Operators

Mathematical morphology operations.

## erode

```cpp
GpuImage erode(const GpuImage& input, int kernelSize);
```

Applies erosion (shrinks white regions).

## dilate

```cpp
GpuImage dilate(const GpuImage& input, int kernelSize);
```

Applies dilation (expands white regions).

## morphologyOpen

```cpp
GpuImage morphologyOpen(const GpuImage& input, int kernelSize);
```

Opening: erosion followed by dilation. Removes small bright spots.

## morphologyClose

```cpp
GpuImage morphologyClose(const GpuImage& input, int kernelSize);
```

Closing: dilation followed by erosion. Fills small dark holes.

## morphologyGradient

```cpp
GpuImage morphologyGradient(const GpuImage& input, int kernelSize);
```

Morphological gradient: difference between dilation and erosion.

## Performance

| Operation | 4K Image | Speedup |
|-----------|----------|---------|
| Erode 5×5 | 1.5 ms | 25.0× |
| Dilate 5×5 | 1.5 ms | 25.0× |
| Open 5×5 | 3.0 ms | 25.0× |

[Back to API](../)
