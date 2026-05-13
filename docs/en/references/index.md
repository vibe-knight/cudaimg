# References & Citations

Academic papers and related projects that influenced Mini-OpenCV's design.

## Foundational Papers

### CUDA Programming

::: cite-card
**CUDA C++ Programming Guide**
*NVIDIA Documentation, 2023*

The official guide for CUDA programming model, memory hierarchy, and optimization techniques. Essential reading for GPU programming.

[PDF](https://docs.nvidia.com/cuda/pdf/CUDA_C_Programming_Guide.pdf)
:::

### Parallel Reduction

::: cite-card
**Optimizing Parallel Reduction in CUDA**
*Mark Harris, NVIDIA Developer, 2007*

Classic paper on parallel reduction patterns using shared memory and warp shuffle operations. Foundation for our histogram implementation.

[Link](https://developer.nvidia.com/content/optimizing-parallel-reduction-cuda)
:::

### Convolution Optimization

::: cite-card
**Efficient CUDA Convolution via Shared Memory**
*Victor Podlozhnyuk, NVIDIA, 2007*

Image convolution optimization using shared memory tiling for cache efficiency. Basis for our convolution kernel design.

[PDF](https://developer.download.nvidia.com/compute/cuda/1.1-Beta/x86_website/Data/Articles/convolutionSeparable.pdf)
:::

### Image Processing

::: cite-card
**Digital Image Processing**
*Rafael C. Gonzalez, Richard E. Woods*

Comprehensive textbook covering image processing fundamentals: transforms, filtering, morphology, and segmentation.

[Amazon](https://www.amazon.com/Digital-Image-Processing-Rafael-Gonzalez/dp/0133356728)
:::

## Related Projects

| Project | Focus | Language | License |
|---------|-------|----------|---------|
| [OpenCV](https://opencv.org/) | CPU image processing | C++ | Apache 2.0 |
| [CUTLASS](https://github.com/NVIDIA/cutlass) | CUDA template library | C++ | BSD-3 |
| [NPP](https://docs.nvidia.com/cuda/npp/) | NVIDIA Primitives | C | Proprietary |
| [VPI](https://developer.nvidia.com/nvidia-vpi) | Vision Programming Interface | C++ | Proprietary |
| [cv::cuda](https://docs.opencv.org/4.x/d2/d3c/group__cudaimgproc.html) | OpenCV CUDA module | C++ | Apache 2.0 |

## Design Inspirations

### Memory Management

- **RAII Pattern**: Automatic GPU memory management inspired by modern C++ practices
- **Memory Pools**: Reusable buffers to reduce allocation overhead

### Kernel Design

- **Tiled Convolution**: Shared memory optimization from NVIDIA CUDA samples
- **Atomic Histogram**: Parallel reduction using atomic operations

### API Design

- **Fluent Interface**: Method chaining for clean code
- **Type Safety**: Strong typing for image formats and dimensions

## How to Cite

If you use Mini-OpenCV in your research or project, please cite:

```bibtex
@software{mini_opencv_2024,
  title = {Mini-OpenCV: High-Performance CUDA Image Processing Library},
  author = {LessUp},
  year = {2024},
  version = {2.1.0},
  url = {https://github.com/LessUp/mini-opencv},
  note = {CUDA-accelerated operators achieving 30-50× speedup over CPU implementations}
}
```

## Contributing References

Found a relevant paper or project? Please [open an issue](https://github.com/LessUp/mini-opencv/issues) or submit a pull request!
