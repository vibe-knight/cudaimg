# 测试方法

我们如何测量和报告性能。

## 测试环境

### 硬件

| 组件 | 规格 |
|------|------|
| GPU | NVIDIA RTX 4090 (24GB 显存) |
| CPU | Intel i9-13900K (24 核) |
| RAM | 64GB DDR5-6000 |
| 存储 | NVMe SSD |

### 软件

| 组件 | 版本 |
|------|------|
| 操作系统 | Ubuntu 22.04 LTS |
| CUDA | 12.4 |
| 驱动 | 550.x |
| OpenCV | 4.8.0 |
| GCC | 11.4 |

## 测量方法

### 预热

每次测量前：
1. 运行操作 10 次预热 GPU
2. 用 `cudaDeviceSynchronize()` 清除 GPU 缓存

### 计时

```cpp
auto start = std::chrono::high_resolution_clock::now();

// 运行操作 N 次
for (int i = 0; i < iterations; i++) {
    operation();
    cudaDeviceSynchronize();
}

auto end = std::chrono::high_resolution_clock::now();
auto avg_time = (end - start) / iterations;
```

### 指标

- **延迟**: 每次操作的平均时间
- **吞吐量**: 每秒操作数
- **加速比**: CPU 时间 / GPU 时间

## 图像尺寸

| 名称 | 分辨率 | 像素数 |
|------|--------|--------|
| HD | 1280×720 | 921K |
| FHD | 1920×1080 | 2.1M |
| 4K | 3840×2160 | 8.3M |
| 8K | 7680×4320 | 33.2M |

## 可复现性

所有基准测试位于 `benchmarks/` 目录：

```bash
mkdir build && cd build
cmake -DBUILD_BENCHMARKS=ON ..
make -j$(nproc)
./bin/benchmark_convolution
```

## 注意事项

- CPU 基准测试使用单线程 OpenCV
- GPU 基准测试包含主机-设备传输时间
- 结果可能因 GPU 温度和时钟速度而异