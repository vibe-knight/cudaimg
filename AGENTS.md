# AGENTS Guide

> This guide is for coding agents (AI assistants) working in `mini-opencv`.
> It captures the repository's build, test, formatting, and code-style conventions.
> Language: The project's source code comments and documentation are bilingual (Chinese and English).

---

## Project Overview

**Mini-OpenCV** is a CUDA-based high-performance image processing library providing GPU-accelerated operators for computer vision applications. It delivers 30-50x faster performance than CPU OpenCV for comparable operations.

**Key Information:**
- **Version:** 2.1.0
- **License:** MIT
- **Repository:** https://github.com/LessUp/mini-opencv
- **Documentation:** https://lessup.github.io/mini-opencv/

### Technology Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++17, CUDA 14 |
| **Build System** | CMake 3.18+ |
| **CUDA** | 11.0+ (Recommended: 12.x) |
| **Testing** | Google Test v1.14.0 |
| **Benchmarking** | Google Benchmark v1.8.3 |
| **Image I/O** | stb (fetched via CMake FetchContent) |
| **Documentation** | Jekyll + Just the Docs theme |

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

---

## Key Configuration Files

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Main build configuration (library targets, dependencies, CUDA settings) |
| `.clang-format` | LLVM-style formatting (2-space indent, 80-column limit) |
| `.editorconfig` | UTF-8, LF endings, final newline, trim trailing whitespace |
| `docs/_config.yml` | Jekyll/GitHub Pages config (Just the Docs theme v0.9.0) |
| `.github/workflows/ci.yml` | CI build and test pipeline |
| `.github/workflows/pages.yml` | Documentation deployment to GitHub Pages |

### CMakeLists.txt Key Details

**Library Target:** `gpu_image_processing` (alias: `gpu_image::gpu_image_processing`)

**Build Options:**
| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build test suite with Google Test |
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_BENCHMARKS` | OFF | Build benchmark executable |
| `GPU_IMAGE_ENABLE_IO` | ON | Enable image file I/O via stb |

**CUDA Configuration:**
- Auto-detects GPU architecture (native on CMake 3.24+)
- Fallback architectures: 75, 80, 86, 89 (Turing+)
- Compiles with `--expt-relaxed-constexpr`

**Dependencies (via FetchContent):**
- Google Test v1.14.0 (required for tests)
- Google Benchmark v1.8.3 (optional)
- stb (master branch, for image I/O)

---

## Project Philosophy: Spec-Driven Development (SDD)

This project uses **OpenSpec** framework for spec-driven development. All code implementations must use the specification documents in the `/openspec/specs` directory as the Single Source of Truth.

### Directory Context

| Directory | Purpose |
|-----------|---------|
| `/openspec/specs/` | Main specifications (Single Source of Truth) |
| `/docs/` | User documentation, tutorials, and setup guides (bilingual) |

### OpenSpec Workflow

| Command | Purpose |
|---------|---------|
| `/opsx:explore <idea>` | Think through ideas before committing |
| `/opsx:propose <feature>` | Create a new change proposal |
| `/opsx:apply` | Implement tasks from the change |

### Active Specifications

| Spec | Type | Status |
|------|------|--------|
| [GPU Image Processing Requirements](openspec/specs/gpu-image-processing/requirements.md) | Product | ✅ Implemented |
| [GPU Image Processing Design](openspec/specs/gpu-image-processing/design.md) | RFC | ✅ Implemented |
| [GPU Image Processing API](openspec/specs/gpu-image-processing/api.md) | API | ✅ Implemented |
| [Architecture Overview](openspec/specs/architecture.md) | Architecture | ✅ Implemented |

### AI Agent Workflow Instructions

When developing a new feature, modifying existing functionality, or fixing a bug, **you must strictly follow this workflow without skipping any steps**:

#### Step 1: Propose Change

- Use `/opsx:propose <feature-description>` to create a change proposal
- This generates: `proposal.md`, `specs/`, `design.md`, `tasks.md`
- If unsure about the approach, use `/opsx:explore <idea>` first to investigate

#### Step 2: Review & Design

- Review the generated artifacts in `/openspec/changes/<change-name>/`
- Refine the design if needed
- If the change conflicts with existing specs, **stop and ask the user** whether to update specs first

#### Step 3: Apply & Implement

- Use `/opsx:apply` to implement tasks from the change
- Tasks are marked complete with `[x]` checkboxes
- **100% comply with spec definitions** (including variable names, API paths, data types, etc.)
- Do not add features not defined in specs (No Gold-Plating)

#### Step 4: Test against Specs

- Write unit tests and integration tests based on the acceptance criteria
- Ensure test cases cover all boundary conditions described in the specs

---

## Project Structure

```
mini-opencv/
├── include/gpu_image/          # Public header files
│   ├── core/                   # DeviceBuffer, GpuImage, CudaError, kernel_helpers, memory_manager
│   ├── operators/              # CUDA operator interfaces
│   ├── processing/             # ImageProcessor, PipelineProcessor
│   ├── io/                     # ImageIO
│   └── gpu_image_processing.hpp # Master header file
├── src/                        # Implementations (.cpp and .cu files)
├── tests/                      # Test suite (Google Test)
├── examples/                   # Example programs
├── benchmarks/                 # Performance benchmarks
├── openspec/specs/             # Specifications (SDD)
├── docs/                       # Documentation (Jekyll/GitHub Pages)
├── cmake/                      # CMake modules
├── scripts/                    # Utility scripts
└── .github/workflows/          # CI/CD (ci.yml, pages.yml)
```

**Main library target:** `gpu_image_processing` (alias: `gpu_image::gpu_image_processing`)

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

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build test suite |
| `BUILD_EXAMPLES` | ON | Build example programs |
| `BUILD_BENCHMARKS` | OFF | Build benchmark executable |
| `GPU_IMAGE_ENABLE_IO` | ON | Enable image file I/O via stb |

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
- Test annotations should include Feature and Property references:
  ```cpp
  /**
   * Feature: gpu-image-processing, Property 1: Data Transfer Round-Trip Consistency
   * Validates: Requirements 1.1, 1.2
   */
  ```

### BDD and Property Testing

The project follows a **dual testing strategy**:

| Type | Framework | Purpose |
|------|-----------|---------|
| **Unit Tests** | Google Test | Specific examples, edge cases, error paths |
| **Property Tests** | RapidCheck | Universal properties, mathematical invariants |
| **Benchmarks** | Google Benchmark | Performance measurement |

**9 Core Correctness Properties** (from design spec):
1. Data Transfer Round-Trip Consistency
2. Invert Operation Involution
3. Grayscale Formula Correctness
4. Brightness Adjustment Range Invariance
5. Convolution vs Reference Consistency
6. Boundary Handling Correctness
7. Histogram Sum Invariance
8. Scaling Approximate Round-Trip
9. Pipeline Processing Result Consistency

**BDD Feature Files** use Gherkin format:
```gherkin
Feature: Image Invert Operation
  As a developer
  I want to invert image colors
  So that I can create negative images

  Scenario: Double invert returns original
    Given any valid image
    When I apply invert operation twice
    Then the result should equal the original image
