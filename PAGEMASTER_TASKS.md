# PageMaster Spec Section Extraction - Task Tracker

**Project**: PDF4QT PageMaster Specification Section Extraction Feature
**Started**: 2025-11-14
**Last Updated**: 2025-11-14
**Status**: Phase 1 - Planning Complete, Ready to Implement

---

## Project Overview

### Goal
Add a feature to PDF4QT PageMaster that can:
1. Search for labeled specification sections (e.g., "23 8100", "237142") in PDF documents
2. Detect consecutive page ranges containing matching results
3. Extract those ranges to individual PDF files named after the spec sections
4. Support batch processing of multiple sections
5. Auto-detect spec sections in documents

### Use Case Example
User has a 500-page specification document containing multiple sections:
- Section "23 8100" appears on pages 12-25
- Section "237142" appears on pages 30-45
- Section "23 9000" appears on pages 67-89

**Desired Workflow**:
1. User opens spec PDF in PageMaster
2. Either:
   - **Manual**: User enters "23 8100" → System finds pages 12-25 → Extract to "23_8100.pdf"
   - **Batch**: User enters list of 10 sections → System finds 7 → User selects 5 → Extracts all 5
   - **Auto**: System scans PDF → Finds 15 sections → User selects which to extract

---

## Implementation Phases

### Phase 1: Basic Text Search ⚙️ IN PROGRESS
**Status**: In Progress
**Goal**: Implement foundational text search capability in PageMaster

#### Tasks
- [ ] 1.1 Design text search UI for PageMaster
- [ ] 1.2 Implement search dialog with input field and results display
- [x] 1.3 Add search operation to MainWindow enum and menu
- [ ] 1.4 Implement searchText method in PageItemModel
- [ ] 1.5 Connect search dialog to document search functionality
- [ ] 1.6 Test basic text search with sample PDFs

#### Technical Notes
- Leverage existing `PDFDocumentTextFlowFactory` from Pdf4QtLibCore
- Use `PDFTextLayoutStorage::find()` for search functionality
- Results should show: matched text, page number, context
- Search should work across all loaded documents in PageMaster

#### Deliverable
User can search for any text (e.g., "23 8100") and see all pages where it appears.

---

### Phase 2: Single Spec Section Extraction
**Status**: Not Started
**Goal**: Extract ONE spec section at a time

#### Tasks
- [ ] 2.1 Create spec section pattern matching (e.g., '23 8100', '237142')
- [ ] 2.2 Implement consecutive page range detection algorithm
- [ ] 2.3 Add UI for single spec section input and search
- [ ] 2.4 Display found page ranges to user for confirmation
- [ ] 2.5 Implement page extraction using PDFDocumentManipulator
- [ ] 2.6 Auto-generate output filename from spec section name
- [ ] 2.7 Test single section extraction end-to-end

#### Technical Notes
**Consecutive Page Range Detection**:
- Input: Pages where "23 8100" appears: [5, 6, 7, 10, 11, 15]
- Output: Ranges: [[5-7], [10-11], [15-15]]
- Algorithm:
  ```
  sort pages
  for each page:
    if page == previous_page + 1:
      extend current range
    else:
      start new range
  ```

**Pattern Matching**:
- Support formats: "23 8100", "237142", "23-8100", "23_8100"
- Normalize to standard format for filename generation

**Extraction**:
- Use `PDFDocumentManipulator::assemble()` to create new PDF
- Generate filename: "23_8100.pdf" from "23 8100"

#### Deliverable
User enters "23 8100" → System finds pages 12-25 → Extracts to "23_8100.pdf"

---

### Phase 3: Multiple User-Defined Sections
**Status**: Not Started
**Goal**: Batch process multiple spec sections

#### Tasks
- [ ] 3.1 Create UI for managing list of spec sections to search
- [ ] 3.2 Implement batch search for all defined sections
- [ ] 3.3 Display all found sections with page ranges in results table
- [ ] 3.4 Allow user to select which sections to extract
- [ ] 3.5 Implement batch extraction with progress indicator
- [ ] 3.6 Test batch extraction with multiple sections

