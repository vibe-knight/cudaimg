# AGENTS Guide

This guide is for coding agents working in `mini-opencv`.
It captures the repository's actual build, test, formatting, and code-style conventions.

## Project Philosophy: Spec-Driven Development (SDD)

This project strictly follows the **Spec-Driven Development (SDD)** paradigm. All code implementations must use the specification documents in the `/specs` directory as the Single Source of Truth.

### Directory Context

| Directory | Purpose |
|-----------|---------|
| `/specs/product/` | Product feature definitions and acceptance criteria (PRD) |
| `/specs/rfc/` | Technical design documents and architecture proposals |
| `/specs/api/` | API interface definitions (function signatures, OpenAPI) |
| `/specs/db/` | Data model and schema definitions |
| `/specs/testing/` | Test specifications and BDD feature files |
| `/docs/` | User documentation, tutorials, and setup guides |

### AI Agent Workflow Instructions

When you (the AI) are asked to develop a new feature, modify existing functionality, or fix a bug, **you must strictly follow this workflow without skipping any steps**:

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

### Code Generation Rules
- Any externally exposed API changes must synchronously update `/specs/api/` definitions.
- If uncertain about technical details, consult `/specs/rfc/` for architecture conventions. Do not invent design patterns.

## Scope
`mini-opencv` is a CUDA-backed C++ image processing library built with CMake.
Public headers live in `include/gpu_image/`, implementations in `src/`, tests in `tests/`, examples in `examples/`, and optional benchmarks in `benchmarks/`.
The main library target is `gpu_image_processing`, with alias `gpu_image::gpu_image_processing`.

## External Rule Files
No `.cursor/rules/` directory was present during analysis.
No root `.cursorrules` file was present during analysis.
No `.github/copilot-instructions.md` file was present during analysis.
If any of those files are added later, treat them as higher-priority instructions and update this file.

## Build Facts
Build system: CMake 3.18+.
Language standards: C++17 and CUDA 14.
CUDA is required to configure the project; `nvcc` must exist on `PATH` or `CUDAToolkit_ROOT` must be set.
Runtime targets are emitted under `build/bin/`.
Defaults: `BUILD_TESTS=ON`, `BUILD_EXAMPLES=ON`, `BUILD_BENCHMARKS=OFF`, `GPU_IMAGE_ENABLE_IO=ON`.

## Build Commands
Use an out-of-tree build.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Build only specific targets
cmake --build build --target gpu_image_processing -j$(nproc)
cmake --build build --target gpu_image_tests -j$(nproc)
cmake --build build --target basic_example pipeline_example -j$(nproc)

# Enable benchmarks
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_BENCHMARKS=ON
cmake --build build --target gpu_image_benchmark -j$(nproc)

# Install
cmake --install build
```

If you add a new source file or test file, update `CMakeLists.txt` in the same change.

## Test Commands
Tests are built into one executable: `build/bin/gpu_image_tests`.
There is not one test binary per source file.
`gtest_discover_tests()` is used, so CTest exposes individual GTest cases like `FiltersTest.MedianFilter`.

```bash
# Full suite
ctest --test-dir build --output-on-failure

# List discovered tests
ctest --test-dir build -N

# Run one exact test through CTest
ctest --test-dir build -R '^FiltersTest\.MedianFilter$' --output-on-failure

# Run one fixture / pseudo-file scope through CTest
ctest --test-dir build -R '^FiltersTest\.' --output-on-failure

# Run the binary directly
./build/bin/gpu_image_tests

# Run one exact GTest case directly
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.MedianFilter

