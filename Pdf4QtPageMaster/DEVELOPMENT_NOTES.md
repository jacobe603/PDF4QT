# PageMaster Development Notes

## Overview

This file documents custom features, technical lessons learned, and development insights from enhancing PDF4QT PageMaster for construction specification document management.

**Primary Use Case**: Managing and organizing construction specification PDFs, particularly Division 23 (HVAC) sections.

---

## Feature Additions

### 1. Spec Section Database System

**Commit**: `7bd26e1` - Add Division 23 spec section database and discovery features

**Components**:
- `specsectiondatabase.h/cpp` - CSV-based database for spec section titles
- Database file: `division_23_sections.csv` (MasterFormat sections)
- Custom title support (session-only, not persisted)
- Auto-loading from application directory

**Key Methods**:
- `loadDatabase()` - Load CSV database on startup
- `getTitle()` - Retrieve section title by number
- `setCustomTitle()` - Set session-only custom title
- `clearCustomTitles()` - Clear session data when closing documents

**Technical Notes**:
- Custom titles are intentionally NOT persisted to QSettings
- Prevents incorrect titles from one document affecting others
- Database loaded on first access (lazy loading pattern)

### 2. Spec Section Discovery

**Commit**: `7bd26e1` - Add Division 23 spec section database and discovery features

**Components**:
- `discoversectionsdialog.h/cpp/ui` - Dialog for discovering spec sections in PDFs
- Regex pattern: `\b23\s*\d{2}\s*\d{2}(?:\.\d{2})?\b`
- Matches: "23 36 00", "23 36 00.01", etc.

**Features**:
- Full document search using PDFTextFlow
- Groups results by spec section number
- Shows page ranges where each section appears
- Right-click menu to save custom titles to database
- Navigation to specific pages on click

**Technical Implementation**:
```cpp
QRegularExpression sectionRegex(R"(\b23\s*\d{2}\s*\d{2}(?:\.\d{2})?\b)");
// Searches through PDFTextFlow from all pages
// Groups by section number
// Displays with database titles
```

### 3. Automatic Spec Section Title Detection

**Commit**: `da946c0` - Add automatic spec section title detection to PageMaster

**Components**:
- `detectsectiontitledialog.h/cpp/ui` - Dialog for detecting section titles
- Searches for spec number + nearby text to extract title

**Algorithm**:
1. Search PDF for spec section number (e.g., "23 36 00")
2. Extract surrounding text context
3. Apply heuristics to identify title (typically on same line)
4. Allow user to confirm/edit before saving

### 4. Text Search with Navigation

**Commit**: `6a193b9` - Add navigation and preview enhancements to PageMaster

**Components**:
- `searchdialog.h/cpp/ui` - Single document text search
- `batchsearchdialog.h/cpp/ui` - Multi-document batch search
- Case-sensitive search option
- Results list with page numbers
- Click-to-navigate functionality

**Features**:
- Search across all grouped documents
- Results show document name, page number, and context
- Double-click navigates to page and updates preview
- Integrates with preview system

### 5. Navigation and Preview System

**Commit**: `1ebfa23` - Fix Previous/Next page buttons to cycle within grouped PDFs

**Components**:
- Previous/Next page buttons in UI
- Preview page tracking via `previewPageIndex`
- Integration with search dialogs

**Key Operations** (mainwindow.h):
- `Operation::NextPage` - Cycle to next page in group
- `Operation::PreviousPage` - Cycle to previous page in group
- `Operation::SearchText` - Single document search
- `Operation::BatchSearchText` - Multi-document search
- `Operation::DiscoverSections` - Spec section discovery

---

## Technical Lessons Learned

### Qt Model/View Architecture

#### Lesson 1: dataChanged() Signal Requires Roles Parameter

**Problem**: Preview thumbnails weren't updating when cycling through pages within a group.

**Root Cause**:
```cpp
Q_EMIT dataChanged(index, index);  // ❌ Insufficient
```

