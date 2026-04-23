# Database/Data Model Specifications

This directory contains data model and schema definitions for the mini-opencv library.

## Purpose

This project is primarily a computation library and does not use a traditional database. However, this directory documents:

- Image data structures (`GpuImage`, `HostImage`)
- Memory layout specifications
- Configuration data models

## Data Models

### Core Image Types

| Type | Location | Purpose |
|------|----------|---------|
| `GpuImage` | GPU Memory | Device-side image container |
| `HostImage` | CPU Memory | Host-side image container |

### Memory Layout

Images are stored in **interleaved format**:

```
RGB Image (width=4, height=3, channels=3):

Memory: [R0 G0 B0 R1 G1 B1 R2 G2 B2 R3 G3 B3]
        [R4 G4 B4 R5 G5 B5 R6 G6 B6 R7 G7 B7]
        [R8 G8 B8 R9 G9 B9 R10 G10 B10 R11 G11 B11]

Stride: row_stride = width * channels (padded to alignment)
```

### Configuration Types

| Type | Purpose |
|------|---------|
| `ProcessorConfig` | Runtime configuration for ImageProcessor |

## Related Documents

- [RFC 0001: Design Document](../rfc/0001-gpu-image-processing-design.md) - Contains detailed data model definitions
