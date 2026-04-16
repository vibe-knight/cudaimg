---
layout: default
title: Installation
nav_order: 3
parent: Documentation
description: Complete installation guide for Mini-OpenCV - system requirements, CUDA setup, build options, and integration
---

# Installation Guide

Complete guide for installing and building Mini-OpenCV from source.

## System Requirements

### Minimum Requirements

| Component | Minimum Version | Notes |
|-----------|-----------------|-------|
| CUDA Toolkit | 11.0 | nvcc compiler required |
| CMake | 3.18 | Target-based configuration |
| C++ Compiler | C++17 | GCC 7+, Clang 7+, or MSVC 2019+ |
| NVIDIA Driver | 450.80.02+ | For CUDA 11.0 |
| GPU | Compute Capability 7.5+ | Turing architecture or newer |

### Recommended Configuration

| Component | Recommended | For Best Performance |
|-----------|-------------|---------------------|
| CUDA Toolkit | 12.x | Latest stable version |
| CMake | 3.24+ | Native GPU arch detection |
| GPU | RTX 30/40 series | Ampere/Ada Lovelace |
| RAM | 16 GB+ | For large image processing |

## Platform-Specific Setup

### Linux (Ubuntu/Debian)

```bash
# Install CUDA Toolkit
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
sudo apt-get update
sudo apt-get -y install cuda-toolkit-12-4

# Install build dependencies
sudo apt-get install -y cmake ninja-build g++ git

# Verify installation
nvcc --version  # Should show CUDA 11.0+
```

### Linux (CentOS/RHEL/Fedora)

```bash
# Fedora
sudo dnf install cmake ninja-build gcc-c++ git

# CentOS/RHEL (enable EPEL first)
sudo yum install epel-release
sudo yum install cmake3 ninja-build gcc-c++ git

# CUDA from NVIDIA repository
# Follow https://developer.nvidia.com/cuda-downloads for your distro
```

### Windows

1. **Install Visual Studio 2019+** with C++ workloads
2. **Install CUDA Toolkit** from [NVIDIA website](https://developer.nvidia.com/cuda-downloads)
3. **Install CMake** from [cmake.org](https://cmake.org/download/)

```powershell
# Verify installation
nvcc --version
cmake --version

# Build using Visual Studio generator
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

### macOS

CUDA support on macOS is limited. Use Docker with CUDA support:

```bash
# Install CMake and build tools
brew install cmake ninja

# Use Docker for CUDA development
docker run --gpus all -it nvidia/cuda:12.4.1-devel-ubuntu22.04
```

## Building from Source

### Basic Build

```bash
# Clone repository
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Configure
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)  # Linux/macOS
cmake --build build --config Release  # Windows
```

### Build with All Options

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

### Build Options Reference

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build Google Test v1.14.0 tests |
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_BENCHMARKS` | OFF | Build Google Benchmark v1.8.3 |
| `GPU_IMAGE_ENABLE_IO` | ON | Enable image I/O via stb |
| `CMAKE_CUDA_ARCHITECTURES` | Auto | Target GPU architectures |

### GPU Architecture Selection

CMake automatically detects your GPU architecture (CMake 3.24+):

```bash
# Auto-detection (CMake 3.24+)
cmake -S . -B build

# Manual specification
cmake -S . -B build -DCMAKE_CUDA_ARCHITECTURES="75;80;86"

# Common architectures:
# 75 = Turing (RTX 20xx, T4)
# 80 = Ampere A100
# 86 = Ampere GeForce (RTX 30xx)
# 89 = Ada Lovelace (RTX 40xx)
# 90 = Hopper (H100)
```

## Installation

### System-wide Installation

```bash
cmake --install build --prefix /usr/local

# Installs to:
#   /usr/local/lib/libgpu_image_processing.a
#   /usr/local/include/gpu_image/
#   /usr/local/lib/cmake/gpu_image_processing/
```

### Local Installation

```bash
cmake --install build --prefix $HOME/.local

# Add to your CMake project:
# list(APPEND CMAKE_PREFIX_PATH "$ENV{HOME}/.local")
# find_package(gpu_image_processing REQUIRED)
# target_link_libraries(your_target gpu_image::gpu_image_processing)
```

## Using as a Dependency

### Option 1: CMake FetchContent (Recommended)

```cmake
# In your CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
    gpu_image_processing
    GIT_REPOSITORY https://github.com/LessUp/mini-opencv.git
    GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(gpu_image_processing)

target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### Option 2: Git Submodule

```bash
# Add as submodule
git submodule add https://github.com/LessUp/mini-opencv.git third_party/mini-opencv
git submodule update --init

# In your CMakeLists.txt
add_subdirectory(third_party/mini-opencv)
target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### Option 3: Pre-built Library

```bash
# Download and install release
curl -L -o mini-opencv-v2.0.0-linux.tar.gz \
    https://github.com/LessUp/mini-opencv/releases/download/v2.0.0/mini-opencv-v2.0.0-linux.tar.gz
tar xzf mini-opencv-v2.0.0-linux.tar.gz -C /opt
```

## Docker Development

### Using Pre-built Image

```bash
# Pull official CUDA development image
docker pull nvidia/cuda:12.4.1-devel-ubuntu22.04

# Run with GPU access
docker run --gpus all -it \
    -v $(pwd):/workspace \
    nvidia/cuda:12.4.1-devel-ubuntu22.04

# Inside container
apt-get update && apt-get install -y cmake ninja-build g++ git
cd /workspace
cmake -S . -B build && cmake --build build
```

### Dockerfile for Your Project

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

## Cloud Development

### Google Colab

```python
# Install CUDA build dependencies
!apt-get update -qq
!apt-get install -y cmake ninja-build

# Clone and build
!git clone https://github.com/LessUp/mini-opencv.git
!cd mini-opencv && cmake -S . -B build && cmake --build build -j2

# Run tests
!cd mini-opencv/build && ctest --output-on-failure
```

## Verification

### Test Installation

```bash
# Run tests
ctest --test-dir build --output-on-failure

# Run example programs
./build/bin/basic_example
./build/bin/pipeline_example

# Check library symbols
nm -C build/lib/libgpu_image_processing.a | grep "T gpu_image::"
```

### Troubleshooting

| Issue | Solution |
|-------|----------|
| `nvcc not found` | Add `/usr/local/cuda/bin` to PATH |
| `CUDA_ARCHITECTURES empty` | Set manually: `-DCMAKE_CUDA_ARCHITECTURES=80` |
| `undefined reference to cuda*` | Link with `CUDA::cudart` |
| `stb_image not found` | Enable IO: `-DGPU_IMAGE_ENABLE_IO=ON` |

## Next Steps

- [Quick Start](quickstart.md) - Build your first program
- [Architecture](architecture.md) - Understand the design
- [Performance Guide](performance.md) - Optimize for your hardware

---

*For additional help, see [FAQ](faq.md) or [GitHub Issues](https://github.com/LessUp/mini-opencv/issues)*