When emitting `dataChanged()` without a roles parameter, Qt checks the model's `data()` method for standard roles (DisplayRole, DecorationRole, etc.). If the DisplayRole (group name) hasn't changed, Qt's view optimization may skip the repaint.

**Solution**:
```cpp
Q_EMIT dataChanged(index, index, {Qt::DecorationRole});  // ✅ Forces repaint
```

**Why This Works**:
- Explicitly tells Qt that the decoration (custom delegate painting) has changed
- Forces the view to call the delegate's `paint()` method
- Bypasses Qt's optimization that checks DisplayRole equality

**Location**: `pageitemmodel.cpp` line 1581

**Reference**: PageItemModel::setGroupPreviewPage()

#### Lesson 2: Preview Update Pipeline

The complete flow for updating a preview thumbnail:

1. **User Action** → Button click or navigation event
2. **Model Update** → `setGroupPreviewPage(index, docIndex, pageIndex)`
   - Updates `item.previewPageIndex` to new page index
   - Emits `dataChanged(index, index, {Qt::DecorationRole})`
3. **View Notification** → Qt's QListView receives dataChanged signal
4. **Delegate Paint** → `PageItemDelegate::paint()` is called
5. **Cache Key Generation** → Based on page properties + previewPageIndex
   ```cpp
   QString key = QString("%1#%2#%3#%4#%5@%6x%7")
       .arg(documentIndex)
       .arg(imageIndex)
       .arg(rotation)
       .arg(pageIndex)        // Different for each page
       .arg(pageType)
       .arg(width)
       .arg(height);
   ```
6. **Cache Lookup** → `QPixmapCache::find(key, &pixmap)`
7. **Render or Retrieve** → Either render new page or use cached pixmap
8. **Display** → Paint pixmap to view

**Key Insight**: Each step must complete correctly for preview updates to work. The DecorationRole parameter is critical for step 3.

#### Lesson 3: Navigation Within Groups vs Between Groups

**Original Implementation** (commit 6a193b9):
```cpp
// Navigated to DIFFERENT GROUP (different row)
int nextRow = currentRow + 1;
QModelIndex index = m_model->index(nextRow, 0);
// Changed selection and scrolled
ui->documentItemsView->selectionModel()->select(index, ...);
ui->documentItemsView->scrollTo(index, ...);
```

**Why It Seemed to Work**:
- Changing selection ALWAYS forces a repaint
- Scrolling triggers visual updates
- Moving to different group meant different DisplayRole value

**Problem**:
- When all pages are in ONE group, there's no "next group" to navigate to
- Buttons did nothing in this common scenario

**Fixed Implementation** (commit 1ebfa23):
```cpp
// Navigate WITHIN current group
size_t nextPageIndex = (item->previewPageIndex + 1) % item->groups.size();
const auto& nextPage = item->groups[nextPageIndex];
m_model->setGroupPreviewPage(currentIndex, nextPage.documentIndex, nextPage.pageIndex);
```

**Why This Works**:
- Cycles through pages in current group using modulo arithmetic
- Stays on same row (no selection change)
- Wraps around: last page → first page, first page → last page
- Works regardless of number of groups
- Combined with DecorationRole fix, properly updates preview

**Location**: `mainwindow.cpp` lines 790-828

### QPixmapCache Behavior

**Cache Key Requirements**:
- Must be unique per rendered page
- Must include all properties that affect rendering:
  - Document index
  - Page index (within document)
  - Image index (for image-based pages)
  - Rotation angle
  - Page type
  - Render dimensions (width × height)

**Cache Invalidation**:
- Not explicitly required when key changes
- Each unique page gets unique cache key
- Old cached pages remain in cache (LRU eviction)
- No need to manually remove old entries

**Performance Consideration**:
- Cache hit = instant display
- Cache miss = page render (can be slow for complex pages)
- Preview updates are fast because we keep all group pages cached

### PDF Text Searching

