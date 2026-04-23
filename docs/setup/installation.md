---
layout: default
title: Installation
nav_order: 2
parent: Documentation
description: Complete installation guide for GPU Image Processing library
---

# Installation

Complete installation and configuration guide.

## Requirements

| Component | Minimum | Recommended |
|:----------|:--------|:------------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ Standard | C++17 | C++17 |
| GPU | Compute 7.5+ | RTX 30/40 series |
| Compiler | GCC 7+, Clang 7+, MSVC 2019+ | GCC 11+, Clang 14+ |

## Ubuntu/Debian

```bash
# Install dependencies
sudo apt update
sudo apt install build-essential cmake git

# Verify CUDA (should be pre-installed for GPU instances)
nvcc --version

# Clone and build
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Install (optional)
sudo cmake --install build
```

## CentOS/RHEL/Fedora

```bash
# Install dependencies
sudo yum install gcc-c++ cmake git
# or: sudo dnf install gcc-c++ cmake git

# Build
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

## macOS (with eGPU)

```bash
# Install dependencies
brew install cmake

# Build (CUDA not available on macOS without eGPU)
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build
cmake --build build -j$(sysctl -n hw.ncpu)
```

Note: CUDA is not natively supported on macOS. Use Docker or remote GPU.

## Windows

```powershell
# Using Visual Studio 2022
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --parallel
```

## CMake Integration

### FetchContent (Recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    gpu_image_processing
    GIT_REPOSITORY https://github.com/LessUp/mini-opencv.git
    GIT_TAG main
)
FetchContent_MakeAvailable(gpu_image_processing)

target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### Installed Package

```cmake
find_package(gpu_image_processing REQUIRED)
target_link_libraries(your_target gpu_image::gpu_image_processing)
```

### Subdirectory

```cmake
add_subdirectory(third_party/mini-opencv)
target_link_libraries(your_target gpu_image::gpu_image_processing)
```

## Build Options

| Option | Default | Description |
|:-------|:--------|:------------|
| `BUILD_TESTS` | OFF | Build test suite |
| `BUILD_EXAMPLES` | OFF | Build example programs |
| `BUILD_BENCHMARKS` | OFF | Build benchmark suite |
| `CMAKE_BUILD_TYPE` | Release | Debug/Release/RelWithDebInfo |
| `CMAKE_CUDA_ARCHITECTURES` | Auto | GPU architecture (e.g., "75;80;86") |

## Troubleshooting

### CUDA Not Found

```bash
# Set CUDA path
export CUDAToolkit_ROOT=/usr/local/cuda
export PATH=$CUDAToolkit_ROOT/bin:$PATH
cmake -S . -B build -DCUDAToolkit_ROOT=$CUDAToolkit_ROOT
```

### CMake Version Too Old

```bash
# Install newer CMake
pip install cmake>=3.18
# or use conda
conda install cmake>=3.18
```

### No CUDA-Capable Device

Verify GPU:
```bash
nvidia-smi
```

If no GPU available, tests requiring CUDA will be skipped automatically.

---

## Next Steps

- [Quick Start]({{ site.baseurl }}/setup/quickstart) - Build and test
- [Architecture]({{ site.baseurl }}/architecture/architecture) - Learn the design
- [API Reference]({{ site.baseurl }}/api/) - Start coding
