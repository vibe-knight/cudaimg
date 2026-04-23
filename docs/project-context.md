---
project_name: mini-opencv
user_name: Shane
date: 2026-04-23
sections_completed:
  ['technology_stack', 'language_rules', 'testing_rules', 'code_quality', 'workflow_rules', 'architecture', 'anti_patterns']
status: 'complete'
rule_count: 58
optimized_for_llm: true
---

# Project Context for AI Agents

_This file contains critical rules and patterns that AI agents must follow when implementing code in this project. Focus on unobvious details that agents might otherwise miss._

---

## Technology Stack & Versions

### Core Technologies

| Technology | Version | Notes |
|------------|---------|-------|
| **CUDA** | 11.0+ (推荐 12.x) | 需要 nvcc 编译器 |
| **C++** | C++17 | 必须标准 |
| **CMake** | 3.18+ (推荐 3.24+) | 构建系统 |
| **GPU Compute Capability** | 7.5+ (Turing) | RTX 20/30/40 系列 |

### Dependencies

| Dependency | Version | Purpose |
|------------|---------|---------|
| **Google Test** | v1.14.0 | 单元测试框架 |
| **Google Benchmark** | v1.8.3 | 性能基准测试 (可选) |
| **stb** | master | 图像 I/O (可选) |

### CUDA Specifics

- **CUDA Standard**: 14 (for `.cu` files)
- **C++ Standard**: 17 (for `.cpp` files)
- **CUDA Architectures**: Native auto-detection (CMake 3.24+) or fallback to 75, 80, 86, 89
- **Compiler Flag**: `--expt-relaxed-constexpr` for CUDA kernel constexpr support

---

## Language-Specific Rules

### C++ Rules

1. **标准要求**: 必须使用 C++17 标准，禁用扩展 (`CMAKE_CXX_EXTENSIONS OFF`)
2. **命名空间**: 所有代码必须在 `gpu_image` 命名空间内
3. **头文件**: 使用 `.hpp` 扩展名，`#pragma once` 作为包含保护
4. **禁止拷贝，启用移动**: 对于 GPU 资源类（如 `DeviceBuffer`），禁用拷贝构造/赋值，启用移动语义
5. **`[[nodiscard]]` 属性**: 用于返回所有权的静态工厂方法（如 `DeviceBuffer::fromRaw()`）
6. **析构函数 noexcept**: 所有资源管理类的析构函数必须声明 `noexcept`
7. **输入验证顺序**: 有效性检查 → 参数约束检查 → 其他验证

### CUDA Kernel Rules

1. **Kernel 命名**: 使用 `XxxKernel` 后缀（如 `invertKernel`, `toGrayscaleKernel`）
2. **线程块大小**:
   - 2D 图像处理：`dim3(16, 16)` = 256 线程
   - 1D 操作：256 或 512 线程
   - **必须是 32 的倍数**（warp 效率）
3. **Grid 计算**: 使用辅助函数 `calculateGridBlock()` 统一计算
4. **边界检查**: Kernel 内必须检查 `if (x < width && y < height)`
5. **Stream 参数**: 所有 Kernel 启动必须支持 `cudaStream_t stream = nullptr` 参数
6. **Kernel 启动格式**: 显式写 `sharedMem=0`，即使为 0
   ```cpp
   kernel<<<grid, block, 0, stream>>>(args);  // ✓ 显式写 0
   kernel<<<grid, block, stream>>>(args);      // ✗ 错误格式
   ```
7. **错误检查**: Kernel 启动后立即调用 `CUDA_CHECK(cudaGetLastError())`

### Stream Safety Rules (流安全规则)

1. **输入输出分离**: 非原地操作时，输入和输出 `GpuImage` 必须是不同的对象
   ```cpp
   // ✗ 未定义行为
   ConvolutionEngine::gaussianBlur(img, img, ...);

   // ✓ 安全模式
   GpuImage temp;
   ConvolutionEngine::gaussianBlur(input, temp, ...);

   // ✓ 使用专门的 in-place 方法
   PixelOperator::invertInPlace(image, stream);
   ```
2. **同步责任**:
   - 底层 Operator API：**调用者负责同步**
   - 高级 ImageProcessor API：可以隐式同步
3. **异步错误检查**: 异步操作后不能立即 `CUDA_CHECK`，需先同步

### Error Handling Rules

