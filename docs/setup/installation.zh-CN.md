---
layout: default
title: 安装指南
title_en: Installation
parent: 中文文档
nav_order: 2
description: Mini-OpenCV 完整安装指南 - 系统要求、CUDA 配置、构建选项和集成方法
---

# 安装指南

从源码安装和构建 Mini-OpenCV 的完整指南。

## 系统要求

### 最低要求

| 组件 | 最低版本 | 说明 |
|------|----------|------|
| CUDA Toolkit | 11.0 | 需要 nvcc 编译器 |
| CMake | 3.18 | 基于目标的配置 |
| C++ 编译器 | C++17 | GCC 7+, Clang 7+, 或 MSVC 2019+ |
| NVIDIA 驱动 | 450.80.02+ | 用于 CUDA 11.0 |
| GPU | Compute Capability 7.5+ | Turing 架构或更新 |

### 推荐配置

| 组件 | 推荐配置 | 最佳性能 |
|------|----------|----------|
| CUDA Toolkit | 12.x | 最新稳定版本 |
| CMake | 3.24+ | 原生 GPU 架构检测 |
| GPU | RTX 30/40 系列 | Ampere/Ada Lovelace |
| 内存 | 16 GB+ | 用于大图像处理 |

## 平台特定设置

### Linux (Ubuntu/Debian)

```bash
# 安装 CUDA Toolkit
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
sudo apt-get update
sudo apt-get -y install cuda-toolkit-12-4

# 安装构建依赖
sudo apt-get install -y cmake ninja-build g++ git

# 验证安装
nvcc --version  # 应显示 CUDA 11.0+
```

### Linux (CentOS/RHEL/Fedora)

```bash
# Fedora
sudo dnf install cmake ninja-build gcc-c++ git

# CentOS/RHEL (先启用 EPEL)
sudo yum install epel-release
sudo yum install cmake3 ninja-build gcc-c++ git

# CUDA 从 NVIDIA 仓库安装
# 参考 https://developer.download.nvidia.com/compute/cuda/repos/ 选择你的发行版
```

### Windows

