#!/bin/bash
# Create GitHub Release Script

VERSION="v2.1.0"
TITLE="v2.1.0 - Documentation Overhaul & Bilingual Support"

BODY=$(cat << 'EOF'
## What's New

This release brings comprehensive documentation improvements with full bilingual (English/Chinese) support.

### Documentation
- Professional bilingual documentation (EN/ZH)
- Restructured docs with guides and API reference
- Enhanced changelog with detailed release history

### Files Changed
- All docs now available in English and Chinese
- Improved README.md and README.zh-CN.md
- Professional changelog structure

### Quick Start
```bash
git clone https://github.com/LessUp/mini-opencv.git
cd mini-opencv
cmake -S . -B build
cmake --build build -j$(nproc)
```

**Full Documentation**: https://lessup.github.io/mini-opencv/
EOF
)

# Create release using curl
curl -X POST \
  -H "Authorization: token $GH_TOKEN" \
  -H "Accept: application/vnd.github.v3+json" \
  https://api.github.com/repos/LessUp/mini-opencv/releases \
  -d "{
    \"tag_name\": \"$VERSION\",
    \"name\": \"$TITLE\",
    \"body\": $(echo "$BODY" | jq -Rs .),
    \"draft\": false,
    \"prerelease\": false
  }" 2>&1
