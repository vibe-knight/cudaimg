# CLAUDE.md - Mini-OpenCV Project Configuration

> **Full project guidelines**: See [AGENTS.md](./AGENTS.md) for comprehensive development standards.

## Project Overview

**Mini-OpenCV** is a CUDA-based high-performance image processing library (v2.1.0).

## Tech Stack

| Component | Version |
|-----------|---------|
| C++ | 17 |
| CUDA | 14+ |
| CMake | 3.18+ |
| GoogleTest | FetchContent |

## Quick Commands

```bash
# Configure
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build -j$(nproc)

# Test
ctest --test-dir build --output-on-failure

# Format check
clang-format-14 --dry-run --Werror -i <file>
```

## Architecture

```
┌─────────────────────────────────────────────────┐
│              Application Layer                   │
│  (gpu_image.hpp - unified high-level API)       │
├─────────────────────────────────────────────────┤
│              Operator Layer                      │
│  (color_convert, convolution, morphology, etc.)  │
├─────────────────────────────────────────────────┤
│           Infrastructure Layer                   │
│  (device_buffer, memory_manager, cuda_error)    │
└─────────────────────────────────────────────────┘
```

## Code Style

- **Naming**: snake_case for functions/variables, PascalCase for types
- **Indentation**: 2 spaces, 80 column limit
- **CUDA Kernels**: Always check return values, use `GPU_IMAGE_CUDA_CHECK`

## OpenSpec Workflow

1. `/opsx:explore` - Understand current state
2. `/opsx:propose` - Create change proposal
3. `/opsx:apply` - Implement changes
4. `/opsx:archive` - Archive completed work

## Language

- Code comments: English
- User documentation: Chinese + English (bilingual)
- Commit messages: Conventional Commits (English)

---

For detailed guidelines on CUDA programming, testing strategies, and architecture decisions, see **[AGENTS.md](./AGENTS.md)**.
