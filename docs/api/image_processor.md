---
layout: default
title: ImageProcessor
description: ImageProcessor API reference - synchronous high-level image processing interface.
---

# ImageProcessor

High-level synchronous API for image processing operations. Provides a simple interface for common tasks.

## Header

```cpp
#include "gpu_image/image_processor.hpp"
```

## Class Overview

```cpp
namespace gpu_image {

class ImageProcessor {
public:
    ImageProcessor();
    ~ImageProcessor() = default;

    // Host <-> Device transfers
    GpuImage loadFromHost(const HostImage& hostImage);
    HostImage downloadImage(const GpuImage& gpuImage);

    // Pixel operations
    GpuImage invert(const GpuImage& image);
    GpuImage toGrayscale(const GpuImage& image);
    GpuImage adjustBrightness(const GpuImage& image, int delta);

    // Convolution
    GpuImage gaussianBlur(const GpuImage& image, int kernelSize, float sigma);
    GpuImage sobelEdgeDetection(const GpuImage& image);

    // Histogram
    std::vector<int> histogram(const GpuImage& image);
    GpuImage histogramEqualize(const GpuImage& image);

    // Geometric
    GpuImage resize(const GpuImage& image, int dstWidth, int dstHeight);
    
    // Convenience methods for filters
    GpuImage medianFilter(const GpuImage& image, int kernelSize);
    GpuImage bilateralFilter(const GpuImage& image, int d, float sigmaColor, float sigmaSpace);
};

}
```

## Constructors

### ImageProcessor()

Default constructor. Initializes internal resources.

```cpp
ImageProcessor processor;  // Simple!
```

## Transfer Operations

### loadFromHost()

Upload host image to GPU memory.

```cpp
GpuImage loadFromHost(const HostImage& hostImage);
```

**Parameters:**
- `hostImage` - Source image in host memory

**Returns:** GPU image copy

**Throws:**
- `CudaException` if memory allocation fails
- `std::invalid_argument` if image is invalid

**Example:**
```cpp
HostImage host = ImageIO::load("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
```

### downloadImage()

Download GPU image to host memory.

```cpp
HostImage downloadImage(const GpuImage& gpuImage);
```

**Parameters:**
- `gpuImage` - Source image in GPU memory

**Returns:** Host image copy

**Throws:**
- `CudaException` if download fails

**Example:**
```cpp
HostImage result = processor.downloadImage(processedGpu);
ImageIO::save("output.jpg", result);
```

## Pixel Operations

### invert()

Invert all pixel values (255 - value).

```cpp
GpuImage invert(const GpuImage& image);
```

**Parameters:**
- `image` - Source image

**Returns:** Inverted image

**Example:**
```cpp
GpuImage inverted = processor.invert(original);
```

### toGrayscale()

Convert RGB/RGBA to grayscale using luminance formula.

```cpp
GpuImage toGrayscale(const GpuImage& image);
```

**Parameters:**
- `image` - Source image (RGB or RGBA)

**Returns:** Grayscale image (1 channel)

**Formula:** `gray = 0.299*R + 0.587*G + 0.114*B`

**Example:**
```cpp
GpuImage gray = processor.toGrayscale(colorImage);
```

### adjustBrightness()

Adjust brightness by adding delta to each pixel.

```cpp
GpuImage adjustBrightness(const GpuImage& image, int delta);
```

**Parameters:**
- `image` - Source image
- `delta` - Brightness adjustment (-255 to 255)

**Returns:** Brightness-adjusted image

**Example:**
```cpp
GpuImage brighter = processor.adjustBrightness(image, 50);
GpuImage darker = processor.adjustBrightness(image, -50);
```

## Convolution

### gaussianBlur()

Apply Gaussian blur filter.

```cpp
GpuImage gaussianBlur(const GpuImage& image, int kernelSize, float sigma);
```

**Parameters:**
- `image` - Source image
- `kernelSize` - Kernel size (must be odd, >= 3)
- `sigma` - Standard deviation (must be > 0)

**Returns:** Blurred image

**Throws:**
- `std::invalid_argument` if kernelSize is even or < 3
- `std::invalid_argument` if sigma <= 0

