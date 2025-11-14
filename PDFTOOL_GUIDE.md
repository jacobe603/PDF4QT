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

**Options**:
- `--pages <range>` - Select pages from each input (e.g., "1-10")

**Example**:
```bash
PdfTool unite merged.pdf file1.pdf file2.pdf file3.pdf
```

#### `separate`
Split PDF into multiple files

```bash
PdfTool separate <input.pdf> [--pattern <pattern>]
```

**Options**:
- `--pattern <pattern>` - Output filename pattern (e.g., "page_%PageNo%.pdf")
- `--pages <range>` - Pages to extract

**Pattern Variables**:
- `%PageNo%` - Page number
- `%FileName%` - Original filename

**Example**:
```bash
PdfTool separate document.pdf --pattern "page_%PageNo%.pdf"
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
PdfTool fetch-text <file.pdf>
```

**Options**:
- `--algorithm <alg>` - Text extraction algorithm:
  - `auto` - Automatic (default)
  - `layout` - Preserve layout
  - `content` - Content stream order
  - `structure` - Use structure tree
- `--pages <range>` - Extract from specific pages
- `--show-page-numbers` - Prefix with page numbers
- `--show-struct-titles` - Show structure titles
- `--show-alt-description` - Show alternative descriptions
- `--show-actual-text` - Show actual text from tags

**Example**:
```bash
PdfTool fetch-text document.pdf --algorithm layout --show-page-numbers > output.txt
```

#### `fetch-images`
Extract images from PDF

```bash
PdfTool fetch-images <file.pdf> [options]
```

**Options**:
- `--pages <range>` - Extract from specific pages
- `--output-directory <dir>` - Where to save images
- `--image-format <fmt>` - Output format (png, jpg, bmp, etc.)
- `--image-quality <0-100>` - JPEG quality (default: 85)

**Example**:
```bash
PdfTool fetch-images document.pdf \
    --output-directory ./images \
    --image-format png \
    --pages "1-10"
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
- `--pages <range>` - Pages to render
- `--output-directory <dir>` - Output directory
- `--output-filename <pattern>` - Filename pattern (default: "page_%PageNo%.png")
- `--image-format <fmt>` - Format (png, jpg, tiff, etc.)
- `--resolution-dpi <dpi>` - Resolution (default: 300)
- `--image-quality <0-100>` - JPEG quality
- `--msaa-samples <n>` - Anti-aliasing samples (1, 2, 4, 8, 16)
- `--render-flags <flags>` - Rendering features to enable/disable

**Render Flags**:
- `antialiasing`, `text-antialiasing`, `smooth-pictures`
- `ignore-optional-content`, `clip-to-crop-box`
- `display-annotations`, `display-times`

**Example**:
```bash
PdfTool render-to-images document.pdf \
    --pages "1-5" \
    --resolution-dpi 600 \
    --image-format png \
    --output-directory ./renders \
    --msaa-samples 4
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

### Example 3: Merge PDFs with Page Selection

```bash
# Merge first 10 pages of each
PdfTool unite output.pdf file1.pdf file2.pdf file3.pdf --pages "1-10"

# Merge specific pages
PdfTool unite combined.pdf \
    doc1.pdf --pages "1-5" \
    doc2.pdf --pages "10-20" \
    doc3.pdf --pages "1,5,10"
```

### Example 4: Extract and Convert Text

```bash
# Extract preserving layout
PdfTool fetch-text document.pdf --algorithm layout > output.txt

# Extract with page numbers
PdfTool fetch-text document.pdf --show-page-numbers --algorithm layout > numbered.txt

# Extract from specific pages
PdfTool fetch-text large.pdf --pages "1-100" --algorithm layout > first100.txt
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
    --resolution-dpi 600 \
    --image-format png \
    --msaa-samples 8 \
    --output-directory ./high-res \
    --pages "1-20"

# Render for web (150 DPI, JPEG)
PdfTool render-to-images document.pdf \
    --resolution-dpi 150 \
    --image-format jpg \
    --image-quality 85 \
    --output-directory ./web
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
PdfTool render-to-images huge.pdf --pages "1" --resolution-dpi 600
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
