# 2026-03-24 Correctness and CI Hardening / 正确性与 CI 加固

## Summary / 摘要

Focused on deterministic correctness fixes with minimal changes while hardening CI validation and improving test coverage.

以最小的代价专注于确定性正确性修复，同时加固 CI 验证并改进测试覆盖率。

## Changes / 变更内容

### English

#### Fixed
- Geometric transform bilinear sampling boundary coordinate zeroing issue
- Removed convolution dependency on global shared constant kernels
- Gaussian blur and bilateral filter sigma parameter validation
- Converged Otsu and scaling round-trip test assumptions

#### Added
- Geometric and convolution regression tests
- Exception tests for invalid parameters
- `ctest --output-on-failure` in CI
- Repository-level `.clang-format`

#### Changed
- CUDA language standard to C++14 compatible
- Removed C++17 features blocking older NVCC
- Updated test coverage descriptions in documentation

### 中文

#### 修复
- 几何变换双线性采样边界坐标清零问题
- 移除卷积对全局共享常量卷积核的依赖
- 高斯模糊和双边滤波 sigma 参数校验
- 收敛 Otsu 和缩放往返测试假设

#### 新增
- 几何和卷积回归测试
- 无效参数的异常测试
- CI 中的 `ctest --output-on-failure`
- 仓库级 `.clang-format`

#### 变更
- CUDA 语言标准至 C++14 兼容
- 移除阻碍旧版 NVCC 的 C++17 特性
- 更新文档中的测试覆盖率描述

## Impact / 影响

- More reliable geometric transforms
- Safer multi-stream convolution
- Better CI feedback on failures
- Improved cross-compiler compatibility

- 更可靠的几何变换
- 更安全的跨流卷积
- 更好的 CI 失败反馈
- 改进的跨编译器兼容性

## References / 参考

- Related: [CHANGELOG.md v2.0.0](../CHANGELOG.md)
