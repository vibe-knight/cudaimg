# Benchmark Methodology

How we measure and report performance.

## Test Environment

### Hardware

| Component | Specification |
|-----------|--------------|
| GPU | NVIDIA RTX 4090 (24GB VRAM) |
| CPU | Intel i9-13900K (24 cores) |
| RAM | 64GB DDR5-6000 |
| Storage | NVMe SSD |

### Software

| Component | Version |
|-----------|---------|
| OS | Ubuntu 22.04 LTS |
| CUDA | 12.4 |
| Driver | 550.x |
| OpenCV | 4.8.0 |
| GCC | 11.4 |

## Measurement Methodology

### Warm-up

Before each measurement:
1. Run operation 10 times to warm up GPU
2. Clear GPU cache with `cudaDeviceSynchronize()`

### Timing

```cpp
auto start = std::chrono::high_resolution_clock::now();

// Run operation N times
for (int i = 0; i < iterations; i++) {
    operation();
    cudaDeviceSynchronize();
}

auto end = std::chrono::high_resolution_clock::now();
auto avg_time = (end - start) / iterations;
```

### Metrics

- **Latency**: Average time per operation
- **Throughput**: Operations per second
- **Speedup**: CPU time / GPU time

## Image Sizes

| Name | Dimensions | Pixels |
|------|------------|--------|
| HD | 1280×720 | 921K |
| FHD | 1920×1080 | 2.1M |
| 4K | 3840×2160 | 8.3M |
| 8K | 7680×4320 | 33.2M |

## Reproducibility

All benchmarks are available in the `benchmarks/` directory:

```bash
mkdir build && cd build
cmake -DBUILD_BENCHMARKS=ON ..
make -j$(nproc)
./bin/benchmark_convolution
```

## Notes

- CPU benchmarks use single-threaded OpenCV
- GPU benchmarks include host-device transfer time
- Results may vary based on GPU temperature and clock speed
