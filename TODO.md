# TODO: Division 23 Spec Section Database

Implementation of MasterFormat Division 23 (HVAC) spec section database integration for PageMaster.

## Feature Overview

Add ability to:
1. Load spec section titles from CSV database
2. Display titles inline in batch search dialog
3. Manually discover spec sections in loaded PDFs via Tools menu

## Phase 1: Database Foundation

- [x] Create SpecSectionDatabase class (header and implementation)
- [x] Add CSV to resources directory and resources.qrc
- [x] Initialize database in MainWindow on startup
- [x] Update CMakeLists.txt with new files
- [x] Test database loads and title lookup works

## Phase 2: Enhanced Batch Search

- [x] Modify batch search to show titles inline in section list
- [x] Update results display to show titles
- [x] Test batch search with titles
- [x] Add spec section titles to extracted PDF filenames

## Phase 3: Discovery Dialog

- [x] Create discoversectionsdialog.ui with scan UI
- [x] Implement scan algorithm in discoversectionsdialog.cpp
- [x] Add Tools menu item and integration in mainwindow
- [x] Test discovery dialog with real PDFs

## Future Enhancements

### Discovery Dialog Improvements
- [ ] Add Select All / Select None buttons for discovered sections
- [ ] Handle unmapped/new spec sections (not in Division 23 database)
  - Option 1: Auto-discover titles from PDF context
  - Option 2: Allow user to manually edit/add titles
- [ ] Add ability to save custom spec section titles to user database

### Performance Optimization
- [ ] Consider using regex search for batch search instead of pattern variants
  - Current: generates variants (233600, 23 3600, 23-3600, etc.) and searches each
  - Potential: use single regex search like Discovery Dialog (VERY fast)
  - Would need to test performance impact and accuracy

## Implementation Details

### Database Class Structure
```cpp
class SpecSectionDatabase {
    static SpecSectionDatabase& instance();
    bool loadFromResource();
    QString getTitle(const QString& section) const;
    bool exists(const QString& section) const;
private:
    QMap<QString, QString> m_sections;  // normalized → title
};
```

### Display Format
- Section list: `"23 36 00 - Air Terminal Units"`
- Search still uses normalized format (233600, 23 3600, etc.)

### Discovery Scan
- Regex: `\b23\s*\d{2}\s*\d{2}(?:\.\d{2})?\b`
- Deduplicate and normalize results
- Show with titles and page counts
- Allow adding to batch search

## Resources

- CSV: `C:\Users\jacob\Downloads\masterformat_div23_arcat.csv` (cleaned)
- 312 Division 23 spec sections with titles
