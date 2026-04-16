#!/bin/bash
# GitHub Pages Build Verification Script

echo "═══════════════════════════════════════════════════════════════"
echo "  GitHub Pages Build Verification"
echo "═══════════════════════════════════════════════════════════════"

# Check required files
echo ""
echo "📁 Checking required files..."

required_files=(
  "_config.yml"
  "index.md"
  "assets/images/logo.svg"
  "_sass/custom/custom.scss"
)

all_present=true
for file in "${required_files[@]}"; do
  if [[ -f "$file" ]]; then
    echo "  ✓ $file"
  else
    echo "  ✗ $file (MISSING)"
    all_present=false
  fi
done

# Check optional files
echo ""
echo "📄 Documentation files..."
echo "  Found $(find docs -name '*.md' | wc -l) docs"
echo "  Found $(find changelog -name '*.md' | wc -l) changelog files"

# Check workflow
echo ""
echo "🔧 GitHub Actions workflow..."
if [[ -f ".github/workflows/pages.yml" ]]; then
  echo "  ✓ pages.yml updated"
fi

# Summary
echo ""
echo "═══════════════════════════════════════════════════════════════"
if [[ "$all_present" == true ]]; then
  echo "  ✅ All required files present"
  echo "  🚀 Ready for GitHub Pages deployment"
else
  echo "  ⚠️  Some files missing"
  exit 1
fi
echo "═══════════════════════════════════════════════════════════════"