1. **安装 Visual Studio 2019+** 带 C++ 工作负载
2. **安装 CUDA Toolkit** 从 [NVIDIA 官网](https://developer.nvidia.com/cuda-downloads)
3. **安装 CMake** 从 [cmake.org](https://cmake.org/download/)

```powershell
# 验证安装
nvcc --version
cmake --version

# 使用 Visual Studio 生成器构建
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### macOS

macOS 上的 CUDA 支持有限。使用带 CUDA 支持的 Docker：

```bash
# 安装 CMake 和构建工具
brew install cmake ninja

# 使用 Docker 进行 CUDA 开发
docker run --gpus all -it nvidia/cuda:12.4.1-devel-ubuntu22.04
```

## 从源码构建

### 基础构建

```bash
# 克隆仓库
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build -j$(nproc)  # Linux/macOS
cmake --build build --config Release  # Windows
```

### 全选项构建

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_BENCHMARKS=ON \
    -DGPU_IMAGE_ENABLE_IO=ON \
    -DCMAKE_CUDA_ARCHITECTURES="75;80;86;89"

cmake --build build -j$(nproc)
```

### 构建选项参考

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_TESTS` | ON | 构建 Google Test v1.14.0 测试 |
| `BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BUILD_BENCHMARKS` | OFF | 构建 Google Benchmark v1.8.3 |
| `GPU_IMAGE_ENABLE_IO` | ON | 通过 stb 启用图像 I/O |
| `CMAKE_CUDA_ARCHITECTURES` | 自动 | 目标 GPU 架构 |

### GPU 架构选择

CMake 自动检测 GPU 架构（CMake 3.24+）：

```bash
# 自动检测 (CMake 3.24+)
cmake -S . -B build

# 手动指定
cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="75;80;86"

# 常见架构：
# 75 = Turing (RTX 20xx, T4)
# 80 = Ampere A100
# 86 = Ampere GeForce (RTX 30xx)
# 89 = Ada Lovelace (RTX 40xx)
# 90 = Hopper (H100)
```

## 安装

### 系统级安装

```bash
cmake --install build --prefix /usr/local

# 安装到：
#   /usr/local/lib/libgpu_image_processing.a
#   /usr/local/include/gpu_image/
#   /usr/local/lib/cmake/gpu_image_processing/
```

### 本地安装

```bash
cmake --install build --prefix $HOME/.local

# 在你的 CMake 项目中添加：
# list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")
# find_package(gpu_image_processing REQUIRED)
# target_link_libraries(your_target gpu_image::gpu_image_processing)
```

## 作为依赖使用

### 选项 1: CMake FetchContent（推荐）

```cmake
# 在你的 CMakeLists.txt 中
include(FetchContent)
FetchContent_Declare(
    gpu_image_processing
    GIT_REPOSITORY https://github.com/LessUp/mini-opencv.git
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(gpu_image_processing)

target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### 选项 2: Git 子模块

```bash
# 添加为子模块
git submodule add https://github.com/LessUp/mini-opencv.git third_party/mini-opencv
git submodule update --init

# 在你的 CMakeLists.txt 中
add_subdirectory(third_party/mini-opencv)
target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### 选项 3: 预编译库

```bash
# 下载并安装发布版本
curl -L -o mini-opencv-v2.0.0-linux.tar.gz \
    https://github.com/LessUp/mini-opencv/releases/download/v2.0.0/mini-opencv-v2.0.0-linux.tar.gz
tar xzf mini-opencv-v2.0.0-linux.tar.gz -C /opt
```

## Docker 开发

### 使用预构建镜像

```bash
# 拉取官方 CUDA 开发镜像
docker pull nvidia/cuda:12.4.1-devel-ubuntu22.04

# 使用 GPU 访问运行
docker run --gpus all -it \
    -v $(pwd):/workspace \
    nvidia/cuda:12.4.1-devel-ubuntu22.04

# 容器内
apt-get update && apt-get install -y cmake ninja-build g++ git
cd /workspace
cmake -S . -B build && cmake --build build
```

### 项目的 Dockerfile

```dockerfile
FROM nvidia/cuda:12.4.1-devel-ubuntu22.04

RUN apt-get update && apt-get install -y \
    cmake ninja-build g++ git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . /app

RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

ENTRYPOINT ["./build/bin/your_app"]
```

## 云开发

### Google Colab

```python
# 安装 CUDA 构建依赖
!apt-get update -qq
!apt-get install -y cmake ninja-build

# 克隆并构建
!git clone https://github.com/LessUp/mini-opencv.git
!cd mini-opencv && cmake -S . -B build && cmake --build build -j2

# 运行测试
!cd mini-opencv/build && ctest --output-on-failure
```

## 验证

### 测试安装

```bash
# 运行测试
ctest --test-dir build --output-on-failure

# 运行示例程序
./build/bin/basic_example
./build/bin/pipeline_example

# 检查库符号
nm -C build/lib/libgpu_image_processing.a | grep "T gpu_image::"
```

### 故障排除

| 问题 | 解决方案 |
|------|----------|
| `nvcc not found` | 添加 `/usr/local/cuda/bin` 到 PATH |
| `CUDA_ARCHITECTURES empty` | 手动设置：`-DCMAKE_CUDA_ARCHITECTURES=80` |
| `undefined reference to cuda*` | 链接 `CUDA::cudart` |
| `stb_image not found` | 启用 IO：`-DGPU_IMAGE_ENABLE_IO=ON` |

## 下一步

- [快速入门](quickstart.zh-CN.md) - 构建你的第一个程序
- [架构概览](architecture.zh-CN.md) - 了解设计
- [性能指南](performance.zh-CN.md) - 针对你的硬件优化

---

*如需额外帮助，请参阅 [FAQ](faq.zh-CN.md) 或 [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)*
