<!-- From: /home/shane/dev/mini-opencv/AGENTS.md -->
# AGENTS Guide

> This guide is for coding agents (AI assistants) working in `mini-opencv`.
> It captures the repository's build, test, formatting, and code-style conventions.
> Language: The project's source code comments and some documentation are in Chinese and English (bilingual).

---

## Project Overview

**Mini-OpenCV** is a CUDA-based high-performance image processing library providing GPU-accelerated operators for computer vision applications. It delivers 30-50x faster performance than CPU OpenCV for comparable operations.

### Architecture (分层架构)

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Layer                         │
│         ImageProcessor  ·  PipelineProcessor                 │
├─────────────────────────────────────────────────────────────┤
│              Operator Layer (CUDA Kernels)                   │
│  PixelOperator  │  ConvolutionEngine  │  Geometric          │
│  Morphology     │  ColorSpace         │  Filters            │
│  Threshold      │  HistogramCalculator│  ImageResizer       │
├─────────────────────────────────────────────────────────────┤
│                  Infrastructure Layer                        │
│  DeviceBuffer  ·  GpuImage/HostImage  ·  CudaError          │
│  ImageIO       ·  StreamManager                              │
└─────────────────────────────────────────────────────────────┘
```

### Technology Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++17, CUDA 14 |
| **Build System** | CMake 3.18+ |
| **CUDA** | 11.0+ (Recommended: 12.x) |
| **Testing** | Google Test v1.14.0 |
| **Benchmarking** | Google Benchmark |
| **Image I/O** | stb (fetched via CMake) |

---

## Project Philosophy: Spec-Driven Development (SDD)

This project strictly follows **Spec-Driven Development (SDD)** paradigm. All code implementations must use the specification documents in the `/specs` directory as the Single Source of Truth.

### Directory Context

| Directory | Purpose |
|-----------|---------|
| `/specs/product/` | Product feature definitions and acceptance criteria (PRD) |
| `/specs/rfc/` | Technical design documents and architecture proposals |
| `/specs/api/` | API interface definitions (function signatures, contracts) |
| `/specs/db/` | Data model and schema definitions |
| `/specs/testing/` | Test specifications and BDD feature files |
| `/docs/` | User documentation, tutorials, and setup guides (bilingual) |

### AI Agent Workflow Instructions

When developing a new feature, modifying existing functionality, or fixing a bug, **you must strictly follow this workflow without skipping any steps**:

#### Step 1: Review Specs

- Before writing any code, first read the relevant product docs, RFCs, and API definitions in the `/specs` directory.
- If the user's request conflicts with existing specs, **stop coding immediately** and point out the conflict. Ask the user whether to update the specs first.

#### Step 2: Spec-First Update

- If this is a new feature, or if existing interfaces/database structures need to change, **you must first propose updating or creating the corresponding spec documents** (e.g., RFCs, API definitions).
- Wait for user confirmation of spec changes before entering the code implementation phase.

#### Step 3: Implementation

- When writing code, **100% comply with spec definitions** (including variable names, API paths, data types, status codes, etc.).
- Do not add features not defined in specs (No Gold-Plating).

#### Step 4: Test against Specs

- Write unit tests and integration tests based on the acceptance criteria in `/specs`.
- Ensure test cases cover all boundary conditions described in the specs.

---

## Code Generation Rules

- Any externally exposed API changes must synchronously update `/specs/api/` definitions.
- If uncertain about technical details, consult `/specs/rfc/` for architecture conventions. Do not invent design patterns.

---

## Project Structure

```
mini-opencv/
├── include/gpu_image/        # Public headers
│   ├── core/                 # Core infrastructure (DeviceBuffer, GpuImage, CudaError)
│   ├── operators/            # CUDA operator kernels (PixelOperator, etc.)
│   ├── processing/           # High-level APIs (ImageProcessor, PipelineProcessor)
│   ├── io/                   # Image I/O (ImageIO)
│   └── utils/                # Utilities (StreamManager)
├── src/                      # Implementations
│   ├── core/                 # Core implementation
│   ├── operators/            # Operator kernels (.cu files)
│   ├── processing/           # High-level API implementation
│   ├── io/                   # Image I/O implementation
│   └── utils/                # Utility implementations
├── tests/                    # Test files
│   ├── core/                 # Core tests
│   ├── operators/            # Operator tests
│   └── processing/           # Processing tests
├── examples/                 # Example programs
├── benchmarks/               # Benchmark programs
├── specs/                    # Specifications (SDD)
├── docs/                     # Documentation (bilingual)
└── .github/workflows/        # CI/CD configuration
```

**Main library target**: `gpu_image_processing` (alias: `gpu_image::gpu_image_processing`)

---

## Build Commands

Use an out-of-tree build with CMake.

### Prerequisites

- CUDA Toolkit 11.0+ with `nvcc` on PATH or `CUDAToolkit_ROOT` set
- CMake 3.18+
- C++17 compatible compiler (GCC, Clang, or MSVC)
- NVIDIA GPU with Compute Capability 7.5+ (Turing or newer)

### Configure and Build

```bash
# Configure (Release mode)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Configure with all options
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DBUILD_BENCHMARKS=OFF

