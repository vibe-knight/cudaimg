#!/usr/bin/env node

/**
 * Sync CHANGELOG.md from root to docs
 *
 * This script:
 * 1. Reads the root CHANGELOG.md
 * 2. Removes HTML comments
 * 3. Converts heading format: ## [0.1.0] - 2024-01-01 → ## 0.1.0 (2024-01-01)
 * 4. Removes sub-headings (### Added, ### Changed, etc.)
 * 5. Outputs to docs/en/changelog.md and docs/zh/changelog.md
 */

import { readFileSync, writeFileSync, existsSync } from 'fs'
import { resolve, dirname } from 'path'
import { fileURLToPath } from 'url'

const __dirname = dirname(fileURLToPath(import.meta.url))
const rootDir = resolve(__dirname, '..', '..')
const changelogPath = resolve(rootDir, 'CHANGELOG.md')

function processChangelog(content) {
  // Remove HTML comments
  let processed = content.replace(/<!--[\s\S]*?-->/g, '')

  // Convert heading format: ## [0.1.0] - 2024-01-01 → ## 0.1.0 (2024-01-01)
  processed = processed.replace(
    /^## \[([^\]]+)\] - (\d{4}-\d{2}-\d{2})$/gm,
    '## $1 ($2)'
  )

  // Remove sub-headings (### Added, ### Changed, etc.)
  processed = processed.replace(/^###\s+.*$/gm, '')

  // Remove multiple blank lines
  processed = processed.replace(/\n{3,}/g, '\n\n')

  return processed.trim()
}

function addFrontmatter(content, lang = 'en') {
  const titles = {
    en: 'Changelog',
    zh: '更新日志'
  }

  return `---
title: ${titles[lang]}
---

${content}
`
}

function main() {
  if (!existsSync(changelogPath)) {
    console.error('CHANGELOG.md not found at:', changelogPath)
    process.exit(1)
  }

  const content = readFileSync(changelogPath, 'utf-8')
  const processed = processChangelog(content)

  // Write English version
  const enPath = resolve(rootDir, 'docs', 'en', 'changelog.md')
  writeFileSync(enPath, addFrontmatter(processed, 'en'))
  console.log('Written:', enPath)

  // Write Chinese version (same content, different frontmatter)
  const zhPath = resolve(rootDir, 'docs', 'zh', 'changelog.md')
  writeFileSync(zhPath, addFrontmatter(processed, 'zh'))
  console.log('Written:', zhPath)

  console.log('Changelog sync complete!')
}

main()