#### Technical Notes
**UI Design**:
```
┌─────────────────────────────────────────┐
│ Spec Sections to Extract               │
├─────────────────────────────────────────┤
│ [Add] [Remove] [Clear]                  │
│                                         │
│ Sections:                               │
│ - 23 8100                               │
│ - 237142                                │
│ - 23 9000                               │
│                                         │
│         [Search All]                    │
└─────────────────────────────────────────┘

After search:
┌─────────────────────────────────────────┐
│ Search Results                          │
├──────────┬────────────┬────────┬────────┤
│ [✓] Sec  │ Page Range │ Pages  │ Status │
├──────────┼────────────┼────────┼────────┤
│ [✓] 23   │ 12-25      │ 14     │ Found  │
│ [✓] 2371 │ 30-45      │ 16     │ Found  │
│ [✓] 23 9 │ 67-89      │ 23     │ Found  │
│ [ ] 23 1 │ -          │ 0      │ Not F. │
└──────────┴────────────┴────────┴────────┘
         [Extract Selected (3)]
```

**Progress Indicator**:
- Show: "Extracting 3 of 5 sections..."
- Progress bar
- Cancel button

#### Deliverable
User inputs 10 spec sections → System finds 7 → User selects 5 → Extracts all 5 in one operation

---

### Phase 4: Automatic Spec Section Detection
**Status**: Not Started
**Goal**: Automatically discover spec sections in PDF

#### Tasks
- [ ] 4.1 Implement regex pattern for common spec section formats
- [ ] 4.2 Scan entire PDF to find all matching spec sections
- [ ] 4.3 Remove duplicates and sort detected sections
- [ ] 4.4 Display detected sections in selectable list UI
- [ ] 4.5 Integrate auto-detection with extraction workflow
- [ ] 4.6 Test auto-detection with real specification documents

#### Technical Notes
**Regex Patterns**:
```cpp
// Pattern 1: Two digits, space, four digits
QRegularExpression("\\b\\d{2}\\s\\d{4}\\b")  // Matches: "23 8100"

// Pattern 2: Six consecutive digits
QRegularExpression("\\b\\d{6}\\b")           // Matches: "237142"

// Pattern 3: Two digits, dash, four digits
QRegularExpression("\\b\\d{2}-\\d{4}\\b")    // Matches: "23-8100"

// Pattern 4: Two digits, underscore, four digits
QRegularExpression("\\b\\d{2}_\\d{4}\\b")    // Matches: "23_8100"
```

**De-duplication**:
- If "23 8100" appears 50 times across pages 12-25, show it once in results
- Use `std::set<QString>` to track unique sections

**Sorting**:
- Numeric sort: "23 0100" < "23 8100" < "23 9000"
- Not alphabetic: "23 8100" < "237142" (numeric interpretation)

#### Deliverable
User opens spec PDF → Clicks "Auto-Detect" → System shows "Found 15 sections: 23 8100, 23 8200, ..." → User selects 8 → Extracts

---

### Phase 5: Polish & Advanced Features
**Status**: Not Started
**Goal**: Quality of life improvements

#### Tasks
- [ ] 5.1 Add settings for custom spec section patterns
- [ ] 5.2 Implement save/load of spec section lists
- [ ] 5.3 Add OCR support as fallback for scanned documents
- [ ] 5.4 Create comprehensive user documentation

#### Technical Notes
**Custom Patterns**:
- Allow user to define regex patterns in settings
- Store patterns in QSettings
- Example: User adds pattern for "Division 23, Section 8100"

**Save/Load Lists**:
- JSON format:
  ```json
  {
    "sections": ["23 8100", "237142", "23 9000"],
    "name": "HVAC Sections"
  }
  ```
- File extension: `.pmspec` (PageMaster Spec)

**OCR Support**:
- Detect if PDF is scanned (no text layer)
- Offer OCR using existing PDF4QT capabilities
- Warning: "Document appears to be scanned. OCR recommended for accurate search."

#### Deliverable
Full-featured, production-ready spec extraction tool with all edge cases handled

---

## Technical Architecture

### Key Classes to Modify/Create

#### Existing Classes (Modify)
1. **mainwindow.h/cpp** (Pdf4QtPageMaster/)
   - Add `Operation::SearchText` enum value
   - Add `Operation::ExtractSpecSection` enum value
   - Add menu items: "Search..." and "Extract Spec Sections..."
   - Implement handlers: `performOperation(SearchText)`, etc.

