# 2026-03-10 Workflow Deep Standardization / 工作流深度标准化

## Summary / 摘要

Second round of GitHub Actions standardization across all workflows with unified permissions, concurrency, and caching strategies.

所有工作流的第二轮 GitHub Actions 标准化，统一权限、并发和缓存策略。

## Changes / 变更内容

### English

#### Standardized
- Unified `permissions: contents: read` across workflows
- Consistent `concurrency` configuration
- Added `actions/configure-pages@v5` to Pages workflow
- `paths` trigger filters to reduce unnecessary builds

#### Technical Improvements
- Standardized job naming conventions
- Consistent checkout depth (`fetch-depth: 1`)
- Aligned timeout configurations
- Unified artifact handling

### 中文

#### 标准化
- 工作流统一 `permissions: contents: read`
- 一致的 `concurrency` 配置
- 在 Pages 工作流中添加 `actions/configure-pages@v5`
- `paths` 触发器过滤以减少不必要的构建

#### 技术改进
- 标准化的任务命名约定
- 一致的检出深度（`fetch-depth: 1`）
- 对齐的超时配置
- 统一的产物处理

## Impact / 影响

- Reduced CI resource usage
- Faster feedback loops
- Consistent security posture

- 减少 CI 资源使用
- 更快的反馈循环
- 一致的安全态势

## References / 参考

- Related: [CHANGELOG.md v1.2.0](../CHANGELOG.md)
