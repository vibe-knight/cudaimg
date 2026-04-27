# Release v2.0.0 — Correctness and CI Hardening

**Release Date:** 2026-03-24  
**Focus:** Deterministic correctness fixes with minimal changes while hardening CI validation

---

## Overview

This release focuses on reliability improvements for geometric transforms, concurrent convolution safety, parameter validation, and CI verification enhancement. All changes are backward-compatible.

---

## 🐛 Bug Fixes

### Geometric Transform Boundary Handling

**Issue:** Bilinear sampling at image boundaries produced incorrect results due to coordinate zeroing.

**Fix:** Corrected boundary coordinate calculations in `geometric.cu`.

**Impact:** More accurate rotation, affine, and perspective transforms near image edges.

**Tests Added:**
- Identity transform regression test
- Boundary pixel accuracy validation

---

### Convolution Multi-Stream Safety

**Issue:** Global shared constant kernels could cause data pollution in multi-stream scenarios.

**Fix:** Removed dependency on global shared constant kernels in convolution implementation.

**Impact:** Safer concurrent convolution across multiple CUDA streams.

**Tests Added:**
- Concurrent convolution regression test
- Multi-stream correctness validation

---

### Filter Parameter Validation

**Issue:** Gaussian blur and bilateral filter accepted invalid sigma values (≤ 0).

**Fix:** Added parameter validation with `std::invalid_argument` exceptions.

```cpp
if (sigma <= 0.0f) {
    throw std::invalid_argument("Sigma must be positive");
}
```

**Tests Added:**
- Exception tests for invalid sigma values
- Boundary parameter validation

---

## ✨ New Features

### Regression Test Suite

| Test | Coverage |
|------|----------|
| Geometric Transform | Identity, rotation, affine transforms |
| Convolution | Single and multi-stream scenarios |
| Parameter Validation | Invalid input rejection |

### Repository-Level Formatting

- Added `.clang-format` for consistent code style
- Integrated format check in CI pipeline

---

## 🔧 Changed

### Language Standard

| Component | Before | After |
|-----------|--------|-------|
| CUDA | C++17 | C++14 compatible |
| Removed | C++17-only features | NVCC-compatible alternatives |

**Rationale:** Maximum compiler compatibility while maintaining functionality.

---

## 📊 CI Improvements

### Enhanced Test Output

```yaml
- name: Test
  run: ctest --output-on-failure
```

**Benefit:** Immediate visibility into test failures.

### Coverage Reporting

- Updated test coverage descriptions in README files
- Added benchmark command documentation

---

## Migration Guide

### No Breaking Changes

v2.0.0 is fully backward-compatible with v1.x.

### Recommended Actions

1. **Upgrade:** Simply pull the new version
2. **Validate:** Run the test suite
3. **Review:** Check parameter validation in your code

---

## Performance Impact

| Metric | Change | Note |
|--------|--------|------|
| Correctness | Improved | Better boundary handling |
| Safety | Improved | Multi-stream reliability |
| Performance | Neutral | No runtime overhead |

---

## Contributors

This release includes contributions from the core development team focused on correctness and reliability.

---

## Full Changelog

See [CHANGELOG.md](../CHANGELOG.md) for complete version history.