1. **CUDA_CHECK 使用边界**:
   - Kernel 启动后：必须调用
   - 异步操作后：需先同步再检查
   - 同步操作后：可以立即检查
2. **异常 vs Result<T>**:
   - 公共高级 API（`ImageProcessor`）：使用异常
   - 底层 operator API：可使用 `Result<T>` 避免异常开销

---

## Testing Rules

### Test Structure

1. **测试框架**: Google Test (v1.14.0)
2. **测试文件命名**: `test_<module>.cpp`（如 `test_device_buffer.cpp`, `test_convolution.cpp`）
3. **测试类命名**: `<Module>Test` 继承 `::testing::Test`
4. **测试位置**: `tests/<category>/test_<module>.cpp`

### Test Patterns

1. **CUDA 可用性检查**: 每个测试夹具必须在 `SetUp()` 中检查 CUDA
   ```cpp
   void SetUp() override {
     int deviceCount;
     cudaError_t err = cudaGetDeviceCount(&deviceCount);
     if (err != cudaSuccess || deviceCount == 0) {
       GTEST_SKIP() << "CUDA not available";
     }
   }
   ```

2. **属性驱动测试**: 测试注释必须标注 Feature 和 Property
   ```cpp
   /**
    * DeviceBuffer 单元测试
    *
    * Feature: gpu-image-processing, Property 1: 数据传输往返一致性
    * Validates: Requirements 1.1, 1.2
    */
   ```

3. **往返一致性测试**: 所有数据传输必须验证往返一致性
   ```cpp
   // Pattern: upload → process → download → verify
   std::vector<unsigned char> original(size);
   DeviceBuffer buffer(size);
   buffer.copyFromHost(original.data(), size);
   std::vector<unsigned char> downloaded(size);
   buffer.copyToHost(downloaded.data(), size);
   EXPECT_EQ(original, downloaded);  // 必须完全一致
   ```

4. **边界条件测试**: 必须测试空数据、零大小、无效参数
   ```cpp
   TEST_F(DeviceBufferTest, EmptyBuffer) { ... }
   TEST_F(DeviceBufferTest, ZeroSizeAllocation) { ... }
   TEST_F(DeviceBufferTest, InvalidParameters) { ... }
   ```

5. **异常测试**: 使用 `EXPECT_THROW` 验证异常抛出
   ```cpp
   EXPECT_THROW(buffer.copyFromHost(nullptr, 100), std::invalid_argument);
   ```

### Test Organization

```
tests/
├── core/           # 核心基础设施测试
│   ├── test_main.cpp
│   └── test_device_buffer.cpp
├── operators/      # 算子测试
│   ├── test_pixel_operator.cpp
│   ├── test_convolution.cpp
│   └── ...
└── processing/     # 处理器测试
    └── test_pipeline.cpp
```

---

## Code Quality & Style Rules

### Formatting (clang-format)

基于 `.clang-format` 配置：
- **风格基础**: LLVM
- **缩进宽度**: 2 空格
- **行宽限制**: 80 字符
- **指针对齐**: 左对齐 (`int* ptr` 而非 `int *ptr`)

### File Organization

1. **目录结构**:
   ```
   include/gpu_image/   # 公共头文件
   src/                  # 实现文件 (.cpp 和 .cu)
   tests/                # 测试文件
   examples/             # 示例程序
   ```

2. **文件命名约定**:
   | 类型 | 命名模式 | 示例 |
   |------|----------|------|
   | 头文件 | `snake_case.hpp` | `device_buffer.hpp` |
   | 源文件 | `snake_case.cpp/.cu` | `device_buffer.cu` |
   | 测试文件 | `test_snake_case.cpp` | `test_device_buffer.cpp` |

### Naming Conventions

1. **类/结构体**: `PascalCase`（如 `DeviceBuffer`, `GpuImage`）
2. **函数/方法**: `camelCase`（如 `copyFromHost`, `calculateGridBlock`）
3. **变量**: `camelCase`（如 `devicePtr`, `kernelSize`）
4. **常量/宏**: `UPPER_SNAKE_CASE`（如 `CUDA_CHECK`）
5. **命名空间**: `snake_case`（如 `gpu_image`, `ImageUtils`）
6. **成员变量**: 末尾下划线（如 `devicePtr_`, `size_`）

### Comment Style

1. **文件头注释**: 中文说明功能和关键特性
   ```cpp
   /**
    * DeviceBuffer 单元测试
    *
    * Feature: gpu-image-processing, Property 1: 数据传输往返一致性
    */
   ```