```

---

## CI/CD and Deployment

### CI Configuration

| Property | Value |
|----------|-------|
| CI image | `nvidia/cuda:12.4.1-devel-ubuntu22.04` |
| CI packages | `cmake`, `ninja-build`, `g++`, `git` |
| CI build | `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` |
| CI test | `ctest --test-dir build --output-on-failure` |
| Format check | `clang-format-14 --dry-run --Werror` |

### GitHub Actions Workflows

**CI Workflow** (`.github/workflows/ci.yml`):
- **Triggers:** Push/PR to main/master, manual dispatch
- **Jobs:**
  - **Build:** Configure with CMake, build with ninja/g++
  - **Test:** `ctest --test-dir build --output-on-failure`
  - **Format Check:** `clang-format-14 --dry-run --Werror`

**Pages Workflow** (`.github/workflows/pages.yml`):
- **Purpose:** Deploy documentation to GitHub Pages
- **Triggers:** Changes to `docs/**` or workflow file on main
- **Technology:** Jekyll with `just-the-docs` theme (v0.9.0)
- **URL:** https://lessup.github.io/mini-opencv
- **Features:**
  - Search enabled
  - Bilingual support (English/Chinese)
  - Mermaid diagrams
  - SEO optimization
  - Responsive design with auto light/dark mode

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

> **Note:** `.clang-format` uses 2-space indentation. Existing code matches `clang-format`, so do not "fix" files toward 4-space indentation.

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

### Documentation Standards

The project maintains **bilingual documentation** (English and Chinese):
- All documentation files have `.zh-CN.md` counterparts
- Language selector on each page
- Chinese and English comments in source code

**Jekyll Front Matter for docs:**
```yaml
---
layout: default
title: Page Title
nav_order: 1          # Navigation order
parent: Documentation # Parent category
description: SEO description
---
```

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

## Commit Message Format

Follow [Conventional Commits](https://www.conventionalcommits.org/) specification:

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types

| Type | Description |
|------|-------------|
| `feat` | A new feature |
| `fix` | A bug fix |
| `docs` | Documentation only changes |
| `style` | Changes that do not affect the code meaning (formatting, etc.) |
| `refactor` | A code change that neither fixes a bug nor adds a feature |
| `perf` | A code change that improves performance |
| `test` | Adding or updating tests |
| `chore` | Changes to the build process or auxiliary tools |
| `ci` | Changes to CI configuration files |

---

## Practical Agent Guidance

1. **Make the smallest correct change**
2. **Preserve public API names and signatures** unless the task requires a breaking change
3. **Do not add new third-party dependencies** unless the task clearly requires them
4. **Do not create new targets** if an existing module or test target is sufficient
5. **Before finishing, run the most relevant filtered test command** when CUDA is available in the environment
6. **Follow SDD workflow**: Specs → Implementation → Tests
7. **Update bilingual documentation** if user-visible behavior changes (both English and Chinese versions in `/docs/`)
8. **Ensure test coverage** aligns with spec acceptance criteria
9. **Verify formatting** using clang-format-14 before finalizing changes

---

## Related Documents

- [Specifications](openspec/specs/architecture.md) - Product requirements and technical designs
- [Contributing Guide](CONTRIBUTING.md) - Full contribution guidelines
- [Changelog](CHANGELOG.md) - Version history

---

## External Rule Files

| File | Status |
|------|--------|
| `.github/copilot-instructions.md` | Present - GitHub Copilot guidelines |
