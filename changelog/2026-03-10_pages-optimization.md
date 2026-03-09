# GitHub Pages 彻底优化 (2026-03-10)

## 变更内容

### index.md — 专业落地页重写
- 添加 CI/Pages/License/CUDA/C++/CMake 全套 badges
- 新增三层架构图（用户应用层 → 算子层 → 基础设施层）
- 新增功能矩阵：按类别列出所有算子及其优化技术
- 新增 GPU 架构支持表（Turing → Hopper）
- 新增技术栈表、项目结构详解、工程质量要点
- 完善快速开始：包含构建、测试、示例运行和代码示例
- 新增链接区：仓库、双语 README、贡献指南、变更日志

### _config.yml — SEO 与构建优化
- 优化 description 为多行中文描述，提升 SEO
- 添加 exclude 列表：排除 src/include/tests/examples/benchmarks/build 等大量源码和构建产物，加速 Jekyll 构建

### pages.yml — 工作流优化
- paths 触发条件补充 `index.md` 和 `changelog/**`
- sparse-checkout 精确到具体文件（index.md, README.md, README.zh-CN.md, CONTRIBUTING.md, LICENSE, _config.yml, changelog）
- 添加 `fetch-depth: 1` 加速 checkout

### README.md — 英文 README 全面升级
- 添加 CI + Docs + License + CUDA + C++17 + CMake 全套 badges
- 新增架构图、功能特性表（11 类算子 + 优化技术）
- 新增构建选项表、GPU 架构支持表
- 扩展项目结构（含模块说明）
- 新增工程质量章节

### README.zh-CN.md — 中文 README 同步升级
- 修复 badge 布局（CI badge 缺失、换行问题）
- 新增架构图、功能特性表
- 构建选项改为表格格式
- 新增 GPU 架构支持表
- 扩展项目结构（含模块说明）
- 新增工程质量章节

### .gitignore — 补充缺失项
- 添加 `_site/`、`.jekyll-cache/`、`.jekyll-metadata`、`.sass-cache/`、`.cache/`