2. **API 文档注释**: 简洁中文描述
   ```cpp
   // 图像反色
   // 对每个像素执行: output[i] = 255 - input[i]
   static void invert(const GpuImage& input, GpuImage& output, ...);
   ```

3. **行内注释**: 解释"为什么"而非"是什么"

### Code Organization Patterns

1. **头文件结构**:
   ```cpp
   #pragma once

   #include "gpu_image/core/device_buffer.hpp"  // 项目头文件
   #include <cuda_runtime.h>                     // 系统头文件
   #include <vector>                             // STL 头文件

   namespace gpu_image {

   // 声明...

   } // namespace gpu_image
   ```

2. **源文件结构**:
   ```cpp
   #include "gpu_image/operators/pixel_operator.hpp"  // 对应头文件
   #include "gpu_image/core/cuda_error.hpp"           // 项目依赖
   #include <stdexcept>                                // 系统依赖

   namespace gpu_image {

   // CUDA Kernels 先定义
   __global__ void invertKernel(...) { ... }

   // 然后是实现
   void PixelOperator::invert(...) { ... }

   } // namespace gpu_image
   ```

---

## Critical Don't-Miss Rules

### Anti-Patterns to Avoid

1. **❌ 永远不要** 在 Kernel 中使用 `malloc` 或 `new`
   ```cpp
   // ✗ 错误
   __global__ void badKernel() {
     int* ptr = new int[100];  // 设备端动态分配禁止
   }
   ```

2. **❌ 永远不要** 忘记边界检查
   ```cpp
   // ✗ 错误 - 可能越界
   __global__ void badKernel(unsigned char* data, int width, int height) {
     int idx = blockIdx.x * blockDim.x + threadIdx.x;
     data[idx] = 255;  // 没有检查是否超出图像边界
   }
   ```

3. **❌ 永远不要** 同一 GpuImage 既作输入又作输出（非原地操作）
   ```cpp
   // ✗ 未定义行为
   GpuImage img;
   PixelOperator::invert(img, img);  // 除非明确是 in-place 方法
   ```

4. **❌ 永远不要** 在析构函数中抛出异常
   ```cpp
   // ✗ 错误
   ~DeviceBuffer() {
     CUDA_CHECK(cudaFree(devicePtr_));  // 可能抛出异常
   }

   // ✓ 正确
   ~DeviceBuffer() noexcept {
     cudaFree(devicePtr_);  // 忽略返回值，不抛异常
   }
   ```

### Edge Cases to Handle

1. **空图像处理**: 始终检查 `image.isValid()` 和 `image.totalBytes() > 0`
2. **无效通道数**: 只支持 1、3、4 通道，其他必须抛出异常
3. **零大小分配**: `DeviceBuffer(0)` 应返回无效缓冲区而非崩溃
4. **CUDA 设备丢失**: 处理 `cudaErrorDeviceUninitialized` 等设备错误

### Performance Gotchas

1. **频繁小 Kernel 启动**: 合并小操作减少启动开销
2. **Host-Device 频繁传输**: 批量传输减少 PCIe 往返
3. **未使用 Pinned Memory**: 大量数据传输时使用 `cudaMallocHost`
4. **流未复用**: 创建流有开销，应复用 `StreamManager`

### CI/CD Requirements

1. **代码格式检查**: CI 使用 `clang-format-14 --dry-run --Werror`
2. **所有测试必须通过**: `ctest --test-dir build --output-on-failure`
3. **CUDA 容器**: CI 在 `nvidia/cuda:12.4.1-devel-ubuntu22.04` 中运行

---

## Architecture Patterns

