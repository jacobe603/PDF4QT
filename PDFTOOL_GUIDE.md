# PdfTool - Command-Line Reference Guide

**Version**: 1.5.2.0
**Application**: PdfTool (Console Application)
**License**: MIT

---

## Table of Contents

1. [Overview](#overview)
2. [Installation and Access](#installation-and-access)
3. [Basic Usage](#basic-usage)
4. [Complete Command Reference](#complete-command-reference)
5. [Common Options](#common-options)
6. [Usage Examples](#usage-examples)
7. [Output Formats](#output-formats)
8. [Exit Codes](#exit-codes)

---

## Overview

**PdfTool** is a powerful command-line application for PDF manipulation and analysis. It provides 30+ specialized operations accessible through a single executable.

### Key Capabilities

- **Document Information**: Extract metadata, structure, fonts, JavaScript
- **Security**: Encrypt, decrypt, verify signatures, certificate management
- **Manipulation**: Merge, split, optimize, redact
- **Analysis**: Compare documents, extract text/images, ink coverage
- **Conversion**: Render to images, create audio books
- **Export**: XML export with full document structure

---

## Installation and Access

### Linux

After installation (via DEB, Flatpak, or AppImage):

```bash
# System installation
PdfTool [command] [options]

# Or direct path
/usr/bin/PdfTool [command] [options]

# Flatpak
flatpak run io.github.JakubMelka.Pdf4qt --command=PdfTool [command] [options]
```

### Windows

After installation via MSI installer or ZIP extract:

```cmd
# If added to PATH
PdfTool.exe [command] [options]

# Or from installation directory
"C:\Program Files\PDF4QT\PdfTool.exe" [command] [options]

# Default installation paths:
# MSI: C:\Program Files\PDF4QT\
# ZIP: [extraction directory]\bin\
```

**Check if installed:**
```cmd
# Windows
where PdfTool.exe

# Or check installation directory
dir "C:\Program Files\PDF4QT\PdfTool.exe"
```

### macOS

```bash
# From application bundle
/Applications/PDF4QT.app/Contents/MacOS/PdfTool [command] [options]
```

---

## Basic Usage

### Command Structure

```bash
PdfTool <command> [options] <arguments>
```

### Getting Help

```bash
# List all available commands
PdfTool help

# Get help for specific command
PdfTool <command> --help

# Version information
PdfTool --version
```

---

## Complete Command Reference

### Document Information

#### `info`
Extract basic document information

```bash
PdfTool info <file.pdf>
```

**Output**: Title, author, subject, keywords, creator, producer, creation date, modification date, PDF version, page count, page sizes, encryption status

**Options**:
- `--compute-hashes` - Calculate SHA-256/MD5 hashes
- `--password <pwd>` - Password for encrypted PDFs

**Example**:
```bash
PdfTool info document.pdf --compute-hashes
```

#### `info-fonts`
List all fonts used in the document

```bash
PdfTool info-fonts <file.pdf>
```

**Output**: Font names, types, encodings, embedded status

**Options**:
- `--show-character-maps` - Show character maps for embedded fonts
- `--pages <range>` - Analyze specific pages (e.g., "1-5,10")

#### `info-metadata`
Extract XMP metadata

```bash
PdfTool info-metadata <file.pdf>
```

**Output**: Raw XMP/XML metadata stream

#### `info-javascript`
Extract embedded JavaScript

```bash
PdfTool info-javascript <file.pdf>
```

**Output**: All JavaScript code from document, forms, and actions

#### `info-inks`
List special inks/separations

```bash
PdfTool info-inks <file.pdf>
```

**Output**: Ink names, types (process/spot colors)

#### `info-pageboxes`
Show page box definitions

```bash
PdfTool info-pageboxes <file.pdf>
```

**Output**: MediaBox, CropBox, BleedBox, TrimBox, ArtBox for each page

#### `info-structure-tree`
Extract document structure tree

```bash
PdfTool info-structure-tree <file.pdf>
```

**Output**: Tagged PDF structure for accessibility

#### `info-named-destinations`
List named destinations (bookmarks)

```bash
PdfTool info-named-destinations <file.pdf>
```

**Output**: All named destinations and their targets

---

### Security Operations

#### `encrypt`
Encrypt a PDF document

```bash
PdfTool encrypt <input.pdf> [options]
```

**Options**:
- `--algorithm <alg>` - Encryption algorithm:
  - `rc4` - RC4 40-bit
  - `rc4-128` - RC4 128-bit
  - `aes-128` - AES 128-bit
  - `aes-256` - AES 256-bit (default, recommended)
- `--user-password <pwd>` - User password (for opening)
- `--owner-password <pwd>` - Owner password (for permissions)
- `--permissions <flags>` - Permission flags (hex or decimal)
- `--encrypt-contents <mode>` - What to encrypt:
  - `all` - Everything (default)
  - `all-except-metadata` - All except metadata
  - `embedded-files-only` - Only embedded files

**Example**:
```bash
PdfTool encrypt input.pdf \
    --algorithm aes-256 \
    --user-password "open123" \
    --owner-password "owner456" \
    --permissions 2820  # Print + copy text
```

#### `decrypt`
Remove encryption from a PDF

```bash
PdfTool decrypt <encrypted.pdf> --password <pwd>
```

**Note**: Requires owner password

**Example**:
```bash
PdfTool decrypt protected.pdf --password "owner456"
```

#### `verify-signatures`
Verify digital signatures

```bash
PdfTool verify-signatures <file.pdf>
```

**Options**:
- `--no-user-certificates` - Don't use user certificate store
- `--no-system-certificates` - Don't use system certificate store
- `--omit-certificate-check` - Skip certificate validity check
- `--print-certificate-details` - Show full certificate info
- `--ignore-expiration-date` - Ignore certificate expiration

**Example**:
```bash
PdfTool verify-signatures signed.pdf --print-certificate-details
```

#### `cert-store`
List certificates in system/user stores

```bash
PdfTool cert-store [options]
```

**Options**:
- `--enumerate-system-certificates` - List system certs
- `--enumerate-user-certificates` - List user certs (default)

#### `cert-store-install`
Install a certificate to user store

```bash
PdfTool cert-store-install --certificate <cert.pem>
```

---

### Document Manipulation

#### `optimize`
Reduce PDF file size

```bash
PdfTool optimize <file.pdf> [--flags <options>]
```

**Optimization Flags**:
- `remove-unused-objects` - Remove unreferenced objects
- `remove-generation-numbers` - Remove object generation numbers
- `remove-null-objects` - Remove null object references
- `shrink-object-storage` - Compact object storage
- `dereference-simple-objects` - Inline simple objects
- `compress-content-streams` - Recompress content streams
- `merge-identical-objects` - Merge duplicate objects
- `remove-unused-pdf-names` - Remove unused name tree entries
- `remove-duplicate-fonts` - Remove duplicate font definitions
- `all` - Apply all optimizations

**Example**:
```bash
PdfTool optimize large.pdf --flags all
```

#### `unite`
Merge multiple PDFs into one

```bash
PdfTool unite <output.pdf> <input1.pdf> <input2.pdf> [...]
```

**Arguments**:
- `<output.pdf>` - Target merged PDF (must not exist)
- `<input1.pdf> <input2.pdf> ...` - Source PDFs to merge (minimum 2 required)

**Note**: Currently does NOT support page selection. To merge specific pages:
1. Use `separate` to extract desired pages first
2. Then `unite` the extracted files

**Example**:
```bash
# Merge entire documents
PdfTool unite merged.pdf file1.pdf file2.pdf file3.pdf

# To merge specific page ranges (two-step process):
# Step 1: Extract pages
PdfTool separate doc1.pdf temp1_%.pdf --page-select "1-5"
PdfTool separate doc2.pdf temp2_%.pdf --page-select "10-15"
# Step 2: Merge extracted pages
PdfTool unite combined.pdf temp1_*.pdf temp2_*.pdf
```

#### `separate`
Split PDF into multiple files

```bash
PdfTool separate <input.pdf> <pattern> [options]
```

**Arguments**:
- `<input.pdf>` - Source PDF file
- `<pattern>` - Output filename pattern (must contain `%` for page number)

**Options**:
- `--page-select <range>` - Pages to extract (e.g., "1-10,15,20-25")
- `--page-first <number>` - First page of range
- `--page-last <number>` - Last page of range

**Pattern Substitution**:
- `%` is replaced with page number (e.g., `page_%.pdf` → `page_5.pdf`)

**Example**:
```bash
# Extract pages 2-5 as separate PDFs
PdfTool separate document.pdf page_%.pdf --page-select "2-5"

# Extract all pages (creates page_1.pdf, page_2.pdf, etc.)
PdfTool separate document.pdf page_%.pdf

# Extract using first/last
PdfTool separate document.pdf page_%.pdf --page-first 10 --page-last 20
```

#### `redact`
Remove sensitive content

```bash
PdfTool redact <input.pdf> --output <output.pdf> [options]
```

**Options**:
- `--scope <scope>` - What to redact:
  - `page-content` - Visible page content
  - `outline` - Document outline/bookmarks
  - `file-attachments` - Attached files
  - `metadata` - XMP metadata
  - `all` - Everything
- `--pages <range>` - Pages to redact

**Example**:
```bash
PdfTool redact sensitive.pdf --output clean.pdf --scope all
```

---

### Analysis and Comparison

#### `diff`
Compare two PDF documents

```bash
PdfTool diff <file1.pdf> <file2.pdf>
```

**Output**: Differences in structure, content, and metadata

**Options**:
- `--pages <range>` - Compare specific pages only
- `--output <format>` - Output format (text, xml, html)

**Example**:
```bash
PdfTool diff version1.pdf version2.pdf --output xml
```

#### `statistics`
Generate document statistics

```bash
PdfTool statistics <file.pdf>
```

**Output**: Object counts, stream statistics, font usage, color space usage

#### `ink-coverage`
Calculate ink coverage for printing

```bash
PdfTool ink-coverage <file.pdf>
```

**Output**: Percentage of page covered by each ink/color

**Options**:
- `--pages <range>` - Analyze specific pages

**Example**:
```bash
PdfTool ink-coverage printfile.pdf --pages "1-10"
```

---

### Content Extraction

#### `fetch-text`
Extract text from PDF

```bash
PdfTool fetch-text <file.pdf> [options]
```

**Options**:
- `--text-analysis-alg <alg>` - Text extraction algorithm:
  - `auto` - Automatic (default)
  - `layout` - Preserve layout
  - `content` - Content stream order
  - `structure` - Use structure tree
- `--page-select <range>` - Extract from specific pages
- `--text-show-page-numbers` - Prefix with page numbers
- `--text-show-struct-title` - Show structure titles
- `--text-show-struct-alt-desc` - Show alternative descriptions
- `--text-show-struct-act-text` - Show actual text from tags

**Example**:
```bash
PdfTool fetch-text document.pdf --text-analysis-alg layout --text-show-page-numbers > output.txt

# Extract from specific pages
PdfTool fetch-text document.pdf --page-select "1-10" --text-analysis-alg layout > output.txt
```

#### `fetch-images`
Extract images from PDF

```bash
PdfTool fetch-images <file.pdf> [options]
```

**Options**:
- `--page-select <range>` - Extract from specific pages
- `--output-directory <dir>` - Where to save images (default: current directory)
- `--image-format <fmt>` - Output format (png, jpg, bmp, tiff)
- `--image-quality <0-100>` - JPEG quality (default: 85)

**Example**:
```bash
PdfTool fetch-images document.pdf \
    --output-directory ./images \
    --image-format png \
    --page-select "1-10"
```

#### `attachments`
Manage file attachments

```bash
PdfTool attachments <file.pdf> [options]
```

**Options**:
- `--save-all` - Extract all attachments
- `--save-number <n>` - Extract attachment by number
- `--save-filename <name>` - Extract attachment by name
- `--output-directory <dir>` - Where to save
- `--target-file <file>` - List attachments from specific embedded file

**Example**:
```bash
PdfTool attachments document.pdf --save-all --output-directory ./attachments
```

---

### Rendering and Export

#### `render-to-images`
Render pages to image files

```bash
PdfTool render-to-images <file.pdf> [options]
```

**Options**:
- `--page-select <range>` - Pages to render
- `--image-export-dir <dir>` - Output directory
- `--image-export-filename <pattern>` - Filename pattern with `%` for page number
- `--image-format <fmt>` - Format (png, jpg, tiff, bmp)
- `--image-export-res-dpi <dpi>` - Resolution (default: 300)
- `--image-quality <0-100>` - JPEG quality
- `--render-msaa-samples <n>` - Anti-aliasing samples (1, 2, 4, 8, 16)

**Example**:
```bash
PdfTool render-to-images document.pdf \
    --page-select "1-5" \
    --image-export-res-dpi 600 \
    --image-format png \
    --image-export-dir ./renders \
    --render-msaa-samples 4
```

#### `create-bitonaldocument`
Convert to black & white

```bash
PdfTool create-bitonaldocument <input.pdf> <output.pdf> [options]
```

**Options**:
- `--pages <range>` - Pages to convert
- `--resolution-dpi <dpi>` - Rendering resolution
- `--threshold <0-255>` - Binarization threshold (default: 128)

**Example**:
```bash
PdfTool create-bitonaldocument color.pdf bw.pdf --threshold 140
```

#### `image-to-pdf`
Convert images to PDF

```bash
PdfTool image-to-pdf <output.pdf> <image1.png> [image2.jpg ...] [options]
```

**Options**:
- `--page-size <size>` - Page size (A4, Letter, etc.)
- `--page-margins <mm>` - Margins in millimeters
- `--image-quality <0-100>` - JPEG quality for compression

**Example**:
```bash
PdfTool image-to-pdf output.pdf img1.png img2.jpg img3.tiff --page-size A4
```

---

### Advanced Features

#### `audio-book`
Convert PDF to audio book

```bash
PdfTool audio-book <input.pdf> [options]
```

**Options**:
- `--output-directory <dir>` - Where to save audio files
- `--audio-format <fmt>` - Format (mp3, wav, flac, etc.)
- `--voice-name <name>` - TTS voice name
- `--voice-gender <gender>` - Voice gender (male, female, neutral)
- `--voice-age <age>` - Voice age (child, adult, senior)
- `--voice-lang <code>` - Language code (en, de, fr, etc.)
- `--say-page-numbers` - Announce page numbers
- `--say-struct-titles` - Read structure titles
- `--mark-page-numbers` - Add markers for page numbers

**Example**:
```bash
PdfTool audio-book textbook.pdf \
    --output-directory ./audiobook \
    --audio-format mp3 \
    --voice-lang en \
    --say-page-numbers
```

#### `xml`
Export to XML

```bash
PdfTool xml <file.pdf>
```

**Options**:
- `--export-streams` - Include stream content
- `--export-streams-as-text` - Try to export streams as text
- `--use-indent` - Pretty-print XML
- `--always-binary-strings` - Force binary encoding for strings

**Example**:
```bash
PdfTool xml document.pdf --use-indent --export-streams > output.xml
```

#### `color-profiles`
List embedded color profiles

```bash
PdfTool color-profiles <file.pdf>
```

**Output**: ICC color profile information

---

## Common Options

### Document Opening

```bash
--document <file.pdf>      # Input PDF file
--password <pwd>           # Password for encrypted PDFs
--permissive-reading       # Try to read damaged PDFs (default: on)
```

### Page Selection

```bash
--pages <range>            # Page range specification
```

**Range Format**:
- `5` - Single page
- `1-10` - Range
- `1,3,5` - Specific pages
- `1-10,15,20-25` - Combined

**Examples**:
- `--pages "1-5"` - First 5 pages
- `--pages "1,10,20"` - Pages 1, 10, and 20
- `--pages "5-"` - From page 5 to end

### Output Formatting

```bash
--output-style <style>     # Output format
```

**Styles**:
- `text` - Plain text (default)
- `xml` - XML format
- `html` - HTML format

**Example**:
```bash
PdfTool info document.pdf --output-style xml > output.xml
```

### Output Encoding

```bash
--output-codec <encoding>  # Character encoding
```

**Common Encodings**:
- `UTF-8` (default)
- `UTF-16`
- `ISO-8859-1` (Latin-1)
- `Windows-1252`

### Date Formatting

```bash
--output-date-format <fmt>
```

**Formats**:
- `locale-short` - Locale short format (default)
- `locale-long` - Locale long format
- `iso` - ISO 8601 format
- `rfc2822` - RFC 2822 format

---

## Usage Examples

### Example 1: Extract Document Info

```bash
# Basic info
PdfTool info document.pdf

# With file hashes
PdfTool info document.pdf --compute-hashes

# XML output for parsing
PdfTool info document.pdf --output-style xml > info.xml
```

### Example 2: Optimize Large PDF

```bash
# Apply all optimizations
PdfTool optimize large.pdf --flags all

# Specific optimizations
PdfTool optimize document.pdf \
    --flags "remove-unused-objects,compress-content-streams,merge-identical-objects"
```

### Example 3: Extract and Merge Specific Pages

```bash
# Extract pages from multiple documents, then merge
# Step 1: Extract desired pages
PdfTool separate doc1.pdf doc1_page_%.pdf --page-select "1-5"
PdfTool separate doc2.pdf doc2_page_%.pdf --page-select "10-15"

# Step 2: Merge extracted pages
PdfTool unite combined.pdf doc1_page_*.pdf doc2_page_*.pdf

# Or for a single document - extract and merge range:
PdfTool separate book.pdf chapter_%.pdf --page-select "10-50"
PdfTool unite chapter.pdf chapter_*.pdf
```

### Example 4: Extract and Convert Text

```bash
# Extract preserving layout
PdfTool fetch-text document.pdf --algorithm layout > output.txt

# Extract with page numbers
PdfTool fetch-text document.pdf --text-show-page-numbers --algorithm layout > numbered.txt

# Extract from specific pages
PdfTool fetch-text large.pdf --page-select "1-100" --algorithm layout > first100.txt
```

### Example 5: Encrypt with Permissions

```bash
# Allow printing and copying, prevent editing
PdfTool encrypt document.pdf \
    --algorithm aes-256 \
    --user-password "user123" \
    --owner-password "owner456" \
    --permissions 2820

# Encrypt metadata too
PdfTool encrypt document.pdf \
    --algorithm aes-256 \
    --user-password "pass" \
    --encrypt-contents all-except-metadata
```

### Example 6: High-Quality Page Rendering

```bash
# Render at print quality (600 DPI)
PdfTool render-to-images presentation.pdf \
    --image-export-res-dpi 600 \
    --image-format png \
    --render-msaa-samples 8 \
    --image-export-dir ./high-res \
    --page-select "1-20"

# Render for web (150 DPI, JPEG)
PdfTool render-to-images document.pdf \
    --image-export-res-dpi 150 \
    --image-format jpg \
    --image-quality 85 \
    --image-export-dir ./web
```

### Example 7: Batch Processing Script

**Linux/macOS** (`process-pdfs.sh`):
```bash
#!/bin/bash
for pdf in *.pdf; do
    echo "Processing $pdf..."
    PdfTool optimize "$pdf" --flags all
    PdfTool info "$pdf" > "${pdf%.pdf}_info.txt"
    PdfTool fetch-text "$pdf" --algorithm layout > "${pdf%.pdf}_text.txt"
done
```

**Windows** (`process-pdfs.bat`):
```batch
@echo off
for %%f in (*.pdf) do (
    echo Processing %%f...
    PdfTool.exe optimize "%%f" --flags all
    PdfTool.exe info "%%f" > "%%~nf_info.txt"
    PdfTool.exe fetch-text "%%f" --algorithm layout > "%%~nf_text.txt"
)
```

### Example 8: Security Workflow

```bash
# 1. Check signatures
PdfTool verify-signatures signed.pdf --print-certificate-details

# 2. Remove encryption (if authorized)
PdfTool decrypt protected.pdf --password "owner_pass"

# 3. Re-encrypt with new settings
PdfTool encrypt protected.pdf \
    --algorithm aes-256 \
    --user-password "new_user" \
    --owner-password "new_owner"
```

---

## Output Formats

### Text Format (Default)

```
Properties:
Property        Value
---------------------------------
Title          Sample Document
Author         John Doe
Pages          42
PDF Version    2.0
```

### XML Format

```xml
<?xml version="1.0"?>
<info>
  <properties>
    <property name="Title">Sample Document</property>
    <property name="Author">John Doe</property>
    <property name="Pages">42</property>
    <property name="PDFVersion">2.0</property>
  </properties>
</info>
```

### HTML Format

```html
<html>
<head><title>Info</title></head>
<body>
  <table>
    <tr><th>Property</th><th>Value</th></tr>
    <tr><td>Title</td><td>Sample Document</td></tr>
    <tr><td>Author</td><td>John Doe</td></tr>
    <tr><td>Pages</td><td>42</td></tr>
  </table>
</body>
</html>
```

---

## Exit Codes

| Code | Name | Description |
|------|------|-------------|
| 0 | ExitSuccess | Operation successful |
| 1 | ExitFailure | General failure |
| 2 | ErrorUnknown | Unknown error |
| 3 | ErrorNoDocumentSpecified | No input document provided |
| 4 | ErrorDocumentReading | Failed to read/parse PDF |
| 5 | ErrorDocumentWriting | Failed to write PDF |
| 6 | ErrorCertificateReading | Certificate read error |
| 7 | ErrorInvalidArguments | Invalid command-line arguments |
| 8 | ErrorFailedWriteToFile | File write failed |
| 9 | ErrorPermissions | Insufficient permissions |
| 10 | ErrorNoText | No text found in document |
| 11 | ErrorCOM | COM initialization error (Windows) |
| 12 | ErrorSAPI | Speech API error (Windows) |
| 13 | ErrorEncryptionSettings | Invalid encryption settings |

### Checking Exit Codes

**Linux/macOS**:
```bash
PdfTool info document.pdf
if [ $? -eq 0 ]; then
    echo "Success"
else
    echo "Failed with code $?"
fi
```

**Windows**:
```batch
PdfTool.exe info document.pdf
if %ERRORLEVEL% EQU 0 (
    echo Success
) else (
    echo Failed with code %ERRORLEVEL%
)
```

---

## Quick Reference Card

| Task | Command |
|------|---------|
| **Get help** | `PdfTool help` or `PdfTool <command> --help` |
| **Document info** | `PdfTool info document.pdf` |
| **Extract pages 2-5 (separate)** | `PdfTool separate doc.pdf page_%.pdf --page-select "2-5"` |
| **Extract all pages** | `PdfTool separate doc.pdf page_%.pdf` |
| **Merge PDFs** | `PdfTool unite output.pdf doc1.pdf doc2.pdf doc3.pdf` |
| **Optimize file** | `PdfTool optimize large.pdf --flags all` |
| **Extract text** | `PdfTool fetch-text doc.pdf > output.txt` |
| **Extract images** | `PdfTool fetch-images doc.pdf --output-directory ./images` |
| **Render to PNG** | `PdfTool render-to-images doc.pdf --image-export-dir ./out` |
| **Encrypt** | `PdfTool encrypt doc.pdf --algorithm aes-256 --user-password "pass"` |
| **Decrypt** | `PdfTool decrypt doc.pdf --password "pass"` |
| **Verify signatures** | `PdfTool verify-signatures signed.pdf` |
| **Compare docs** | `PdfTool diff doc1.pdf doc2.pdf` |

---

## Tips and Best Practices

### 1. Always Backup Originals

```bash
# Create backup before modifying
cp important.pdf important.pdf.backup
PdfTool optimize important.pdf --flags all
```

### 2. Use --help for Command Details

```bash
# Get all options for a command
PdfTool separate --help
PdfTool render-to-images --help
```

### 3. Pipe Output to Files

```bash
# Save output
PdfTool info document.pdf > info.txt 2> errors.txt

# XML output for processing
PdfTool info document.pdf --output-style xml | xmllint --format -
```

### 4. Test with Small Page Ranges First

```bash
# Test on first page before processing all pages
PdfTool render-to-images huge.pdf --page-select "1" --image-export-res-dpi 600
```

### 5. Combine with Other Tools

```bash
# Extract text and count words
PdfTool fetch-text document.pdf | wc -w

# Find specific content
PdfTool fetch-text document.pdf | grep -i "important"

# Convert extracted images
PdfTool fetch-images doc.pdf --output-directory ./temp
mogrify -format jpg ./temp/*.png
```

---

## Version Information

This guide is for **PdfTool version 1.5.2.0**.

Get version:
```bash
PdfTool --version
```

---

**Last Updated**: 2025-11-14
**For**: PDF4QT v1.5.2.0
