# Configuration

CMake configuration options for Mini-OpenCV.

## Build Types

| Type | Description |
|------|-------------|
| `Release` | Optimized build (default) |
| `Debug` | Debug symbols, no optimization |
| `RelWithDebInfo` | Release with debug info |

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

## CMake Options

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_TESTS` | ON | Build GoogleTest suite |
| `BUILD_BENCHMARKS` | OFF | Build performance benchmarks |
| `BUILD_SHARED_LIBS` | OFF | Build as shared library |

### CUDA Options

| Option | Default | Description |
|--------|---------|-------------|
| `CUDA_ARCH` | auto | GPU architectures (e.g., "80;86;89") |
| `CUDA_FAST_MATH` | ON | Fast math mode |
| `CUDA_VERBOSE_PTXAS` | OFF | Verbose PTX assembly |

### Advanced Options

| Option | Default | Description |
|--------|---------|-------------|
| `ENABLE_CUDA_ERROR_CHECK` | ON | Runtime error checking |
| `ENABLE_MEMORY_TRACKING` | OFF | Track GPU memory usage |

## GPU Architecture

### Auto Detection

By default, CMake detects installed GPU:

```bash
cmake -S . -B build  # Auto-detect
```

### Manual Specification

```bash
# Single architecture
cmake -S . -B build -DCUDA_ARCH=89

# Multiple architectures
cmake -S . -B build -DCUDA_ARCH="75;80;86;89"
```

### Architecture Reference

| Architecture | Compute | GPUs |
|--------------|---------|------|
| Turing | 75 | RTX 20xx, T4 |
| Ampere | 80 | A100 |
| Ampere | 86 | RTX 30xx |
| Ada | 89 | RTX 40xx, L4 |
| Hopper | 90 | H100 |

## Example Configurations

### Development

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_EXAMPLES=ON \
    -DBUILD_TESTS=ON \
    -DENABLE_CUDA_ERROR_CHECK=ON
```

### Production

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_TESTS=OFF \
    -DENABLE_CUDA_ERROR_CHECK=OFF
```

### Benchmarking

```bash
cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_BENCHMARKS=ON \
    -DCUDA_ARCH=89
```

## Next Steps

- [Quick Start](./quickstart) - Basic usage
- [Examples](../tutorials/examples) - Code examples