2. **pageitemmodel.h/cpp** (Pdf4QtPageMaster/)
   - Add method: `searchText(QString, bool caseSensitive)`
   - Add method: `extractPageRange(int docIndex, PDFInteger firstPage, PDFInteger lastPage, QString outputPath)`
   - Access existing `m_documents` map for searching

#### New Classes (Create)
1. **searchdialog.h/cpp** (Pdf4QtPageMaster/)
   - Text input field
   - Search button
   - Results list (page numbers, context preview)
   - Case-sensitive checkbox

2. **searchdialog.ui** (Qt Designer)
   - Visual layout for search dialog

3. **specextractdialog.h/cpp** (Pdf4QtPageMaster/)
   - Spec section input/management
   - Search results table
   - Extract button with progress

4. **specextractdialog.ui** (Qt Designer)
   - Visual layout for spec extraction dialog

### Core PDF4QT APIs to Use

From **Pdf4QtLibCore**:

1. **Text Extraction**:
   ```cpp
   pdf::PDFDocumentTextFlowFactory factory;
   pdf::PDFDocumentTextFlow textFlow = factory.create(
       &document,
       pdf::PDFDocumentTextFlowFactory::Algorithm::Layout
   );
   QString allText = textFlow.getText();
   ```

2. **Text Search**:
   ```cpp
   pdf::PDFTextLayoutStorage storage(document.getCatalog()->getPageCount());
   // ... populate storage ...
   pdf::PDFFindResults results = storage.find(
       "23 8100",
       Qt::CaseInsensitive,
       pdf::PDFTextFlow::None
   );

   for (const auto& result : results) {
       qDebug() << "Found on page:" << result.textSelectionItems.first().first.pageIndex;
       qDebug() << "Matched:" << result.matched;
       qDebug() << "Context:" << result.context;
   }
   ```

3. **Page Extraction**:
   ```cpp
   pdf::PDFDocumentManipulator manipulator;

   // Create assembled page for extraction
   pdf::PDFDocumentManipulator::AssembledPage page;
   page.documentIndex = 0;
   page.pageIndex = 12; // Page 13 (0-indexed)
   page.pageRotation = pdf::PageRotation::None;

   std::vector<pdf::PDFDocumentManipulator::AssembledPage> pages;
   pages.push_back(page);
   // ... add more pages 13-25 ...

   // Assemble new document
   pdf::PDFDocument extracted = manipulator.assemble({pages});

   // Save
   pdf::PDFDocumentWriter writer;
   writer.write("23_8100.pdf", &extracted);
   ```

### File Structure
```
Pdf4QtPageMaster/
├── main.cpp                          (existing)
├── mainwindow.h/cpp                  (modify - add operations)
├── mainwindow.ui                     (modify - add menu items)
├── pageitemmodel.h/cpp               (modify - add search/extract methods)
├── pageitemdelegate.h/cpp            (existing - no changes)
├── searchdialog.h/cpp                (NEW - Phase 1)
├── searchdialog.ui                   (NEW - Phase 1)
├── specextractdialog.h/cpp           (NEW - Phase 2)
├── specextractdialog.ui              (NEW - Phase 2)
├── CMakeLists.txt                    (modify - add new files)
└── resources.qrc                     (existing)
```

---

## Development Strategy

### Incremental Development
- Build one phase at a time
- Test each phase thoroughly before moving to next
- Each phase delivers a working feature (no dead code)
- Can ship after any phase

### Testing Plan
1. **Unit Testing**: Test search algorithms with known inputs
2. **Integration Testing**: Test with real spec PDFs
3. **Edge Cases**:
   - Empty PDF
   - PDF with no text layer (scanned)
   - Spec section appears only once (single page)
   - Spec section spans entire document
   - Multiple documents loaded simultaneously

### Code Review Checklist
- [ ] Follows PDF4QT naming conventions (`pdf*` lowercase files)
- [ ] MIT license header on all new files
- [ ] Uses Qt types appropriately (QString, etc.)
- [ ] Memory management correct (parent ownership for Qt widgets)
- [ ] Thread-safe where needed
- [ ] Translatable strings use `tr()`
- [ ] CMakeLists.txt updated for new files

