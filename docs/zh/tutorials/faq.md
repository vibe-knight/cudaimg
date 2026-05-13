# 常见问题

Mini-OpenCV 常见问题解答。

## 安装

### Q: CMake 找不到 CUDA

```bash
# 显式设置 CUDA 路径
export CUDAToolkit_ROOT=/usr/local/cuda
cmake -S . -B build
```

### Q: 找不到 nvcc

```bash
# 添加 CUDA 到 PATH
export PATH=/usr/local/cuda/bin:$PATH
export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH
```

### Q: GPU 架构错误

```bash
# 手动指定架构
cmake -S . -B build -DCUDA_ARCH="80;86;89"
```

## 使用

### Q: 如何加载图像？

```cpp
HostImage image = ImageIO::load("image.jpg");
GpuImage gpu = processor.loadFromHost(image);
```

### Q: 如何保存图像？

```cpp
HostImage result = processor.downloadImage(gpu);
ImageIO::save("output.jpg", result);
```

### Q: 支持哪些图像格式？

- JPEG/JPG
- PNG
- BMP
- TGA（仅读取）

### Q: 如何高效处理多张图像？

使用 `PipelineProcessor` 配合多流：

```cpp
PipelineProcessor pipeline(4);  // 4 个流
for (auto& img : images) {
    pipeline.gaussianBlur(img, 5, 1.5f, pipeline.getStream());
}
pipeline.synchronize();
```

## 性能

### Q: 为什么 GPU 比预期慢？

1. 检查 GPU 温度：`nvidia-smi`
2. 确保电源模式已设置：`sudo nvidia-smi -pm 1`
3. 验证 CUDA 版本与驱动匹配

### Q: 需要多少显存？

对于 4K 图像 (3840×2160)：
- 单张图像：~25 MB
- 处理流水线：~100 MB

### Q: 最优内核大小是多少？

对于高斯模糊：
- 3×3 到 15×15：最佳性能
- 更大：使用可分离卷积

## 错误

### Q: "CUDA 不可用"

- 安装 CUDA Toolkit
- 验证 GPU 可见：`nvidia-smi`
- 检查计算能力 ≥ 7.5

### Q: 内存不足

- 减小图像尺寸
- 批量处理
- 使用内存池

### Q: 测试失败

```bash
# 详细输出运行
ctest --test-dir build --output-on-failure -V
```

## 对比

### Q: 与 OpenCV 相比如何？

| 特性 | OpenCV | Mini-OpenCV |
|------|--------|-------------|
| 执行位置 | CPU | GPU |
| 速度 | 基准 | 快 30-50 倍 |
| 内存 | 系统内存 | GPU 显存 |

### Q: 何时使用 Mini-OpenCV？

- 实时处理
- 大图像（4K+）
- 批量处理
- 性能关键应用

### Q: 何时使用 OpenCV？

- 仅 CPU 系统
- 小图像
- 尚未实现的操作
- 与现有 OpenCV 代码集成

## 更多帮助

- [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)
- [GitHub Discussions](https://github.com/LessUp/mini-opencv/discussions)