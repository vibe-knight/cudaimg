# 配置说明

Mini-OpenCV 的 CMake 配置选项。

## 构建类型

| 类型 | 描述 |
|------|------|
| `Release` | 优化构建（默认） |
| `Debug` | 调试符号，无优化 |
| `RelWithDebInfo` | 带调试信息的发布版 |

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

## CMake 选项

### 构建选项

| 选项 | 默认值 | 描述 |
|------|--------|------|
| `BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BUILD_TESTS` | ON | 构建 GoogleTest 套件 |
| `BUILD_BENCHMARKS` | OFF | 构建性能基准测试 |
| `BUILD_SHARED_LIBS` | OFF | 构建为动态库 |

### CUDA 选项

| 选项 | 默认值 | 描述 |
|------|--------|------|
| `CUDA_ARCH` | auto | GPU 架构（如 "80;86;89"） |
| `CUDA_FAST_MATH` | ON | 快速数学模式 |
| `CUDA_VERBOSE_PTXAS` | OFF | 详细 PTX 汇编输出 |

### 高级选项

| 选项 | 默认值 | 描述 |
|------|--------|------|
| `ENABLE_CUDA_ERROR_CHECK` | ON | 运行时错误检查 |
| `ENABLE_MEMORY_TRACKING` | OFF | 跟踪 GPU 内存使用 |

## GPU 架构

### 自动检测

默认情况下，CMake 检测已安装的 GPU：

```bash
cmake -S . -B build  # 自动检测
```

### 手动指定

```bash
# 单一架构
cmake -S . -B build -DCUDA_ARCH=89

# 多架构
cmake -S . -B build -DCUDA_ARCH="75;80;86;89"
```

### 架构参考

| 架构 | 计算能力 | GPU |
|------|---------|-----|
| Turing | 75 | RTX 20xx, T4 |
| Ampere | 80 | A100 |
| Ampere | 86 | RTX 30xx |
| Ada | 89 | RTX 40xx, L4 |
| Hopper | 90 | H100 |

## 示例配置

### 开发环境

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON \
    -DENABLE_CUDA_ERROR_CHECK=ON
```

### 生产环境

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_TESTS=OFF \
    -DENABLE_CUDA_ERROR_CHECK=OFF
```

### 性能测试

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_BENCHMARKS=ON \
    -DCUDA_ARCH=89
```

## 下一步

- [快速入门](./quickstart) - 基本用法
- [示例代码](../tutorials/examples) - 代码示例