# PDF4QT - AI Assistant Guide

**Version**: 1.5.2.0
**License**: MIT License (changed from LGPLv3 in April 2025)
**Author**: Jakub Melka (c) 2018-2025
**Language**: C++20

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Repository Structure](#repository-structure)
3. [Architecture and Design](#architecture-and-design)
4. [Build System and Dependencies](#build-system-and-dependencies)
5. [Coding Conventions](#coding-conventions)
6. [Development Workflows](#development-workflows)
7. [Testing](#testing)
8. [Key Components Reference](#key-components-reference)
9. [Common Development Tasks](#common-development-tasks)
10. [AI Assistant Guidelines](#ai-assistant-guidelines)

---

## Project Overview

PDF4QT is a comprehensive PDF library and application suite implementing PDF functionality based on PDF Reference 2.0. The project consists of:

- **Core PDF Library**: Full-featured PDF parsing, rendering, and manipulation
- **GUI Applications**: Viewer, Editor, Diff tool, PageMaster
- **Command-line Tools**: PdfTool for batch operations
- **Plugin System**: Extensible architecture for additional functionality

### Key Features

- Multithreading support with Blend2D rendering engine
- Encryption, digital signatures, and public key security
- Color management system (LittleCMS)
- Text layout analysis and text-to-speech
- Annotations, form filling, and document editing
- PDF optimization and compression
- Document comparison and internal structure inspection
- Static XFA support (read-only)

### Platforms

- Windows (Visual Studio 2022, MinGW)
- Linux (GCC 11+)
- Cross-platform via Qt 6.9+

---

## Repository Structure

### Core Libraries (Foundation Layer)

```
Pdf4QtLibCore/          Core PDF engine (NO Qt Widgets dependency)
├── sources/            79 headers, 68 source files
│   ├── pdf*.h/cpp      All PDF-related classes
│   ├── cmaps/          CMap resources
│   └── fonts/          Embedded Liberation fonts
└── CMakeLists.txt

Pdf4QtLibWidgets/       Widget-based UI components
├── sources/            24 source files + UI forms
│   ├── pdfadvancedtools.*
│   ├── pdfwidgettool.*
│   └── *.ui            Qt Designer forms
└── CMakeLists.txt

Pdf4QtLibGui/           High-level GUI components
├── pdfviewermainwindow.*
├── pdfeditormainwindow.*
├── pdfprogramcontroller.*
└── *.ui                Extensive UI forms
```

### Applications

```
Pdf4QtViewer/           Standalone PDF viewer
Pdf4QtEditor/           PDF editor with plugin support
Pdf4QtDiff/             PDF comparison tool (8 source files)
Pdf4QtPageMaster/       Page manipulation tool (9 source files)
PdfTool/                Command-line tool (26+ specialized operations)
Pdf4QtLaunchPad/        Application launcher hub
```

### Plugin System

```
Pdf4QtEditorPlugins/
├── SignaturePlugin/           Digital and electronic signatures
├── ObjectInspectorPlugin/     Internal PDF object inspection
├── DimensionsPlugin/          Measurement tools
├── OutputPreviewPlugin/       Print preview
├── RedactPlugin/              Content redaction
├── AudioBookPlugin/           Audio book conversion
├── SoftProofingPlugin/        Color proofing
└── EditorPlugin/              General editing tools
```

### Supporting Components

```
CodeGenerator/          Code generation from XML definitions
UnitTests/              Qt Test framework tests
JBIG2_Viewer/           JBIG2 image format viewer
PdfExampleGenerator/    Sample PDF generator for testing
vcpkg/                  Package manager overlays
translations/           i18n support (en, de, cs, es, ko, zh_CN, fr, tr, ru)
```

---

## Architecture and Design

### Dependency Graph

```
┌─────────────────────────────────────────────┐
│  Applications (Viewer, Editor, Diff, etc.)  │
├─────────────────────────────────────────────┤
│            Pdf4QtLibGui                     │
│  (Windows, Dialogs, Controllers)            │
├─────────────────────────────────────────────┤
│          Pdf4QtLibWidgets                   │
│  (Widgets, Tools, UI Components)            │
├─────────────────────────────────────────────┤
│           Pdf4QtLibCore                     │
│  (PDF Engine, Parser, Renderer)             │
├─────────────────────────────────────────────┤
│  Third-party Libraries                      │
│  (Qt, OpenSSL, Blend2D, etc.)              │
└─────────────────────────────────────────────┘

       Plugin System (Side-loading)
```

### Design Patterns Used

1. **Decorator Pattern**: `PDFDocumentDataLoaderDecorator` wraps `PDFObjectStorage`
2. **Visitor Pattern**: `PDFAbstractVisitor` for traversing PDF object trees
3. **Factory Pattern**: `PDFDocumentBuilder`, stream filter factories
4. **Plugin Pattern**: Qt plugin architecture (`Q_PLUGIN_METADATA`)
5. **Strategy Pattern**: Rendering engines (Blend2D vs QPainter), execution policies
6. **Builder Pattern**: `PDFDocumentBuilder`, `PDFPageContentEditorContentStreamBuilder`
7. **Observer Pattern**: Qt signal-slot mechanism throughout

### Key Class Organization (Pdf4QtLibCore)

**Document Model**:
- `pdfobject.h/cpp` - Base object representation
- `pdfdocument.h/cpp` - Document container
- `pdfstorage` - `PDFObjectStorage` for object management
- `pdfcatalog.h/cpp` - Document catalog
- `pdfpage.h/cpp` - Page representation

**Parsing & I/O**:
- `pdfparser.h/cpp` - PDF file parser
- `pdfdocumentreader.h/cpp` - Document loading
- `pdfdocumentwriter.h/cpp` - Document saving
- `pdfxreftable.h/cpp` - Cross-reference table

**Rendering Pipeline**:
- `pdfrenderer.h/cpp` - Main renderer
- `pdfblpainter.h/cpp` - Blend2D backend
- `pdfpagecontentprocessor.h/cpp` - Content stream processing
- `pdftransparencyrenderer.h/cpp` - Transparency handling

**Security**:
- `pdfsecurityhandler.h/cpp` - Security handlers
- `pdfsignaturehandler.h/cpp` - Digital signatures
- `pdfcertificatemanager.h/cpp` - Certificate handling

**Resources**:
- `pdffont.h/cpp` - Font handling (113KB source)
- `pdfcolorspaces.h/cpp` - Color spaces (105KB source)
- `pdfimage.h/cpp` - Image handling
- `pdfpattern.h/cpp` - Pattern support (143KB source)

**Features**:
- `pdfannotation.h/cpp` - Annotations (131KB header, 62KB source)
- `pdfform.h/cpp` - Interactive forms
- `pdfaction.h/cpp` - Actions
- `pdfoutline.h/cpp` - Bookmarks

---

## Build System and Dependencies

### CMake Configuration

- **CMake Version**: 3.16+
- **C++ Standard**: C++20 (required)
- **Qt Version**: 6.9+ (tested with 6.9.0)
- **Project Version**: 1.5.2.0

### Required Dependencies (via vcpkg)

```json
{
  "dependencies": [
    "tbb",           // Threading Building Blocks (Linux/GCC only)
    "openssl",       // Encryption and signatures
    "lcms",          // LittleCMS color management
    "zlib",          // Compression
    "openjpeg",      // JPEG2000 support
    "freetype",      // Font rendering
    "libjpeg-turbo", // JPEG support
    "libpng",        // PNG support
    "blend2d"        // High-performance 2D rendering
  ]
}
```

### Qt Modules Required

- **Core Libraries**: `Qt6::Core`, `Qt6::Gui`, `Qt6::Xml`, `Qt6::Svg`
- **Widgets**: `Qt6::Widgets`
- **Additional**: `Qt6::PrintSupport`, `Qt6::TextToSpeech`, `Qt6::Test`

### Important CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `PDF4QT_BUILD_ONLY_CORE_LIBRARY` | OFF | Build only core library |
| `PDF4QT_INSTALL_DEPENDENCIES` | `VCPKG_TOOLCHAIN` | Install dependent libraries |
| `PDF4QT_INSTALL_QT_DEPENDENCIES` | `VCPKG_TOOLCHAIN` | Install Qt dependencies |
| `PDF4QT_INSTALL_INCLUDE` | ON | Install public headers |
| `PDF4QT_FLATPAK_BUILD` | OFF | Special Flatpak build flag |
| `PDF4QT_INSTALL_TO_USR` | `VCPKG_TOOLCHAIN` | Install to /usr |
| `PDF4QT_INSTALL_MSVC_REDISTRIBUTABLE` | ON (Win) | Include MSVC redist |
| `PDF4QT_INSTALL_PREPARE_WIX_INSTALLER` | ON (Win) | Prepare .msi installer |

### Important CMake Variables

| Variable | Description |
|----------|-------------|
| `PDF4QT_QT_ROOT` | Qt installation directory (required for dependency install) |
| `CMAKE_TOOLCHAIN_FILE` | Should point to vcpkg toolchain |
| `VCPKG_OVERLAY_PORTS` | Set to prevent crashes with incompatible libpng on Linux |
| `CMAKE_BUILD_TYPE` | Release or Debug |

### Build Process

```bash
# 1. Setup vcpkg
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh -disableMetrics
export VCPKG_ROOT=$(pwd)/vcpkg

# 2. Clone repository
git clone https://github.com/JakubMelka/PDF4QT
cd PDF4QT

# 3. Configure (Linux example)
cmake -B build -S . \
  -DPDF4QT_INSTALL_QT_DEPENDENCIES=0 \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_INSTALL_PREFIX='/' \
  -DCMAKE_BUILD_TYPE=Release \
  -DVCPKG_OVERLAY_PORTS=PDF4QT/vcpkg/overlays

# 4. Build (use -j for parallel build)
cmake --build build -j6

# 5. Install
sudo cmake --install build

# Uninstall
sudo xargs rm < ./build/install_manifest.txt
```

### Compiler Flags

**MSVC**:
```cmake
/bigobj        # Large object files
/W4            # Warning level 4
/wd5054        # Disable specific warnings
/wd4127
/wd4702
```

**MinGW**:
```cmake
-Wa,-mbig-obj  # Big object files
```

### CMake Automation

```cmake
CMAKE_AUTOMOC ON    # Automatic Qt MOC
CMAKE_AUTOUIC ON    # Automatic Qt UIC for .ui files
CMAKE_AUTORCC ON    # Automatic Qt RCC for .qrc files
CMAKE_AUTORCC_OPTIONS "--threshold;0;--compress;9"  # Maximum compression
```

---

## Coding Conventions

### File Naming

**Strict lowercase pattern**:
- Headers: `pdf*.h` (e.g., `pdfobject.h`, `pdfdocument.h`)
- Sources: `pdf*.cpp` (matching header names)
- UI Forms: `*.ui` (e.g., `pdfviewermainwindow.ui`)
- Resources: `*.qrc` (e.g., `cmaps.qrc`, `fonts.qrc`)
- Plugins: `*plugin.h/cpp` (e.g., `signatureplugin.h`)

### Namespace Convention

```cpp
namespace pdf {          // Primary namespace
namespace pdfplugin {    // Plugin namespace
namespace pdfviewer {    // Viewer namespace
```

### Class Naming

- **All PDF classes**: `PDF` prefix
- **Core types**: `PDFInteger`, `PDFReal`, `PDFColorComponent`
- **References**: `PDFObjectReference`
- **Pattern**: PascalCase after `PDF` prefix

### Type System

```cpp
// Strong typing (pdfglobal.h)
using PDFInteger = int64_t;
using PDFReal = double;
using PDFColorComponent = float;
using PDFGray = PDFColorComponent;
using PDFRGB = std::array<PDFColorComponent, 3>;
using PDFCMYK = std::array<PDFColorComponent, 4>;

// Constants
constexpr PDFInteger PDF_INTEGER_MIN = std::numeric_limits<int64_t>::min() / 100;
constexpr PDFInteger PDF_INTEGER_MAX = std::numeric_limits<int64_t>::max() / 100;
constexpr PDFReal PDF_EPSILON = 0.000001;
```

### Header Structure

```cpp
// MIT License header (22 lines)
// Copyright (c) 2018-2025 Jakub Melka and Contributors
// ... MIT License text ...

#ifndef PDFGLOBAL_H
#define PDFGLOBAL_H

// Qt includes first
#include <QtSystemDetection>
#include <QCoreApplication>

// Standard library includes
#include <limits>
#include <array>

// Project includes
#include <pdf4qtlibcore_export.h>

namespace pdf {
    // Implementation
}

#endif // PDFGLOBAL_H
```

### Memory Management

```cpp
// Smart pointers preferred
using PDFDocumentPointer = QSharedPointer<PDFDocument>;
std::unique_ptr<ContentObject> content;

// Raw pointers for Qt object tree ownership
QWidget* widget = new QWidget(parent); // parent owns

// Optimization patterns
PDFInplaceString        // Small string optimization (no heap allocation)
PDFCachedItem<T>        // Lazy evaluation cache
optimize() methods      // Manual memory optimization
```

### Code Style

```cpp
// Constexpr for compile-time evaluation
constexpr inline PDFObjectReference() :
    objectNumber(0),
    generation(0)
{
}

// Default operators when possible (C++20)
inline bool operator==(const PDFInplaceOrMemoryString&) const = default;

// Explicit constructors for single-argument
explicit PDFInplaceOrMemoryString(QByteArray string);

// Inline for small functions in headers
inline bool isInplace() const { return std::holds_alternative<PDFInplaceString>(m_value); }

// Virtual destructors for polymorphic classes
virtual ~PDFObjectContent() = default;

// Override keyword for virtual functions
void optimize() override;
```

### Qt Conventions

```cpp
// No emit macro (defined globally)
add_compile_definitions(QT_NO_EMIT)
// Use: signalName() instead of Q_EMIT signalName()

// Qt property system
Q_OBJECT
Q_PROPERTY(...)
signals:
    void documentChanged();
public slots:
    void onUpdate();
```

### Export Macros

```cpp
// Library visibility control
PDF4QTLIBCORESHARED_EXPORT      // Pdf4QtLibCore
PDF4QTLIBWIDGETSSHARED_EXPORT   // Pdf4QtLibWidgets
PDF4QTLIBGUILIBSHARED_EXPORT    // Pdf4QtLibGui

// Usage
class PDF4QTLIBCORESHARED_EXPORT PDFObject { ... };
```

---

## Development Workflows

### Git Workflow

**Main Branch**: `master`

**Commit Message Style** (from recent commits):
```
Issue #XXX: Short description

Issue #XXX: Feature description [Tag, Tag]
```

Examples:
- `Issue #333: ijg-libjpeg obsoleted in vcpkg`
- `Issue #251: Let user lock direction of lines/polylines when pressing keybinding (e.g. shift)`
- `Update CMake`
- `Flatpak fix`

### Issue-Driven Development

All major changes reference GitHub issues. Check RELEASES.txt and recent commits to understand the issue-tracking pattern.

### CI/CD Pipeline

**GitHub Actions**: `.github/workflows/ci.yml`

**Ubuntu Build**:
- Qt 6.9.0
- vcpkg dependencies cached
- Builds DEB package
- Runs translation generation

**Windows Build**:
- Visual Studio 2022
- Qt 6.9.0 (win64_msvc2022_64)
- Generates MSI installer (Wix)
- Includes MSVC redistributables

**Build Commands**:
```bash
# Ubuntu
cmake -B build -S . \
  -DPDF4QT_INSTALL_QT_DEPENDENCIES=0 \
  -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --target all release_translations -j6
cmake --install build

# Windows
cmake -B build -S . \
  -DCMAKE_BUILD_TYPE=Release \
  -DPDF4QT_INSTALL_QT_DEPENDENCIES=ON \
  -DPDF4QT_INSTALL_DEPENDENCIES=ON \
  -DPDF4QT_INSTALL_MSVC_REDISTRIBUTABLE=ON \
  -DPDF4QT_INSTALL_PREPARE_WIX_INSTALLER=ON

cmake --build build --target release_translations -j6
cmake --build build -j6
cmake --install build
```

### Translation Workflow

**Supported Languages**: en, de, cs, es, ko, zh_CN, fr, tr, ru

**Translation System**:
```cmake
qt_add_translations(
    TARGETS Pdf4QtEditor Pdf4QtViewer Pdf4QtPageMaster Pdf4QtDiff PdfTool
    MERGE_QT_TRANSLATIONS
    TS_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/translations
    QM_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/translations
)
```

**Files**:
- Source: `translations/*.ts` (XML)
- Compiled: `build/bin/translations/*.qm` (binary)

### Release Process

See `Release_Process.txt` for official release procedures.

**Version Locations**:
- `CMakeLists.txt`: Line 25: `set(PDF4QT_VERSION 1.5.2.0)`
- `vcpkg.json`: `version-string: "1.5.2"`
- `version.txt.in`: Template for version file

---

## Testing

### Unit Tests

**Location**: `UnitTests/`

**Framework**: Qt Test (`Qt6::Test`)

**Main Test**: `tst_lexicalanalyzertest.cpp` (52KB)

**Running Tests**:
```bash
cd build
ctest
# or
./UnitTests/tst_lexicalanalyzertest
```

### Example Generator

**Location**: `PdfExampleGenerator/`

Generates sample PDF documents for manual testing and validation.

### Manual Testing Applications

- **Pdf4QtViewer**: Test viewing, navigation, rendering
- **Pdf4QtEditor**: Test editing, annotations, forms
- **Pdf4QtDiff**: Test document comparison
- **PdfTool**: Test command-line operations

---

## Key Components Reference

### Pdf4QtLibCore Major Classes

**Document Management**:
- `PDFDocument` - Main document class
- `PDFObjectStorage` - Object storage and management
- `PDFCatalog` - Document catalog
- `PDFPage` - Individual page

**Parsing**:
- `PDFParser` - Low-level PDF parser
- `PDFDocumentReader` - High-level document loading
- `PDFLexicalAnalyzer` - Tokenization

**Rendering**:
- `PDFRenderer` - Abstract renderer interface
- `PDFBLPainter` - Blend2D implementation
- `PDFPageContentProcessor` - Content stream interpreter
- `PDFTransparencyRenderer` - Transparency groups

**Security**:
- `PDFSecurityHandler` - Encryption/decryption
- `PDFSignatureHandler` - Digital signatures
- `PDFCertificateManager` - Certificate management

**Content**:
- `PDFAnnotation` - Annotation support (largest file: 131KB header)
- `PDFForm` - Interactive forms
- `PDFAction` - Actions (links, etc.)
- `PDFOutline` - Document outline (bookmarks)

**Resources**:
- `PDFFont` - Font handling
- `PDFColorSpaces` - Color space conversions
- `PDFImage` - Image decoding
- `PDFPattern` - Tiling and shading patterns

**Utilities**:
- `PDFUtils` - Utility functions
- `PDFExecutionPolicy` - Threading control
- `PDFCachedItem<T>` - Lazy evaluation

### Pdf4QtLibWidgets Major Classes

- `PDFAdvancedTools` - Drawing and editing tools
- `PDFWidgetTool` - Base widget tool class
- `PDFWidgetFormManager` - Form interaction
- `PDFPageContentEditorWidget` - Content editing widget
- `PDFDrawSpaceController` - Drawing space management

### Pdf4QtLibGui Major Classes

- `PDFViewerMainWindow` - Viewer main window
- `PDFEditorMainWindow` - Editor main window
- `PDFProgramController` - Application controller
- `PDFSidebarWidget` - Sidebar with thumbnails/bookmarks
- `PDFUndoRedoManager` - Undo/redo functionality
- `PDFBookmarkManager` - Bookmark management
- `PDFTextToSpeech` - Text-to-speech support

### PdfTool Commands

26+ specialized operations including:
- `encrypt`, `decrypt` - Security operations
- `optimize` - Document optimization
- `unite`, `separate` - Page operations
- `info` - Document information extraction
- `render-to-images` - Rendering to image files
- `create-bitonaldocument` - Black and white conversion
- `image-to-pdf` - Image to PDF conversion
- `sign` - Digital signature creation
- `verify-signature` - Signature validation
- `redact` - Content redaction

---

## Common Development Tasks

### Adding a New Feature to LibCore

1. **Create header and source files**:
   ```bash
   cd Pdf4QtLibCore/sources
   touch pdfnewfeature.h pdfnewfeature.cpp
   ```

2. **Follow naming conventions**:
   - Class: `PDFNewFeature`
   - Namespace: `pdf`
   - File names: lowercase `pdfnewfeature.*`

3. **Add to CMakeLists.txt**:
   ```cmake
   set(SOURCES
       sources/pdfnewfeature.cpp
       # ... other sources
   )
   set(HEADERS
       sources/pdfnewfeature.h
       # ... other headers
   )
   ```

4. **Add export macro if public API**:
   ```cpp
   class PDF4QTLIBCORESHARED_EXPORT PDFNewFeature { ... };
   ```

5. **Write unit tests** in `UnitTests/`

### Adding a New Plugin

1. **Create plugin directory**:
   ```bash
   cd Pdf4QtEditorPlugins
   mkdir NewFeaturePlugin
   cd NewFeaturePlugin
   ```

2. **Create plugin files**:
   - `newfeatureplugin.h` - Header
   - `newfeatureplugin.cpp` - Implementation
   - `newfeatureplugin.json` - Metadata
   - `NewFeaturePlugin.pro` or add to CMakeLists.txt

3. **Inherit from PDFPlugin**:
   ```cpp
   class NewFeaturePlugin : public QObject, public pdf::PDFPlugin
   {
       Q_OBJECT
       Q_PLUGIN_METADATA(IID "PDF4QT.NewFeaturePlugin" FILE "newfeatureplugin.json")

   private:
       Q_INTERFACES(pdf::PDFPlugin)
   };
   ```

4. **Add to parent CMakeLists.txt**:
   ```cmake
   add_subdirectory(NewFeaturePlugin)
   ```

### Adding a Translation

1. **Add language code to CMakeLists.txt** (line 70):
   ```cmake
   qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES en de cs es ko zh_CN fr tr ru new_lang)
   ```

2. **Run lupdate** to generate .ts files:
   ```bash
   cmake --build build --target update_translations
   ```

3. **Translate** using Qt Linguist:
   ```bash
   linguist translations/PDF4QT_new_lang.ts
   ```

4. **Compile** translations:
   ```bash
   cmake --build build --target release_translations
   ```

### Fixing a Bug

1. **Find the issue on GitHub** and reference it
2. **Locate relevant code** using grep/IDE search
3. **Make minimal changes** to fix the issue
4. **Test** with appropriate application
5. **Commit** with message: `Issue #XXX: Description`

### Optimizing Performance

**Check**:
- `PDFExecutionPolicy` - Ensure multithreading is used
- Rendering engine - Blend2D_MultiThread is fastest
- `optimize()` methods - Call on large objects
- `PDFCachedItem<T>` - Use for expensive computations

### Adding a New UI Dialog

1. **Create .ui file** in Qt Designer
2. **Add to CMakeLists.txt** (AUTOUIC will handle it)
3. **Create corresponding .h/.cpp**:
   ```cpp
   #include "ui_mydialog.h"

   class MyDialog : public QDialog
   {
       Q_OBJECT
   public:
       explicit MyDialog(QWidget* parent = nullptr);
   private:
       Ui::MyDialog ui;
   };
   ```

4. **In constructor**:
   ```cpp
   MyDialog::MyDialog(QWidget* parent) : QDialog(parent)
   {
       ui.setupUi(this);
   }
   ```

---

## AI Assistant Guidelines

### When Working with This Codebase

1. **Always use the `pdf` namespace** for core functionality
2. **Follow the strict file naming convention**: `pdf*.h/cpp` in lowercase
3. **Use export macros** for public API classes
4. **Prefer Qt types** where appropriate: `QString`, `QByteArray`, etc.
5. **Use strong typing**: `PDFInteger`, `PDFReal`, not raw types
6. **Reference issues** in commit messages when fixing bugs
7. **Add MIT license header** to all new files (see templates in existing files)
8. **Use C++20 features**: concepts, ranges, constexpr, default comparisons
9. **Respect the layer architecture**: Core → Widgets → Gui → Apps
10. **Never add Qt Widgets dependency to LibCore**

### Common Pitfalls to Avoid

1. **Don't mix naming conventions** - stay consistent with lowercase `pdf*` pattern
2. **Don't forget export macros** - classes won't be visible across library boundaries
3. **Don't break layer dependencies** - LibCore must not depend on LibWidgets/LibGui
4. **Don't use `emit` keyword** - it's disabled via `QT_NO_EMIT`
5. **Don't hardcode paths** - use `CMAKE_INSTALL_*` variables
6. **Don't forget VCPKG_OVERLAY_PORTS** on Linux (libpng compatibility)
7. **Don't modify generated files** - they're regenerated from XML

### Code Review Checklist

- [ ] MIT license header present
- [ ] Follows naming conventions (lowercase files, PDF prefix classes)
- [ ] Export macros used for public APIs
- [ ] No Qt Widgets in LibCore
- [ ] Uses strong typing (`PDFInteger`, not `int64_t`)
- [ ] Memory management correct (smart pointers preferred)
- [ ] Thread-safe where needed
- [ ] Documented with comments for complex logic
- [ ] Translatable strings use `tr()` or `QCoreApplication::translate()`
- [ ] CMakeLists.txt updated if new files added
- [ ] Follows Qt signal/slot conventions
- [ ] No `emit` keyword used

### Understanding the Codebase

**Start here**:
1. `pdfglobal.h` - Core types and constants
2. `pdfobject.h` - Object model
3. `pdfdocument.h` - Document structure
4. `pdfrenderer.h` - Rendering pipeline
5. `pdfviewermainwindow.h` - Application structure

**Key patterns**:
- `PDFDocumentDataLoaderDecorator` - Safe data extraction
- `PDFCachedItem<T>` - Lazy evaluation
- `PDFExecutionPolicy` - Threading control
- Plugin base: `pdf::PDFPlugin`

### PDF 2.0 Conformance

See `NOTES.txt` for detailed conformance notes. Many features are implemented, but some have known limitations (e.g., halftoning, multimedia, linearized PDF).

### Resources

- **Official Website**: https://jakubmelka.github.io/
- **GitHub**: https://github.com/JakubMelka/PDF4QT
- **Issue Tracker**: https://github.com/JakubMelka/PDF4QT/issues
- **Sponsorship**: https://github.com/sponsors/JakubMelka

---

## Quick Reference

### File Locations

| Component | Path |
|-----------|------|
| Core library | `Pdf4QtLibCore/sources/` |
| Widgets library | `Pdf4QtLibWidgets/sources/` |
| GUI library | `Pdf4QtLibGui/` |
| Plugins | `Pdf4QtEditorPlugins/*/` |
| Translations | `translations/*.ts` |
| Build config | `CMakeLists.txt` |
| Dependencies | `vcpkg.json` |
| CI/CD | `.github/workflows/ci.yml` |

### Build Commands Quick Reference

```bash
# Configure
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build -j6

# Install
sudo cmake --install build

# Uninstall
sudo xargs rm < build/install_manifest.txt

# Translations
cmake --build build --target release_translations

# Clean
rm -rf build
```

### Important Constants

```cpp
PDF_INTEGER_MIN    // Minimum safe integer
PDF_INTEGER_MAX    // Maximum safe integer
PDF_EPSILON        // Floating point comparison tolerance (0.000001)
```

---

**Last Updated**: 2025-11-14
**For**: PDF4QT v1.5.2.0