---

## Session Notes

### Session 1 (2025-11-14)
**What We Did**:
- Analyzed PDF4QT codebase structure
- Created comprehensive CLAUDE.md documentation
- Created PDFTOOL_GUIDE.md for CLI tools
- Explored PageMaster architecture and existing text search APIs
- Designed 5-phase implementation plan
- Created this task tracking document

**Key Discoveries**:
- PDF4QT has robust text search via `PDFTextLayoutStorage::find()`
- `PDFDocumentTextFlowFactory` handles text extraction
- PageMaster already loads full `PDFDocument` objects in `m_documents`
- `PDFDocumentManipulator::assemble()` can extract page ranges

**Next Session Start Here**:
→ **Phase 1, Task 1.1**: Design text search UI (searchdialog.ui)

**Questions to Resolve**:
- None currently

### Session 2 (2025-11-14)
**What We Did**:
- Set up complete Windows build environment for PDF4QT
  - Installed vcpkg package manager
  - Installed Qt 6.9.1 via aqtinstall
  - Installed all PDF4QT dependencies (17 packages via vcpkg)
  - Configured CMake build with Visual Studio 2022
  - Successfully built entire PDF4QT project
- Updated PageMaster window title to "PageMaster - Spec Extractor [DEVELOPMENT BUILD]"
- Completed Task 1.3: Added Search menu item to Edit menu (Ctrl+F)
  - Added `SearchText` to `Operation` enum in mainwindow.h
  - Created `actionSearchText` action in mainwindow.ui
  - Menu item visible and functional (no handler yet)

**Key Learnings**:
- Windows build requires proper Qt path configuration in CMAKE_PREFIX_PATH
- windeployqt needed to deploy Qt DLLs alongside executables
- Incremental builds very fast (~10 seconds for PageMaster changes)
- Full initial build takes ~12 minutes

**Next Session Start Here**:
→ **Phase 1, Tasks 1.1-1.2**: Create search dialog UI and implementation

**Questions to Resolve**:
- None currently

---

## Resources

### Documentation
- [CLAUDE.md](CLAUDE.md) - Full codebase guide for AI assistants
- [PDFTOOL_GUIDE.md](PDFTOOL_GUIDE.md) - PdfTool CLI reference

### Relevant Code Locations
- **Text Search**: `Pdf4QtLibCore/sources/pdftextlayout.h` (line 580-589)
- **Text Extraction**: `Pdf4QtLibCore/sources/pdfdocumenttextflow.h`
- **Document Assembly**: `Pdf4QtLibCore/sources/pdfdocumentmanipulator.h`
- **PageMaster Model**: `Pdf4QtPageMaster/pageitemmodel.h/cpp`
- **PageMaster Main Window**: `Pdf4QtPageMaster/mainwindow.h/cpp`

### Examples in Codebase
- Search implementation: `Pdf4QtEditor` has find dialog
- Dialog creation: Look at `selectoutlinetoregroupdialog.h/cpp`
- Document manipulation: `mainwindow.cpp` line 376-406 (document loading)

---

## Progress Summary

| Phase | Status | Tasks Complete | Tasks Total | % Complete |
|-------|--------|----------------|-------------|------------|
| Phase 1 | In Progress | 1 | 6 | 17% |
| Phase 2 | Not Started | 0 | 7 | 0% |
| Phase 3 | Not Started | 0 | 6 | 0% |
| Phase 4 | Not Started | 0 | 6 | 0% |
| Phase 5 | Not Started | 0 | 4 | 0% |
| **Total** | **In Progress** | **1** | **29** | **3%** |

---

## Git Workflow

### Branch
`claude/claude-md-mhytxiwqpdmzb10c-01Ds332zwWDHxfgBhf53gYGm`

### Commit Message Template
```
Issue #XXX: [Phase N] Feature description

- Specific change 1
- Specific change 2
```

Example:
```
Issue #XXX: [Phase 1] Add basic text search to PageMaster

- Created searchdialog.ui with input field and results list
- Added Operation::SearchText to mainwindow.h
- Implemented PageItemModel::searchText() using PDFTextLayoutStorage
```

---

**Last Updated**: 2025-11-14 by Claude