**Example:**
```cpp
GpuImage blurred = processor.gaussianBlur(image, 5, 1.5f);
```

### sobelEdgeDetection()

Detect edges using Sobel operator.

```cpp
GpuImage sobelEdgeDetection(const GpuImage& image);
```

**Parameters:**
- `image` - Source image

**Returns:** Edge magnitude image (grayscale)

**Example:**
```cpp
GpuImage edges = processor.sobelEdgeDetection(image);
```

## Histogram

### histogram()

Calculate grayscale histogram.

```cpp
std::vector<int> histogram(const GpuImage& image);
```

**Parameters:**
- `image` - Source image (automatically converted to grayscale)

**Returns:** 256-bin histogram (values 0-255)

**Example:**
```cpp
std::vector<int> hist = processor.histogram(image);
// hist[0] = count of black pixels
// hist[255] = count of white pixels
```

### histogramEqualize()

Apply histogram equalization for contrast enhancement.

```cpp
GpuImage histogramEqualize(const GpuImage& image);
```

**Parameters:**
- `image` - Source image

**Returns:** Equalized image

**Example:**
```cpp
GpuImage enhanced = processor.histogramEqualize(lowContrastImage);
```

## Geometric

### resize()

Resize image using bilinear interpolation.

```cpp
GpuImage resize(const GpuImage& image, int dstWidth, int dstHeight);
```

**Parameters:**
- `image` - Source image
- `dstWidth` - Target width
- `dstHeight` - Target height

**Returns:** Resized image

**Example:**
```cpp
GpuImage thumbnail = processor.resize(image, 256, 256);
GpuImage upscaled = processor.resize(image, 1920, 1080);
```

## Filters

### medianFilter()

Apply median filter for noise reduction.

```cpp
GpuImage medianFilter(const GpuImage& image, int kernelSize);
```

**Parameters:**
- `image` - Source image
- `kernelSize` - Kernel size (3 or 5)

**Returns:** Filtered image

**Example:**
```cpp
GpuImage denoised = processor.medianFilter(noisyImage, 3);
```

### bilateralFilter()

Apply bilateral filter (edge-preserving smoothing).

```cpp
GpuImage bilateralFilter(const GpuImage& image, int d, float sigmaColor, float sigmaSpace);
```

**Parameters:**
- `image` - Source image
- `d` - Diameter of pixel neighborhood
- `sigmaColor` - Filter sigma in color space
- `sigmaSpace` - Filter sigma in coordinate space

**Returns:** Filtered image

**Example:**
```cpp
GpuImage smooth = processor.bilateralFilter(image, 5, 50.0f, 50.0f);
```

## Complete Example

```cpp
#include "gpu_image/gpu_image_processing.hpp"
#include <iostream>

using namespace gpu_image;

int main() {
    try {
        ImageProcessor processor;
        
        // Load image
        HostImage host = ImageIO::load("photo.jpg");
        if (!host.isValid()) {
            std::cerr << "Failed to load image" << std::endl;
            return 1;
        }
        
        // Upload to GPU
        GpuImage gpu = processor.loadFromHost(host);
        
        // Process
        GpuImage grayscale = processor.toGrayscale(gpu);
        GpuImage edges = processor.sobelEdgeDetection(grayscale);
        GpuImage blurred = processor.gaussianBlur(edges, 3, 1.0f);
        
        // Download and save
        HostImage result = processor.downloadImage(blurred);
        ImageIO::save("output.jpg", result);
        
        std::cout << "Processing complete!" << std::endl;
        
    } catch (const CudaException& e) {
        std::cerr << "CUDA Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Thread Safety

`ImageProcessor` is **not thread-safe**. Create one instance per thread:

```cpp
// Good: Each thread has its own processor
void worker(int id, const HostImage& input) {
    ImageProcessor processor;
    // Process...
}
```

## Performance Notes

- Each method synchronizes with the device before returning
- For batch processing, use `PipelineProcessor`
- Memory transfers are the primary bottleneck

## See Also

- [PipelineProcessor](pipeline_processor) - For batch/async processing
- [PixelOperator](pixel_operator) - Low-level pixel operations
- [ConvolutionEngine](convolution_engine) - Low-level convolution
