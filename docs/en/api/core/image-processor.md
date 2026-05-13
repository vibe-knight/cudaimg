# ImageProcessor

Main entry point for GPU image operations.

## Constructor

```cpp
ImageProcessor();
```

Creates a new processor with default CUDA stream.

## Image Transfer

### loadFromHost

```cpp
GpuImage loadFromHost(const HostImage& host);
```

Uploads image data from host to GPU memory.

**Parameters:**
- `host`: Host image data

**Returns:** `GpuImage` with data on GPU

### downloadImage

```cpp
HostImage downloadImage(const GpuImage& gpu);
```

Downloads image data from GPU to host memory.

**Parameters:**
- `gpu`: GPU image

**Returns:** `HostImage` with data on CPU

## Pixel Operations

### grayscale

```cpp
GpuImage grayscale(const GpuImage& input);
```

Converts RGB image to grayscale.

### invert

```cpp
GpuImage invert(const GpuImage& input);
```

Inverts image colors.

### brightness

```cpp
GpuImage brightness(const GpuImage& input, float factor);
```

Adjusts image brightness.

## Convolution

### gaussianBlur

```cpp
GpuImage gaussianBlur(const GpuImage& input, int kernelSize, float sigma);
```

Applies Gaussian blur.

**Parameters:**
- `kernelSize`: Must be odd, ≥3
- `sigma`: Standard deviation, >0

### sobelEdgeDetection

```cpp
GpuImage sobelEdgeDetection(const GpuImage& input);
```

Detects edges using Sobel operator.

## Geometric

### resize

```cpp
GpuImage resize(const GpuImage& input, int width, int height);
```

Resizes image using bilinear interpolation.

### rotate

```cpp
GpuImage rotate(const GpuImage& input, float angle);
```

Rotates image by angle in degrees.

## Threshold

### threshold

```cpp
GpuImage threshold(const GpuImage& input, float value);
```

Applies binary threshold.

## Example

```cpp
ImageProcessor processor;

HostImage input = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(input);

GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
GpuImage edges = processor.sobelEdgeDetection(blurred);
GpuImage result = processor.threshold(edges, 128);

HostImage output = processor.downloadImage(result);
ImageIO::save("output.jpg", output);
```
