# Development History / 开发历史

Historical archive of project development milestones. For detailed release notes, see [CHANGELOG.md](../CHANGELOG.md).

项目开发里程碑的历史归档。详细的发布说明请参阅 [CHANGELOG.md](../CHANGELOG.md)。

---

## Timeline

| Date | Version | Key Changes |
|------|---------|-------------|
| 2026-03-24 | v2.0.0 | Correctness fixes, CI hardening, parameter validation / 正确性修复、CI 加固、参数校验 |
| 2026-03-10 | v1.3.0 | GitHub Pages optimization, professional landing page / GitHub Pages 优化、专业落地页 |
| 2026-03-10 | v1.2.0 | Workflow deep standardization / 工作流深度标准化 |
| 2026-03-09 | v1.1.0 | Initial CI workflow setup / 初始 CI 工作流搭建 |
| 2025-02-13 | v1.0.0 | Initial release / 初始版本发布 |

---

## Detailed Milestones

### 2026-03-24: Correctness and CI Hardening / 正确性与 CI 加固

**English:**
- Fixed geometric transform bilinear sampling boundary issue
- Removed global shared constant kernel dependency in convolution
- Added parameter validation for Gaussian and bilateral filters
- Enhanced CI pipeline with `ctest --output-on-failure`

**中文：**
- 修复几何变换双线性采样边界问题
- 移除卷积实现中的全局共享常量卷积核依赖
- 为高斯和双边滤波添加参数校验
- 使用 `ctest --output-on-failure` 增强 CI 流水线

### 2026-03-10: Pages Optimization / 页面优化

**English:**
- Professional landing page with architecture diagram
- Complete feature matrix and GPU support table
- SEO optimization in `_config.yml`
- Bilingual README enhancement

**中文：**
- 带架构图的专业落地页
- 完整的功能矩阵和 GPU 支持表
- `_config.yml` 中的 SEO 优化
- 双语 README 增强

### 2025-02-13: Project Foundation / 项目基础建设

**English:**
- Initial project structure
- MIT License
- Basic CI configuration
- Core GPU image processing operators

**中文：**
- 初始项目结构
- MIT 许可证
- 基础 CI 配置
- 核心 GPU 图像处理算子

---

Original detailed logs can be found in individual files below:
- `2025-02-13_project-infrastructure.md`
- `2026-03-09_workflow-optimization.md`
- `2026-03-10_pages-optimization.md`
- `2026-03-10_workflow-deep-standardization.md`
- `2026-03-24_correctness-and-ci-hardening.md`
