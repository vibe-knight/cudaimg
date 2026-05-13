# Examples

Code examples for common use cases.

## Basic Operations

### Gaussian Blur

```cpp
#include "gpu_image/gpu_image_processing.hpp"
using namespace gpu_image;

int main() {
    ImageProcessor processor;
    
    HostImage input = ImageIO::load("input.jpg");
    GpuImage gpu = processor.loadFromHost(input);
    
    // Apply Gaussian blur with 5x5 kernel, sigma=1.5
    GpuImage blurred = processor.gaussianBlur(gpu, 5, 1.5f);
    
    HostImage output = processor.downloadImage(blurred);
    ImageIO::save("blurred.jpg", output);
}
```

### Edge Detection

```cpp
// Sobel edge detection
GpuImage edges = processor.sobelEdgeDetection(gpu);

// Convert to grayscale first for better results
GpuImage gray = processor.grayscale(gpu);
GpuImage edges = processor.sobelEdgeDetection(gray);
```

### Resize

```cpp
// Resize to specific dimensions
GpuImage resized = processor.resize(gpu, 1920, 1080);

// Scale by factor
int newWidth = gpu.width() * 2;
int newHeight = gpu.height() * 2;
GpuImage doubled = processor.resize(gpu, newWidth, newHeight);
```

## Image Processing Pipeline

```cpp
// Multi-step processing
GpuImage gray = processor.grayscale(gpu);
GpuImage blurred = processor.gaussianBlur(gray, 5, 1.0f);
GpuImage edges = processor.sobelEdgeDetection(blurred);
GpuImage thresholded = processor.threshold(edges, 128);
```

## Batch Processing

```cpp
// Process multiple images
std::vector<std::string> files = {"img1.jpg", "img2.jpg", "img3.jpg"};

for (const auto& file : files) {
    HostImage input = ImageIO::load(file);
    GpuImage gpu = processor.loadFromHost(input);
    GpuImage processed = processor.gaussianBlur(gpu, 5, 1.5f);
    HostImage output = processor.downloadImage(processed);
    
    std::string outFile = "processed_" + file;
    ImageIO::save(outFile, output);
}
```

## Async Processing

```cpp
// Using PipelineProcessor for async operations
PipelineProcessor pipeline(4);  // 4 CUDA streams

std::vector<GpuImage> images;
for (auto& img : images) {
    auto stream = pipeline.getStream();
    pipeline.gaussianBlur(img, 5, 1.5f, stream);
}

pipeline.synchronize();
```

## Morphological Operations

```cpp
// Erosion and dilation
GpuImage eroded = processor.erode(gpu, 5);  // 5x5 kernel
GpuImage dilated = processor.dilate(gpu, 5);

// Opening (erosion followed by dilation)
GpuImage opened = processor.morphologyOpen(gpu, 5);

// Closing (dilation followed by erosion)
GpuImage closed = processor.morphologyClose(gpu, 5);
```

## Color Space Conversion

```cpp
// RGB to HSV
GpuImage hsv = processor.rgbToHsv(gpu);

// RGB to YUV
GpuImage yuv = processor.rgbToYuv(gpu);
```

## Histogram Operations

```cpp
// Calculate histogram
std::vector<int> hist = processor.histogram(gpu);

// Histogram equalization
GpuImage equalized = processor.histogramEqualization(gpu);
```

## More Examples

See the `examples/` directory in the repository for complete programs:

- `basic_example.cpp` - Basic operations
- `pipeline_example.cpp` - Multi-step processing
- `batch_example.cpp` - Batch processing
- `benchmark_example.cpp` - Performance testing
