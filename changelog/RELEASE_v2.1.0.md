# Release v2.1.0 - Manual Creation Guide

## Release Information

- **Version:** v2.1.0
- **Title:** v2.1.0 - Documentation Overhaul & Bilingual Support
- **Date:** 2026-04-16

## Release Notes (English)

```markdown
## What's New

This release brings comprehensive documentation improvements with full bilingual (English/Chinese) support.

### 📚 Documentation Overhaul

#### Bilingual Support
- **English Documentation** (`docs/`)
  - Quick Start Guide
  - Installation Guide  
  - Architecture Overview
  - Performance Optimization Guide
  - FAQ
  - API Reference
  - Examples and Tutorials

- **中文文档** (`docs/`)
  - 快速入门指南
  - 安装指南
  - 架构概览
  - 性能优化指南
  - 常见问题
  - API 参考
  - 示例教程

### 🔧 Enhanced Changelog
- Professional release history structure
- Detailed release notes for all versions
- Technical notes and migration guides

### 📁 Files Changed
- All documentation files restructured
- README.md / README.zh-CN.md improved
- CHANGELOG.md enhanced
- changelog/*.md detailed release logs

### Quick Start
```bash
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

**Full Documentation**: https://lessup.github.io/mini-opencv/

---

## 发布说明 (中文)

```markdown
## 更新内容

本次发布带来全面的文档改进，提供完整的双语（英文/中文）支持。

### 📚 文档重构

#### 双语支持
- **英文文档** (`docs/`)
  - 快速入门指南
  - 安装指南
  - 架构概览
  - 性能优化指南
  - FAQ
  - API 参考
  - 示例教程

- **中文文档** (`docs/`)
  - 快速入门指南
  - 安装指南
  - 架构概览
  - 性能优化指南
  - 常见问题
  - API 参考
  - 示例教程

### 🔧 增强的更新日志
- 专业的版本历史结构
- 所有版本的详细发布说明
- 技术说明和迁移指南

### 📁 变更文件
- 所有文档文件重构
- README.md / README.zh-CN.md 优化
- CHANGELOG.md 增强
- changelog/*.md 详细发布日志

### 快速开始
```bash
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build -DBUILD_EXAMPLES=ON
cmake --build build -j$(nproc)
ctest --test-dir build --output-on-failure
```

**完整文档**: https://lessup.github.io/mini-opencv/
```

## Manual Release Steps

Since automatic release creation failed due to SSL/TLS issues, please create the release manually:

1. Go to: https://github.com/LessUp/mini-opencv/releases/new
2. Tag: `v2.1.0`
3. Title: `v2.1.0 - Documentation Overhaul & Bilingual Support`
4. Copy the release notes above (both English and Chinese)
5. Click "Publish release"

Or use GitHub CLI when network is available:

```bash
gh release create v2.1.0 \
    --title "v2.1.0 - Documentation Overhaul & Bilingual Support" \
    --notes-file RELEASE_v2.1.0.md \
    --latest
```