**PDFTextFlow API**:
```cpp
pdf::PDFTextFlows flows = pdf::PDFTextFlow::createTextFlows(
    document,
    pdf::PDFTextFlow::FlowFlags()
);
```

**Key Concepts**:
- TextFlow represents logical text order (reading order)
- Spans multiple pages
- Preserves text structure (paragraphs, columns, etc.)
- Used for searching across documents

**Search Pattern**:
```cpp
for (const pdf::PDFTextFlow& flow : flows) {
    for (const pdf::PDFTextBlock& block : flow.getBlocks()) {
        QString text = block.getText();
        if (text.contains(searchTerm, caseSensitive ?
            Qt::CaseSensitive : Qt::CaseInsensitive)) {
            // Found match
        }
    }
}
```

---

## Architecture Patterns

### Model-View-Delegate Pattern

**PageItemModel** (QAbstractListModel):
- Stores `std::vector<PageGroupItem>`
- Each PageGroupItem contains multiple GroupItem pages
- Tracks `previewPageIndex` - which page in group to display
- Emits signals when data changes

**PageItemDelegate** (QStyledItemDelegate):
- Custom painting of thumbnails
- Generates cache keys
- Renders page images
- Handles layout (title, page info, thumbnail)

**QListView** (ui->documentItemsView):
- Displays list of groups
- Connects model signals to view updates
- Handles selection and scrolling

**Interaction Flow**:
```
User Click → MainWindow::performOperation()
           → PageItemModel::setGroupPreviewPage()
           → dataChanged() signal
           → QListView receives signal
           → PageItemDelegate::paint()
           → Display updated
```

### Session-Only Data Pattern

**Use Case**: Custom spec section titles that shouldn't persist across documents.

**Implementation**:
```cpp
// In-memory storage
mutable QHash<QString, QString> m_customTitles;

// Load: NO QSettings
void loadCustomTitles() const {
    m_customTitlesLoaded = true;
    // Don't load from QSettings
}

// Save: NO QSettings
void setCustomTitle(const QString& section, const QString& title) {
    m_customTitles[section] = title;
    // Don't save to QSettings
}

// Clear on document close
void clearCustomTitles() {
    m_customTitles.clear();
    m_customTitlesLoaded = false;
}
```

**Why This Matters**:
- Spec section titles can vary between documents
- Same number might have different context/title
- Persistent storage would cause cross-contamination
- Session-only keeps data document-specific

---

## Troubleshooting Guide

### Problem: Preview Not Updating

**Symptoms**: Clicking Previous/Next buttons doesn't change preview thumbnail.

**Diagnosis Steps**:
1. Check if `setGroupPreviewPage()` is being called
2. Verify `previewPageIndex` is changing in model
3. Check if `dataChanged()` signal includes roles parameter
4. Verify delegate's `paint()` is being called
5. Check cache key generation

**Solutions**:
- Add `{Qt::DecorationRole}` to dataChanged() signal
- Ensure navigation logic uses correct index calculations
- Verify group contains multiple pages to cycle through

### Problem: Search Not Finding Text

**Symptoms**: Known text in PDF doesn't appear in search results.

**Diagnosis Steps**:
1. Check if PDF has searchable text (not scanned image)
2. Verify case-sensitivity setting matches search term
3. Check if text is in vector content vs image

**Solutions**:
- Use case-insensitive search for better results
- Verify PDF has OCR text layer if scanned
- Check PDFTextFlow is being created correctly

### Problem: Spec Sections Not Discovered

**Symptoms**: Discovery dialog shows no results despite visible spec numbers.

**Diagnosis Steps**:
1. Check regex pattern matches spec number format
2. Verify document has Division 23 sections
3. Check if text is searchable (not image-only)

**Solutions**:
- Current regex: `\b23\s*\d{2}\s*\d{2}(?:\.\d{2})?\b`
- Matches "23 36 00" with flexible spacing
- May need adjustment for non-standard formats

---

