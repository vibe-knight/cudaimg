# 2026-03-09 Workflow Optimization / 工作流优化

## Summary / 摘要

Added standardized core GitHub Actions CI workflow for the repository.

为仓库添加标准化的核心 GitHub Actions CI 工作流。

## Changes / 变更内容

### English

#### Added
- `.github/workflows/ci.yml` - Core CI workflow
- Standardized triggers: `push`, `pull_request`, `workflow_dispatch`
- CUDA container-based build validation
- `clang-format` check job

#### Technical Details
- CI runs in `nvidia/cuda:12.4.1-devel-ubuntu22.04` container
- Automatic build and test execution
- Code format verification on PR
- Parallel job execution (build + format check)

### 中文

#### 新增
- `.github/workflows/ci.yml` - 核心 CI 工作流
- 标准化触发器：`push`、`pull_request`、`workflow_dispatch`
- 基于 CUDA 容器的构建验证
- `clang-format` 检查任务

#### 技术细节
- CI 在 `nvidia/cuda:12.4.1-devel-ubuntu22.04` 容器中运行
- 自动构建和测试执行
- PR 时进行代码格式验证
- 并行任务执行（构建 + 格式检查）

## Impact / 影响

- Automated quality assurance
- Consistent code formatting
- Faster feedback on contributions

- 自动化质量保证
- 一致的代码格式
- 更快的贡献反馈

## References / 参考

- Related: [CHANGELOG.md v1.1.0](../CHANGELOG.md)