### Three-Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                      APPLICATION LAYER                           │
│         ImageProcessor (同步)  ·  PipelineProcessor (异步)       │
├─────────────────────────────────────────────────────────────────┤
│                      OPERATOR LAYER                              │
│  PixelOperator │ ConvolutionEngine │ HistogramCalculator        │
│  ImageResizer  │ Morphology        │ Threshold │ ColorSpace     │
│  Geometric     │ Filters           │ ImageArith                │
├─────────────────────────────────────────────────────────────────┤
│                   INFRASTRUCTURE LAYER                           │
│  DeviceBuffer │ MemoryManager │ StreamManager │ GpuImage        │
│  HostImage    │ ImageIO       │ CudaError                         │
└─────────────────────────────────────────────────────────────────┘
```

### Layer Dependencies

- **Application Layer**: 依赖 Operator 和 Infrastructure
- **Operator Layer**: 仅依赖 Infrastructure
- **Infrastructure Layer**: 无项目内部依赖，仅依赖 CUDA Runtime

### Memory Management Pattern

```cpp
// 标准模式: Load-Process-Download
HostImage host = loadImage("input.jpg");
GpuImage gpu = processor.loadFromHost(host);
GpuImage result = processor.gaussianBlur(gpu, 5, 1.5f);
HostImage output = processor.downloadImage(result);
```

### Operator Design Pattern

- **Stateless**: 所有 operator 类使用静态方法，无实例状态
- **Stream-Aware**: 所有方法接受可选的 `cudaStream_t` 参数
- **Output Parameter**: 输出图像通过引用参数返回，由调用者管理生命周期

---

## Development Workflow Rules

### Build Commands

```bash
# 开发环境配置 (仅编译当前 GPU 架构，加快编译)
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CUDA_ARCHITECTURES=native \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON

# 生产环境配置 (多架构支持)
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CUDA_ARCHITECTURES="75;80;86;89" \
  -DBUILD_BENCHMARKS=ON

# 编译 (并行)
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure

# 运行示例
./build/bin/basic_example
```

### CUDA Architecture Support

| 架构代号 | 计算能力 | GPU 系列 | 支持状态 |
|----------|----------|----------|----------|
| Turing | 7.5 | RTX 20xx, GTX 16xx | ✅ 最低支持 |
| Ampere | 8.0/8.6 | RTX 30xx, A100 | ✅ 推荐 |
| Ada Lovelace | 8.9 | RTX 40xx | ✅ 最佳性能 |

**关键决策**: 最低要求 CC 7.5，低于此版本 (如 Pascal 6.x) 不支持

### Build Options

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_TESTS` | ON | 构建测试 |
| `BUILD_EXAMPLES` | ON | 构建示例 |
| `BUILD_BENCHMARKS` | OFF | 构建基准测试 |
| `GPU_IMAGE_ENABLE_IO` | ON | 启用 stb 图像 I/O |

### Debugging & Profiling Workflow

```bash
# 调试构建
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug

# 交互调试
cuda-gdb ./build-debug/bin/basic_example

# 内存错误检测 (推荐)
compute-sanitizer ./build-debug/bin/gpu_image_tests

# 性能分析 (新版推荐)
nsys profile --stats=true ./build/bin/basic_example
```

| 工具 | 用途 | 命令 |
|------|------|------|
| `compute-sanitizer` | 内存错误检测 | `compute-sanitizer ./tests` |
| `nsys` | 性能分析 | `nsys profile ./app` |
| `cuda-gdb` | 交互调试 | `cuda-gdb ./app` |

### Git Workflow

1. **分支策略**: 主分支 `main` 或 `master`
2. **CI 触发**: Push 和 PR 到主分支自动触发
3. **并发控制**: 同一 workflow/ref 的运行会取消之前的

### Local CI Simulation

```bash
# 1. 格式检查
find . -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.cu' \) \
  -not -path './build/*' | xargs clang-format-14 --dry-run --Werror

# 2. 测试检查
ctest --test-dir build --output-on-failure

# 3. 内存检查 (可选)
compute-sanitizer ./build/bin/gpu_image_tests
```

### PR Checklist

- [ ] 代码通过 `clang-format-14` 检查
- [ ] 所有测试通过
- [ ] 新功能有对应测试
- [ ] 公共 API 有文档注释
- [ ] 通过 `compute-sanitizer` 内存检查 (重要变更)

---

## Usage Guidelines

### For AI Agents

1. **阅读顺序**: 在实现任何代码前，完整阅读此文件
2. **严格遵守**: 所有规则必须完全遵循，不可选择性忽略
3. **优先级**: 当有疑问时，选择更保守/更严格的选项
4. **更新维护**: 发现新的模式或规则时，更新此文件

### For Humans

1. **保持精简**: 文件应聚焦于 AI 代理需要的信息
2. **及时更新**: 技术栈或模式变更时更新
3. **定期审查**: 每季度审查一次，移除过时规则
4. **避免冗余**: 移除变得显而易见的规则

---

**Last Updated**: 2026-04-23 | **Total Rules**: 58 | **Sections**: 7
