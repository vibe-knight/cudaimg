#!/usr/bin/env node
/**
 * Sync CHANGELOG.md to docs/{zh,en}/changelog.md
 *
 * This script copies the content from the root CHANGELOG.md to the docs site,
 * with formatting changes for both Chinese and English versions.
 *
 * Run from the docs directory: node scripts/sync-changelog.mjs
 */

import { readFileSync, writeFileSync, existsSync } from "fs";
import { dirname, join } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const docsDir = join(__dirname, "..");
const rootDir = join(docsDir, "..");

const sourcePath = join(rootDir, "CHANGELOG.md");

if (!existsSync(sourcePath)) {
  console.log("No CHANGELOG.md found, skipping sync");
  process.exit(0);
}

const ZH_HEADER = `# 更新日志

本页记录 Mini-OpenCV 每个版本的变更内容。

`;

const EN_HEADER = `# Changelog

This page documents the changes in each Mini-OpenCV release.

`;

// Read the source file
let content = readFileSync(sourcePath, "utf-8");

// Remove the HTML comment block at the top
content = content.replace(/<!--[\s\S]*?-->\n*/g, "");

// Remove the "# Changelog" title (we'll add our own header)
content = content.replace(/^#.*Changelog.*\n+/i, "");

// Convert title format: ## [2.1.0] - 2024-01-15 -> ## 2.1.0 (2024-01-15)
content = content.replace(
  /^## \[([^\]]+)\] - (\d{4}-\d{1,2}-\d{1,2})/gm,
  "## $1 ($2)"
);

// Remove subsection headers like ### Added, ### Changed, ### Fixed
content = content.replace(/^### (Added|Changed|Fixed|Improved|Breaking|Removed|Deprecated)\n+/gm, "");

// Write the Chinese version
const zhTargetPath = join(docsDir, "zh/changelog.md");
writeFileSync(zhTargetPath, ZH_HEADER + content.trim() + "\n");
console.log(`Synced changelog to ${zhTargetPath}`);

// Write the English version
const enTargetPath = join(docsDir, "en/changelog.md");
writeFileSync(enTargetPath, EN_HEADER + content.trim() + "\n");
console.log(`Synced changelog to ${enTargetPath}`);
