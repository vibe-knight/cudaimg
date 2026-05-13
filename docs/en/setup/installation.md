# Installation

Complete installation guide for Mini-OpenCV.

## System Requirements

| Component | Minimum | Recommended |
|-----------|---------|-------------|
| CUDA | 11.0 | 12.x |
| CMake | 3.18 | 3.24+ |
| C++ | C++17 | C++17 |
| GPU | CC 7.5+ | RTX 30/40 series |

## Install CUDA

### Ubuntu

```bash
# Add NVIDIA package repository
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
sudo apt update

# Install CUDA
sudo apt install cuda-toolkit-12-4

# Add to PATH
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc
```

### Verify Installation

```bash
nvcc --version
nvidia-smi
```

## Build Mini-OpenCV

```bash
# Clone
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Configure with options
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_BENCHMARKS=ON \
    -DBUILD_TESTS=ON

# Build
cmake --build build -j$(nproc)
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_TESTS` | ON | Build test suite |
| `BUILD_BENCHMARKS` | OFF | Build benchmarks |
| `CUDA_ARCH` | auto | GPU architecture(s) |

## Verify Build

```bash
# Run tests
ctest --test-dir build --output-on-failure

# Run example
./build/bin/basic_example
```

## Troubleshooting

### CUDA Not Found

```bash
export CUDAToolkit_ROOT=/usr/local/cuda
cmake -S . -B build
```

### Wrong GPU Architecture

```bash
# Specify architecture explicitly
cmake -S . -B build -DCUDA_ARCH="80;86;89"
```

## Next Steps

- [Quick Start](./quickstart) - Basic usage
- [Configuration](./configuration) - Advanced options
