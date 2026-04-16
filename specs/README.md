# Specifications

This directory contains all specification documents for the mini-opencv project.
Specs are the **Single Source of Truth** for development.

## Directory Structure

| Directory | Purpose |
|-----------|---------|
| `product/` | Product requirements and feature definitions (PRD) |
| `rfc/` | Technical design documents and architecture proposals |
| `api/` | API interface definitions (OpenAPI, function signatures) |
| `db/` | Data model and schema definitions |
| `testing/` | Test specifications and BDD feature files |

## Active Specifications

### Core Library

| Spec | Type | Status |
|------|------|--------|
| [GPU Image Processing Requirements](product/gpu-image-processing-requirements.md) | Product | ✅ Implemented |
| [GPU Image Processing Design](rfc/0001-gpu-image-processing-design.md) | RFC | ✅ Implemented |
| [GPU Image Processing Tasks](rfc/0001-gpu-image-processing-tasks.md) | RFC | ✅ Implemented |

## How to Use Specs

1. **Before coding**: Read relevant specs in this directory
2. **When adding features**: Update or create specs first
3. **When fixing bugs**: Check specs for expected behavior
4. **When reviewing**: Verify code matches spec definitions

## Spec Format

All spec documents should follow this structure:

```markdown
# Spec Title

## Overview
Brief description of what this spec covers

## Requirements/Design/Definition
Main content with code examples, diagrams, etc.

## Acceptance Criteria
Clear, testable conditions for completion

## Related Documents
Links to related specs or documentation
```
