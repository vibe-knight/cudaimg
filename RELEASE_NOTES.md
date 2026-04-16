# Mini-OpenCV v2.1.0 Release Notes

## 🎉 What's New

This release brings comprehensive documentation improvements with full bilingual (English/Chinese) support and enhanced project structure.

---

## 📚 Documentation Overhaul

### New Structure
- **English Documentation** (`docs/`)
  - Quick Start Guide
  - Installation Guide
  - Architecture Overview
  - Performance Optimization Guide
  - FAQ
  - API Reference
  - Examples

- **中文文档** (`docs/`)
  - 快速入门指南
  - 安装指南
  - 架构概览
  - 性能优化指南
  - 常见问题
  - API 参考
  - 示例代码

### Key Improvements
- Professional documentation structure
- Comprehensive bilingual support
- Enhanced changelog with detailed release history
- Improved navigation and cross-references

---

## 🔧 Enhanced Changelog

### Professional Release History
- Detailed release notes for v1.0.0 through v2.0.0
- Categorized changes (Features, Fixes, Changes)
- Technical notes for each release
- Migration guides where applicable

---

## 📋 Files Changed

### Documentation
- `docs/README.md` - Documentation homepage (EN)
- `docs/README.zh-CN.md` - 文档主页 (中文)
- `docs/quickstart.md` / `quickstart.zh-CN.md`
- `docs/installation.md` / `installation.zh-CN.md`
- `docs/architecture.md` / `architecture.zh-CN.md`
- `docs/performance.md` / `performance.zh-CN.md`
- `docs/faq.md` / `faq.zh-CN.md`
- `docs/examples/` - Example tutorials
- `docs/api/` - API reference

### Project
- `README.md` - Enhanced with better structure
- `README.zh-CN.md` - Synchronized improvements
- `CHANGELOG.md` - Professional format
- `changelog/*.md` - Detailed release logs

---

## 🚀 Quick Start

```bash
# Clone
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv

# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Test
ctest --test-dir build --output-on-failure
```

---

## 📖 Resources

- **Documentation**: https://lessup.github.io/mini-opencv/
- **Issues**: https://github.com/LessUp/mini-opencv/issues
- **Discussions**: https://github.com/LessUp/mini-opencv/discussions

---

## 🙏 Credits

Thanks to all contributors who helped improve the documentation and project structure.

---

*Release Date: 2026-04-16*