# Build all
cmake --build build -j$(nproc)

# Build specific targets
cmake --build build --target gpu_image_processing -j$(nproc)
cmake --build build --target gpu_image_tests -j$(nproc)
cmake --build build --target basic_example pipeline_example -j$(nproc)

# Enable benchmarks
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
cmake --build build --target gpu_image_benchmark -j$(nproc)

# Install
cmake --install build
```

---

## Test Commands

Tests are built into one executable: `build/bin/gpu_image_tests`.

There is **not** one test binary per source file. `gtest_discover_tests()` is used, so CTest exposes individual GTest cases like `FiltersTest.MedianFilter`.

```bash
# Full suite
ctest --test-dir build --output-on-failure

# List discovered tests
ctest --test-dir build -N

# Run one exact test through CTest
ctest --test-dir build -R '^FiltersTest\.MedianFilter$' --output-on-failure

# Run one fixture through CTest
ctest --test-dir build -R '^FiltersTest\.' --output-on-failure

# Run the binary directly
./build/bin/gpu_image_tests

# Run one exact GTest case directly
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.MedianFilter

# Run one whole fixture directly
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.*
```

If you need to target "one test file," use that file's fixture name, e.g., `FiltersTest.*` or `DeviceBufferTest.*`.

### Testing Conventions

- Tests use GoogleTest fixtures with `TEST_F(...)`
- Most CUDA-dependent fixtures check `cudaGetDeviceCount()` in `SetUp()` and call `GTEST_SKIP()` when CUDA is unavailable
- Follow that pattern for new CUDA-dependent tests
- Prefer deterministic inputs and explicit expected values
- When validating image operations, assert image validity, dimensions, channels, and representative pixel values
- Use `EXPECT_THROW` for input validation behavior
- Add or update regression tests alongside bug fixes

---

## Lint and Formatting

There is no separate static-lint target today. The CI "format" job is the closest equivalent.

CI uses `clang-format-14 --dry-run --Werror` on `*.h`, `*.hpp`, `*.cpp`, `*.cu`, and `*.cuh`, excluding `build/` and `third_party/`.

```bash
# CI-equivalent format check
find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' \) \
  -not -path './build/*' -not -path './third_party/*' -print0 | \
  xargs -0 -r clang-format-14 --dry-run --Werror

# In-place formatting
find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' \) \
  -not -path './build/*' -not -path './third_party/*' -print0 | \
  xargs -0 -r clang-format-14 -i
```

If only `clang-format` is installed locally, use it, but keep output identical to CI.

---

## Style Sources of Truth

| File | Authority |
|------|-----------|
| `.clang-format` | C++ and CUDA layout (LLVM style, 2-space indent, 80-column limit) |
| `.editorconfig` | UTF-8, LF endings, final newline, trim trailing whitespace |

> **Note**: `.clang-format` uses 2-space indentation, while `.editorconfig` lists 4 spaces for code files. Existing code matches `clang-format`, so do not "fix" files toward 4-space indentation.

---

## Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Files | `snake_case` | `image_processor.cpp`, `device_buffer.hpp` |
| Classes/Structs | `PascalCase` | `DeviceBuffer`, `GpuImage`, `FiltersTest` |
| Functions/Methods | `lowerCamelCase` | `copyFromHost`, `downloadImage`, `processBatchHost` |
| Enum Types/Values | `PascalCase` | `ThresholdType::Binary`, `FlipDirection::Horizontal` |
| Private Members | `snake_case_` | `devicePtr_`, `size_`, `streams_` |

---

## Includes and File Layout

- Use `#pragma once` in headers
- Wrap declarations and definitions in `namespace gpu_image`
- Put the matching project header first in `.cpp` and `.cu` files
- After project headers, include standard library and CUDA headers
- Keep public declarations in `include/gpu_image/` and implementations in `src/`
- Add new tests under `tests/` and register them in the `gpu_image_tests` target
- Do not churn include order or file organization in unrelated files

---

## API Design and Types

| Guideline | Description |
|-----------|-------------|
| Prefer small, direct changes | Over introducing helper layers |
| Follow existing split | Between low-level operator modules and `ImageProcessor` wrappers |
| `const T&` | For read-only heavy inputs |
| Non-const reference output parameters | For operator-style APIs |
| Return value objects | When matching existing `ImageProcessor` wrapper style |
| `explicit` | Mark single-argument constructors |
| `[[nodiscard]]` | For accessors whose result should not be ignored |
| `noexcept` | For trivial accessors, moves, ownership-transfer helpers |
| `enum class` | Over unscoped enums |
| `cudaStream_t stream = nullptr` | Async-capable operators should preserve this API shape |
| Types in use | `int` for dimensions/channels, `size_t` for byte counts, `unsigned char` for pixels |
| Prefer existing types | `DeviceBuffer`, `GpuImage`, `HostImage` over parallel abstractions |

---

## Error Handling and CUDA

| Guideline | Description |
|-----------|-------------|
| Validate inputs at top | Of public functions |
| `std::invalid_argument` | For bad caller input or invalid image parameters |
| `std::runtime_error` | For broader runtime failures |
| `CUDA_CHECK(...)` | Wrap CUDA runtime calls |
| `cudaGetLastError()` | Call after kernel launches before returning |
| Keep error messages short | Specific, consistent with current wording |
| No silent clamping | Unless current API already does so |
| Low-level operators | Generally check `cudaGetLastError()` |
| High-level wrappers | Usually call `cudaDeviceSynchronize()` before returning |
| Preserve sync-vs-async contract | Of the layer you are editing |

### Error Types

```cpp
enum class ErrorCode {
  Success = 0,
  OutOfMemory,
  InvalidMemoryAccess,
  InvalidImageSize,
  InvalidKernelSize,
  InvalidChannelCount,
  NullPointer,
  CudaDriverError,
  CudaKernelLaunchError,
  CudaSyncError,
  FileNotFound,
  FileReadError,
  FileWriteError,
  UnsupportedFormat
};
```

---

## Namespace, Comments, and Docs

| Guideline | Description |
|-----------|-------------|
| Library code | Does not use `using namespace gpu_image;` in implementation files |
| Tests/examples/benchmarks | Do use `using namespace gpu_image;` |
| Comments | Brief and useful; explain invariants, ownership, non-obvious math, GPU constraints |
| Language | Existing comments are often Chinese or bilingual; match surrounding file |
| README updates | If behavior changes in user-visible way |

---

## CI/CD Information

### CI Configuration

| Property | Value |
|----------|-------|
| CI image | `nvidia/cuda:12.4.1-devel-ubuntu22.04` |
| CI packages | `cmake`, `ninja-build`, `g++`, `git` |
| CI build | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` |
| CI test | `ctest --test-dir build --output-on-failure` |
| Format check | Separate job using `clang-format-14` |

### GitHub Actions Workflows

- **CI workflow** (`.github/workflows/ci.yml`): Build and test on every push/PR
- **Pages workflow** (`.github/workflows/pages.yml`): Deploy documentation to GitHub Pages

---

## Key Data Structures

### GpuImage (GPU 图像表示)

```cpp
struct GpuImage {
  DeviceBuffer buffer;
  int width = 0;
  int height = 0;
  int channels = 0;  // 1 for grayscale, 3 for RGB, 4 for RGBA
  
  size_t pitch() const;
  size_t totalBytes() const;
  bool isValid() const;
  size_t pixelCount() const;
};
```

### HostImage (Host 端图像数据)

```cpp
struct HostImage {
  std::vector<unsigned char> data;
  int width = 0;
  int height = 0;
  int channels = 0;
  
  unsigned char& at(int x, int y, int c);
  size_t totalBytes() const;
  bool isValid() const;
};
```

### DeviceBuffer (GPU 内存管理)

```cpp
class DeviceBuffer {
public:
  explicit DeviceBuffer(size_t size);
  ~DeviceBuffer();
  
  // Disable copy, enable move
  DeviceBuffer(const DeviceBuffer&) = delete;
  DeviceBuffer(DeviceBuffer&&) noexcept;
  
  void copyFromHost(const void* hostPtr, size_t size);
  void copyToHost(void* hostPtr, size_t size) const;
  void copyFromHostAsync(const void* hostPtr, size_t size, cudaStream_t stream);
  void copyToHostAsync(void* hostPtr, size_t size, cudaStream_t stream) const;
  
  [[nodiscard]] void* data() noexcept;
  [[nodiscard]] size_t size() const noexcept;
  [[nodiscard]] bool isValid() const noexcept;
};
```

---

## Available Operators

| Category | Operators |
|----------|-----------|
| **Pixel** | `invert`, `toGrayscale`, `adjustBrightness` |
| **Convolution** | `gaussianBlur`, `sobelEdgeDetection`, `convolve` |
| **Histogram** | `histogram`, `histogramRGB`, `histogramEqualize` |
| **Geometric** | `resize`, `rotate`, `flip`, `affine`, `perspective`, `crop`, `pad` |
| **Morphology** | `erode`, `dilate`, `open`, `close`, `gradient`, `topHat`, `blackHat` |
| **Threshold** | `global`, `adaptive`, `otsu` |
| **Color Space** | `rgbToHsv`, `hsvToRgb`, `rgbToYuv`, `yuvToRgb` |
| **Filters** | `median`, `bilateral`, `box`, `sharpen`, `laplacian` |

---

## Practical Agent Guidance

1. **Make the smallest correct change**
2. **Preserve public API names and signatures** unless the task requires a breaking change
3. **Do not add new third-party dependencies** unless the task clearly requires them
4. **Do not create new targets** if an existing module or test target is sufficient
5. **Before finishing, run the most relevant filtered test command** when CUDA is available in the environment
6. **Follow SDD workflow**: Specs → Implementation → Tests

---

## Related Documents

- [Specifications](specs/README.md) - Product requirements, RFCs, and technical designs
- [Documentation](docs/README.md) - User guides, tutorials, and API reference (bilingual)
- [Contributing Guide](CONTRIBUTING.md) - Full contribution guidelines
- [Changelog](CHANGELOG.md) - Version history

---

## External Rule Files

| File | Status |
|------|--------|
| `.cursor/rules/` | Not present |
| `.cursorrules` | Not present |
| `.github/copilot-instructions.md` | Not present |

If any of these files are added later, treat them as higher-priority instructions and update this file.
