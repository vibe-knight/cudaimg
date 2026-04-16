# Release v1.1.0 — Workflow Optimization

**Release Date:** 2026-03-09  
**Focus:** Initial CI/CD infrastructure setup

---

## Overview

First standardized CI workflow implementation with automated build validation and code formatting checks.

---

## ✨ New Features

| Feature | Description |
|---------|-------------|
| CI Workflow | `.github/workflows/ci.yml` |
| Triggers | `push`, `pull_request`, `workflow_dispatch` |
| Build | CUDA container-based validation |
| Format | `clang-format` check job |

---

## Technical Details

- **Container:** `nvidia/cuda:12.4.1-devel-ubuntu22.04`
- **Parallel Jobs:** Build + Format check
- **Execution:** Automatic on PR and push

---

## Resources

- [CHANGELOG.md v1.1.0](../CHANGELOG.md)