## Development Patterns

### Adding a New Dialog

1. **Create UI file** in Qt Designer:
   ```
   Pdf4QtPageMaster/newdialog.ui
   ```

2. **Create header/source**:
   ```cpp
   // newdialog.h
   #include "ui_newdialog.h"

   class NewDialog : public QDialog {
       Q_OBJECT
   public:
       explicit NewDialog(QWidget* parent = nullptr);
   private:
       Ui::NewDialog ui;
   };
   ```

3. **Add to CMakeLists.txt**:
   ```cmake
   add_executable(Pdf4QtPageMaster
       newdialog.cpp
       newdialog.h
       newdialog.ui
       # ... other files
   )
   ```

4. **Connect in MainWindow**:
   ```cpp
   // Add to Operation enum
   enum class Operation {
       NewFeature,
       // ...
   };

   // Add menu action
   // Connect to performOperation()
   ```

### Adding Database Field

1. **Update CSV file** structure
2. **Modify loadDatabase()** parsing logic
3. **Add getter method** in SpecSectionDatabase
4. **Update UI** to display new field

### Pattern: Navigation with Preview Update

```cpp
// In dialog with search results
connect(ui->resultsList, &QListWidget::itemDoubleClicked,
    this, [this](QListWidgetItem* item) {
        int docIndex = item->data(Qt::UserRole).toInt();
        int pageIndex = item->data(Qt::UserRole + 1).toInt();

        // Emit signal to MainWindow
        Q_EMIT navigateToPage(docIndex, pageIndex);
    });

// In MainWindow
connect(dialog, &Dialog::navigateToPage,
    this, [this](int docIndex, int pageIndex) {
        // Find group containing this page
        for (int row = 0; row < m_model->rowCount(); ++row) {
            QModelIndex index = m_model->index(row, 0);
            const PageGroupItem* item = m_model->getPageGroupItem(index);

            // Find page in group
            for (size_t i = 0; i < item->groups.size(); ++i) {
                if (item->groups[i].documentIndex == docIndex &&
                    item->groups[i].pageIndex == pageIndex) {

                    // Select group
                    ui->documentItemsView->selectionModel()->select(
                        index, QItemSelectionModel::ClearAndSelect);

                    // Update preview
                    m_model->setGroupPreviewPage(index, docIndex, pageIndex);

                    // Scroll to view
                    ui->documentItemsView->scrollTo(index);
                    return;
                }
            }
        }
    });
```

---

## Future Enhancement Ideas

### Potential Features

1. **Auto-Highlighting**
   - Attempted but blocked by internal header dependencies
   - Would require PDF4QT API changes or plugin architecture
   - Alternative: External post-processing tool

2. **Spec Section Template Export**
   - Export discovered sections to structured format
   - JSON/XML output with page ranges
   - Integration with project management tools

3. **Multi-Division Support**
   - Currently Division 23 only
   - Could expand to all MasterFormat divisions
   - Configurable division databases

4. **OCR Integration**
   - For scanned PDFs without text layer
   - Would enable search on image-only documents

5. **Section Comparison**
   - Compare sections across different spec documents
   - Highlight differences
   - Track revisions

### Technical Debt

- None currently identified
- Code follows PDF4QT conventions
- All features properly integrated

---

## Key Commits Reference

| Commit | Date | Description |
|--------|------|-------------|
| `7bd26e1` | Session 3 | Add Division 23 spec section database and discovery features |
| `da946c0` | Session 4 | Add automatic spec section title detection |
| `6a193b9` | Session 6 | Add navigation and preview enhancements |
| `1ebfa23` | Session 7 | Fix Previous/Next page buttons to cycle within grouped PDFs |

---

## Contact & Attribution

**Developer**: jacobe603 (GitHub)
**AI Assistant**: Claude Code
**Base Project**: PDF4QT by Jakub Melka
**License**: MIT License

---

**Last Updated**: 2025-11-18
**Version**: 1.5.2.0