# Run one whole fixture directly
./build/bin/gpu_image_tests --gtest_filter=FiltersTest.*
```

If you need to target "one test file," use that file's fixture name, for example `FiltersTest.*` or `DeviceBufferTest.*`.

## Lint And Formatting
There is no separate static-lint target today.
The CI "format" job is the closest equivalent.
CI uses `clang-format-14 --dry-run --Werror` on `*.h`, `*.hpp`, `*.cpp`, `*.cu`, and `*.cuh`, excluding `build/` and `third_party/`.

```bash
# CI-equivalent format check
find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' \) -not -path './build/*' -not -path './third_party/*' -print0 | xargs -0 -r clang-format-14 --dry-run --Werror

# In-place formatting
find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' -o -name '*.cuh' \) -not -path './build/*' -not -path './third_party/*' -print0 | xargs -0 -r clang-format-14 -i
```

If only `clang-format` is installed locally, use it, but keep output identical to CI.

## Style Sources Of Truth
`.clang-format` is authoritative for C++ and CUDA layout.
`.editorconfig` is authoritative for UTF-8, LF endings, final newline, and trimming trailing whitespace.
There is a repo-specific mismatch: `.clang-format` uses 2-space indentation, while `.editorconfig` lists 4 spaces for code files.
Existing C++ and CUDA source matches `clang-format`, so do not "fix" files toward 4-space indentation.

## Naming Conventions
Use `snake_case` for filenames: `image_processor.cpp`, `device_buffer.hpp`, `test_filters.cpp`.
Use `PascalCase` for classes, structs, exceptions, and fixtures: `DeviceBuffer`, `GpuImage`, `CudaException`, `FiltersTest`.
Use `lowerCamelCase` for functions and methods: `copyFromHost`, `downloadImage`, `processBatchHost`.
Use `PascalCase` for enum types and enumerators: `ThresholdType::Binary`, `FlipDirection::Horizontal`.
Use trailing underscores for private members: `devicePtr_`, `size_`, `streams_`.

## Includes And File Layout
Use `#pragma once` in headers.
Wrap declarations and definitions in `namespace gpu_image`.
Put the matching project header first in `.cpp` and `.cu` files when there is one.
After project headers, include standard library and CUDA headers.
Keep public declarations in `include/gpu_image/` and implementations in `src/`.
Add new tests under `tests/` and register them in the `gpu_image_tests` target.
Do not churn include order or file organization in unrelated files.

## API Design And Types
Prefer small, direct changes over introducing helper layers.
Follow the existing split between low-level operator modules and higher-level `ImageProcessor` convenience wrappers.
Use `const T&` for read-only heavy inputs.
Use non-const reference output parameters when extending operator-style APIs.
Return value objects when matching the existing `ImageProcessor` wrapper style.
Mark single-argument constructors `explicit`.
Use `[[nodiscard]]` for accessors or helpers whose result should not be ignored.
Use `noexcept` for trivial accessors, moves, and ownership-transfer helpers when appropriate.
Prefer `enum class` over unscoped enums.
Async-capable operators usually accept `cudaStream_t stream = nullptr`; preserve that API shape when extending them.
The codebase commonly uses `int` for dimensions and channels, `size_t` for byte counts, and `unsigned char` for pixels.
Prefer existing project types like `DeviceBuffer`, `GpuImage`, and `HostImage` over introducing parallel abstractions.

## Error Handling And CUDA
Validate inputs at the top of public functions.
Use `std::invalid_argument` for bad caller input or invalid image parameters.
Use `std::runtime_error` for broader runtime failures where no narrower type already exists.
Wrap CUDA runtime calls with `CUDA_CHECK(...)` so failures become `CudaException`.
After kernel launches, call `CUDA_CHECK(cudaGetLastError())` before returning.
Keep error messages short, specific, and consistent with current wording.
Do not silently clamp, coerce, or auto-correct invalid inputs unless the current API already does so.
Low-level operator modules generally check `cudaGetLastError()`.
Higher-level `ImageProcessor` wrappers usually call `cudaDeviceSynchronize()` before returning.
Preserve the current sync-vs-async contract of the layer you are editing.

## Namespace, Comments, And Docs
Library code does not use `using namespace gpu_image;` in implementation files.
Tests, examples, and benchmarks do use `using namespace gpu_image;` today; follow that split unless surrounding code clearly differs.
Keep comments brief and useful.
Explain invariants, ownership, non-obvious math, or GPU constraints; do not narrate obvious assignments.
Existing comments are often Chinese or bilingual, so matching the surrounding file is acceptable.
If behavior changes in a user-visible way, update `README.md` when appropriate.

## Testing Conventions
Tests use GoogleTest fixtures with `TEST_F(...)`.
Most CUDA-dependent fixtures check `cudaGetDeviceCount()` in `SetUp()` and call `GTEST_SKIP()` when CUDA is unavailable.
Follow that pattern for new CUDA-dependent tests.
Prefer deterministic inputs and explicit expected values.
When validating image operations, assert image validity, dimensions, channels, and representative pixel values.
Use `EXPECT_THROW` for input validation behavior.
Add or update regression tests alongside bug fixes.

## CI Notes
CI runs in `nvidia/cuda:12.4.1-devel-ubuntu22.04`.
CI installs `cmake`, `ninja-build`, `g++`, and `git`.
Without presets, CI uses `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`, then `cmake --build build -j$(nproc)`, then `ctest --test-dir build --output-on-failure`.
There is also a separate formatting check job using `clang-format-14`.

## Practical Agent Guidance
Make the smallest correct change.
Preserve public API names and signatures unless the task requires a breaking change.
Do not add new third-party dependencies unless the task clearly requires them.
Do not create new targets if an existing module or test target is sufficient.
Before finishing, run the most relevant filtered test command when CUDA is available in the environment.
