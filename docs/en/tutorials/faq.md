# FAQ

Frequently asked questions about Mini-OpenCV.

## Installation

### Q: CMake cannot find CUDA

```bash
# Set CUDA path explicitly
export CUDAToolkit_ROOT=/usr/local/cuda
cmake -S . -B build
```

### Q: nvcc not found

```bash
# Add CUDA to PATH
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
```

### Q: Wrong GPU architecture

```bash
# Specify architecture manually
cmake -S . -B build -DCUDA_ARCH="80;86;89"
```

## Usage

### Q: How do I load an image?

```cpp
HostImage image = ImageIO::load("image.jpg");
GpuImage gpu = processor.loadFromHost(image);
```

### Q: How do I save an image?

```cpp
HostImage result = processor.downloadImage(gpu);
ImageIO::save("output.jpg", result);
```

### Q: What image formats are supported?

- JPEG/JPG
- PNG
- BMP
- TGA (read only)

### Q: How do I process multiple images efficiently?

Use `PipelineProcessor` with multiple streams:

```cpp
PipelineProcessor pipeline(4);  // 4 streams
for (auto& img : images) {
    pipeline.gaussianBlur(img, 5, 1.5f, pipeline.getStream());
}
pipeline.synchronize();
```

## Performance

### Q: Why is my GPU slower than expected?

1. Check GPU temperature: `nvidia-smi`
2. Ensure power mode is set: `sudo nvidia-smi -pm 1`
3. Verify CUDA version matches driver

### Q: How much VRAM do I need?

For 4K images (3840×2160):
- Single image: ~25 MB
- Processing pipeline: ~100 MB

### Q: What's the optimal kernel size?

For Gaussian blur:
- 3×3 to 15×15: Best performance
- Larger: Use separable convolution

## Errors

### Q: "CUDA is not available"

- Install CUDA Toolkit
- Verify GPU is visible: `nvidia-smi`
- Check Compute Capability ≥ 7.5

### Q: Out of memory

- Reduce image size
- Process in batches
- Use memory pool

### Q: Tests fail

```bash
# Run with verbose output
ctest --test-dir build --output-on-failure -V
```

## Comparison

### Q: How does this compare to OpenCV?

| Feature | OpenCV | Mini-OpenCV |
|---------|--------|-------------|
| Execution | CPU | GPU |
| Speed | Baseline | 30-50× faster |
| Memory | System RAM | GPU VRAM |

### Q: When should I use Mini-OpenCV?

- Real-time processing
- Large images (4K+)
- Batch processing
- Performance-critical applications

### Q: When should I use OpenCV instead?

- CPU-only systems
- Small images
- Operations not yet implemented
- Integration with existing OpenCV code

## More Help

- [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)
