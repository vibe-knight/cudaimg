# Security Policy

## Supported Versions

| Version | Supported |
| ------- | --------- |
| 2.0.x   | ✅ |
| 1.x.x   | ⚠️ (security fixes only) |
| < 1.0   | ❌ |

## Reporting a Vulnerability

If you discover a security vulnerability in this project, please report it by:

1. **Do not** open a public issue for security vulnerabilities
2. Email the maintainer directly or use GitHub's private vulnerability reporting feature
3. Include:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if available)

We will respond to security reports within 48 hours and aim to release fixes within 7 days for critical vulnerabilities.

## Security Considerations

This library:
- Does not process untrusted input by default
- Does not have network connectivity
- Runs on the local GPU device only
- Uses standard CUDA runtime APIs

Potential security concerns:
- Image file parsing (via stb library) - ensure image files are from trusted sources
- GPU memory operations - no data leaves the GPU except through explicit download APIs
