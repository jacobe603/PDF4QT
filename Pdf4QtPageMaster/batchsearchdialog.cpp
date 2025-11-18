// MIT License
//
// Copyright (c) 2018-2025 Jakub Melka and Contributors
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "batchsearchdialog.h"
#include "ui_batchsearchdialog.h"
#include "pageitemmodel.h"
#include "specsectiondatabase.h"
#include "detectsectiontitledialog.h"

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QInputDialog>
#include <QDir>
#include <QMenu>

namespace pdfpagemaster
{

BatchSearchDialog::BatchSearchDialog(PageItemModel* model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::BatchSearchDialog),
    m_model(model)
{
    ui->setupUi(this);

    // Connect section management buttons
    connect(ui->addButton, &QPushButton::clicked, this, &BatchSearchDialog::onAddSectionClicked);
    connect(ui->editButton, &QPushButton::clicked, this, &BatchSearchDialog::onEditSectionClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &BatchSearchDialog::onRemoveSectionClicked);
    connect(ui->clearAllButton, &QPushButton::clicked, this, &BatchSearchDialog::onClearAllClicked);

    // Connect list widget signals
    connect(ui->sectionsListWidget, &QListWidget::itemDoubleClicked, this, &BatchSearchDialog::onSectionListDoubleClicked);
    connect(ui->sectionsListWidget, &QListWidget::itemSelectionChanged, this, &BatchSearchDialog::onSectionSelectionChanged);

    // Connect search and extract buttons
    connect(ui->searchAllButton, &QPushButton::clicked, this, &BatchSearchDialog::onSearchAllClicked);
    connect(ui->extractButton, &QPushButton::clicked, this, &BatchSearchDialog::onExtractSelectedClicked);

    // Enable Return key in line edit to add section
    connect(ui->sectionInputLineEdit, &QLineEdit::returnPressed, this, &BatchSearchDialog::onAddSectionClicked);

    // Setup context menu for sections list
    ui->sectionsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->sectionsListWidget, &QWidget::customContextMenuRequested,
            this, &BatchSearchDialog::onSectionsContextMenu);

    // Connect results list widget signals for navigation
    connect(ui->resultsListWidget, &QListWidget::itemDoubleClicked, this, &BatchSearchDialog::onResultItemDoubleClicked);

    updateButtons();
}

BatchSearchDialog::~BatchSearchDialog()
{
    delete ui;
}

void BatchSearchDialog::addSections(const QStringList& sections)
{
    for (const QString& section : sections)
    {
        QString trimmedSection = section.trimmed();

        if (trimmedSection.isEmpty())
        {
            continue;  // Skip empty sections
        }

        // Validate it's a spec section pattern
        if (!PageItemModel::isSpecSection(trimmedSection))
        {
            continue;  // Skip invalid sections silently
        }

        // Check for duplicates (normalize first to catch different formats of same section)
        QString normalized = PageItemModel::normalizeSpecSection(trimmedSection);

        bool isDuplicate = false;
        for (int i = 0; i < ui->sectionsListWidget->count(); ++i)
        {
            QListWidgetItem* existingItem = ui->sectionsListWidget->item(i);
            QString existingSection = existingItem->data(Qt::UserRole).toString();
            if (PageItemModel::normalizeSpecSection(existingSection) == normalized)
            {
                isDuplicate = true;
                break;
            }
        }

        if (isDuplicate)
        {
            continue;  // Skip duplicates silently
        }

        // Lookup title from database
        QString title = SpecSectionDatabase::instance().getTitle(normalized);

        // Format display text: "23 36 00 - Air Terminal Units" or just "23 36 00" if no title
        QString displayText = trimmedSection;
        if (!title.isEmpty())
        {
            displayText = trimmedSection + " - " + title;
        }

        // Add to list with section stored in UserRole for later retrieval
        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, trimmedSection);  // Store original section number
        ui->sectionsListWidget->addItem(item);
    }

    updateButtons();
}

void BatchSearchDialog::onAddSectionClicked()
{
    QString section = ui->sectionInputLineEdit->text().trimmed();

    if (section.isEmpty())
    {
        return;
    }

    // Validate it's a spec section pattern
    if (!PageItemModel::isSpecSection(section))
    {
        QMessageBox::warning(this, tr("Invalid Format"),
            tr("Please enter a valid spec section format.\n\n"
               "Examples:\n"
               "  233600\n"
               "  23 3600\n"
               "  23-3600\n"
               "  23 36 00"));
        ui->sectionInputLineEdit->selectAll();
        ui->sectionInputLineEdit->setFocus();
        return;
    }

    // Check for duplicates (normalize first to catch different formats of same section)
    QString normalized = PageItemModel::normalizeSpecSection(section);

    for (int i = 0; i < ui->sectionsListWidget->count(); ++i)
    {
        QListWidgetItem* existingItem = ui->sectionsListWidget->item(i);
        QString existingSection = existingItem->data(Qt::UserRole).toString();
        if (PageItemModel::normalizeSpecSection(existingSection) == normalized)
        {
            QMessageBox::information(this, tr("Duplicate"),
                tr("This section is already in the list."));
            ui->sectionInputLineEdit->clear();
            ui->sectionInputLineEdit->setFocus();
            return;
        }
    }

    // Lookup title from database
    QString title = SpecSectionDatabase::instance().getTitle(normalized);

    // Format display text: "23 36 00 - Air Terminal Units" or just "23 36 00" if no title
    QString displayText = section;
    if (!title.isEmpty())
    {
        displayText = section + " - " + title;
    }

    // Add to list with section stored in UserRole for later retrieval
    QListWidgetItem* item = new QListWidgetItem(displayText);
    item->setData(Qt::UserRole, section);  // Store original section number
    ui->sectionsListWidget->addItem(item);

    ui->sectionInputLineEdit->clear();
    ui->sectionInputLineEdit->setFocus();

    updateButtons();
}

void BatchSearchDialog::onEditSectionClicked()
{
    QListWidgetItem* currentItem = ui->sectionsListWidget->currentItem();

    if (!currentItem)
    {
        return;
    }

    QString currentSection = currentItem->data(Qt::UserRole).toString();

    bool ok;
    QString newText = QInputDialog::getText(
        this,
        tr("Edit Spec Section"),
        tr("Spec section:"),
        QLineEdit::Normal,
        currentSection,
        &ok);

    if (!ok || newText.trimmed().isEmpty())
    {
        return;
    }

    newText = newText.trimmed();

    // Validate new text
    if (!PageItemModel::isSpecSection(newText))
    {
        QMessageBox::warning(this, tr("Invalid Format"),
            tr("Please enter a valid spec section format."));
        return;
    }

    // Check for duplicates (excluding the current item)
    QString normalized = PageItemModel::normalizeSpecSection(newText);

    for (int i = 0; i < ui->sectionsListWidget->count(); ++i)
    {
        if (ui->sectionsListWidget->item(i) == currentItem)
        {
            continue;  // Skip the item being edited
        }

        QListWidgetItem* existingItem = ui->sectionsListWidget->item(i);
        QString existingSection = existingItem->data(Qt::UserRole).toString();
        if (PageItemModel::normalizeSpecSection(existingSection) == normalized)
        {
            QMessageBox::information(this, tr("Duplicate"),
                tr("This section is already in the list."));
            return;
        }
    }

    // Lookup title from database
    QString title = SpecSectionDatabase::instance().getTitle(normalized);

    // Format display text: "23 36 00 - Air Terminal Units" or just "23 36 00" if no title
    QString displayText = newText;
    if (!title.isEmpty())
    {
        displayText = newText + " - " + title;
    }

    currentItem->setText(displayText);
    currentItem->setData(Qt::UserRole, newText);  // Update stored section number
}

void BatchSearchDialog::onRemoveSectionClicked()
{
    QList<QListWidgetItem*> selectedItems = ui->sectionsListWidget->selectedItems();

    for (QListWidgetItem* item : selectedItems)
    {
        delete ui->sectionsListWidget->takeItem(ui->sectionsListWidget->row(item));
    }

    updateButtons();
}

void BatchSearchDialog::onClearAllClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Clear All"),
        tr("Remove all spec sections from the list?"),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        ui->sectionsListWidget->clear();
        updateButtons();
    }
}

void BatchSearchDialog::onSectionListDoubleClicked(QListWidgetItem* item)
{
    Q_UNUSED(item);
    onEditSectionClicked();
}

void BatchSearchDialog::onSectionSelectionChanged()
{
    updateButtons();
}

void BatchSearchDialog::onSearchAllClicked()
{
    // Clear previous results
    m_searchResults.clear();
    ui->resultsListWidget->clear();

    int sectionCount = ui->sectionsListWidget->count();

    if (sectionCount == 0)
    {
        QMessageBox::warning(this, tr("No Sections"),
            tr("Please add at least one spec section to search."));
        return;
    }

    // Show progress dialog
    QProgressDialog progress(tr("Searching documents..."),
                            tr("Cancel"), 0, sectionCount, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(500);  // Show after 500ms

    bool exactMatch = ui->exactMatchRadioButton->isChecked();

    // Search each section
    for (int i = 0; i < sectionCount; ++i)
    {
        progress.setValue(i);
        progress.setLabelText(tr("Searching section %1 of %2...").arg(i + 1).arg(sectionCount));

        if (progress.wasCanceled())
        {
            break;
        }

        // Get section from UserRole (original section without title)
        QString section = ui->sectionsListWidget->item(i)->data(Qt::UserRole).toString();
        SectionSearchResult result;
        result.specSection = section;

        std::vector<PageItemModel::SearchResult> searchResults;

        // Check search mode
        if (ui->regexSearchRadioButton->isChecked())
        {
            // Regex search mode: generate regex pattern and search once
            QString pattern = PageItemModel::generateRegexPattern(section);
            QRegularExpression regex(pattern);
            searchResults = m_model->searchTextRegex(regex);
        }
        else if (exactMatch)
        {
            // Exact match: search only the entered text
            searchResults = m_model->searchText(section, false);
        }
        else
        {
            // Pattern variant mode: search all variants
            QStringList variants = PageItemModel::generateSearchVariants(section);
            std::set<QString> seenKeys;  // Deduplicate results

            for (const QString& variant : variants)
            {
                std::vector<PageItemModel::SearchResult> variantResults =
                    m_model->searchText(variant, false);

                for (const auto& res : variantResults)
                {
                    QString key = QString("%1_%2").arg(res.documentIndex)
                                                   .arg(res.pageNumber);

                    if (seenKeys.find(key) == seenKeys.end())
                    {
                        seenKeys.insert(key);
                        searchResults.push_back(res);
                    }
                }
            }

            // Sort results by document and page
            std::sort(searchResults.begin(), searchResults.end(),
                [](const PageItemModel::SearchResult& a, const PageItemModel::SearchResult& b) {
                    if (a.documentIndex != b.documentIndex)
                        return a.documentIndex < b.documentIndex;
                    return a.pageNumber < b.pageNumber;
                });
        }

        // Detect page ranges
        result.ranges = m_model->detectPageRanges(searchResults);

        // Filter to multi-page ranges if any exist
        bool hasMultiPageRanges = false;
        for (const auto& range : result.ranges)
        {
            if (range.pageCount() > 1)
            {
                hasMultiPageRanges = true;
                break;
            }
        }

        if (hasMultiPageRanges)
        {
            std::vector<PageItemModel::PageRange> filtered;
            for (const auto& range : result.ranges)
            {
                if (range.pageCount() > 1)
                {
                    filtered.push_back(range);
                }
            }
            result.ranges = std::move(filtered);
        }

        m_searchResults.push_back(result);
    }

    progress.setValue(sectionCount);

    // Display results
    displaySearchResults();

    updateButtons();
}

void BatchSearchDialog::displaySearchResults()
{
    ui->resultsListWidget->clear();

    if (m_searchResults.empty())
    {
        ui->resultsListWidget->addItem(tr("No results found."));
        return;
    }

    int totalRanges = 0;

    for (const auto& sectionResult : m_searchResults)
    {
        // Section header with title
        QString normalized = PageItemModel::normalizeSpecSection(sectionResult.specSection);
        QString title = SpecSectionDatabase::instance().getTitle(normalized);

        QString headerText;
        if (!title.isEmpty())
        {
            headerText = tr("Section: %1 - %2 (%3 ranges found)")
                .arg(sectionResult.specSection)
                .arg(title)
                .arg(sectionResult.ranges.size());
        }
        else
        {
            headerText = tr("Section: %1 (%2 ranges found)")
                .arg(sectionResult.specSection)
                .arg(sectionResult.ranges.size());
        }

        QListWidgetItem* headerItem = new QListWidgetItem(headerText);
        QFont headerFont = headerItem->font();
        headerFont.setBold(true);
        headerItem->setFont(headerFont);
        headerItem->setFlags(Qt::ItemIsEnabled);  // Not selectable or checkable
        headerItem->setBackground(QBrush(QColor(240, 240, 240)));  // Light gray background
        ui->resultsListWidget->addItem(headerItem);

        // Range items with checkboxes
        for (size_t i = 0; i < sectionResult.ranges.size(); ++i)
        {
            const auto& range = sectionResult.ranges[i];

            QString rangeText;
            if (range.firstPage == range.lastPage)
            {
                rangeText = tr("  Range %1: Page %2 (%3) - 1 page")
                    .arg(i + 1)
                    .arg(range.firstPage)
                    .arg(QFileInfo(range.documentName).fileName());
            }
            else
            {
                rangeText = tr("  Range %1: Pages %2-%3 (%4) - %5 pages")
                    .arg(i + 1)
                    .arg(range.firstPage)
                    .arg(range.lastPage)
                    .arg(QFileInfo(range.documentName).fileName())
                    .arg(range.pageCount());
            }

            QListWidgetItem* rangeItem = new QListWidgetItem(rangeText);
            rangeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
            rangeItem->setCheckState(Qt::Checked);  // Default: all checked

            // Store section and range indices as item data
            QVariantMap data;
            data["sectionIndex"] = static_cast<int>(&sectionResult - &m_searchResults[0]);
            data["rangeIndex"] = static_cast<int>(i);
            rangeItem->setData(Qt::UserRole, data);

            ui->resultsListWidget->addItem(rangeItem);
            totalRanges++;
        }

        // Blank line between sections
        if (&sectionResult != &m_searchResults.back())
        {
            QListWidgetItem* blankItem = new QListWidgetItem("");
            blankItem->setFlags(Qt::ItemIsEnabled);
            ui->resultsListWidget->addItem(blankItem);
        }
    }

    // Summary
    ui->resultsListWidget->addItem("");  // Blank line
    QString summaryText = tr("Total: %1 ranges found across %2 sections")
        .arg(totalRanges)
        .arg(m_searchResults.size());
    QListWidgetItem* summaryItem = new QListWidgetItem(summaryText);
    QFont summaryFont = summaryItem->font();
    summaryFont.setBold(true);
    summaryItem->setFont(summaryFont);
    summaryItem->setFlags(Qt::ItemIsEnabled);
    ui->resultsListWidget->addItem(summaryItem);
}

void BatchSearchDialog::onExtractSelectedClicked()
{
    // Collect selected ranges
    std::vector<std::pair<int, int>> selectedRanges;  // pair<sectionIndex, rangeIndex>

    for (int i = 0; i < ui->resultsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->resultsListWidget->item(i);

        if (item->checkState() == Qt::Checked)
        {
            QVariantMap data = item->data(Qt::UserRole).toMap();

            if (data.contains("sectionIndex") && data.contains("rangeIndex"))
            {
                int sectionIdx = data["sectionIndex"].toInt();
                int rangeIdx = data["rangeIndex"].toInt();
                selectedRanges.push_back(std::make_pair(sectionIdx, rangeIdx));
            }
        }
    }

    if (selectedRanges.empty())
    {
        QMessageBox::warning(this, tr("No Selection"),
            tr("Please check at least one range to extract."));
        return;
    }

    // For now, only support separate files extraction
    extractSeparateFiles(selectedRanges);
}

void BatchSearchDialog::extractSeparateFiles(const std::vector<std::pair<int, int>>& selectedRanges)
{
    // Choose output directory
    QString outputDir = QFileDialog::getExistingDirectory(
        this,
        tr("Select Output Directory"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly);

    if (outputDir.isEmpty())
    {
        return;
    }

    QProgressDialog progress(tr("Extracting ranges..."),
                            tr("Cancel"), 0, selectedRanges.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    int successCount = 0;
    int failureCount = 0;
    QStringList errors;

    for (size_t i = 0; i < selectedRanges.size(); ++i)
    {
        progress.setValue(static_cast<int>(i));

        if (progress.wasCanceled())
        {
            break;
        }

        int sectionIdx = selectedRanges[i].first;
        int rangeIdx = selectedRanges[i].second;

        const SectionSearchResult& sectionResult = m_searchResults[sectionIdx];
        const PageItemModel::PageRange& range = sectionResult.ranges[rangeIdx];

        QString specSection = sectionResult.specSection;

        // Lookup title from database
        QString normalized = PageItemModel::normalizeSpecSection(specSection);
        QString title = SpecSectionDatabase::instance().getTitle(normalized);

        // Sanitize title for filename (remove invalid characters)
        QString sanitizedTitle = title;
        sanitizedTitle.replace("/", "_");
        sanitizedTitle.replace("\\", "_");
        sanitizedTitle.replace(":", "_");
        sanitizedTitle.replace("*", "_");
        sanitizedTitle.replace("?", "_");
        sanitizedTitle.replace("\"", "_");
        sanitizedTitle.replace("<", "_");
        sanitizedTitle.replace(">", "_");
        sanitizedTitle.replace("|", "_");

        // Create filename with title if available
        QString fileName;
        QString sectionNormalized = specSection;
        sectionNormalized.replace(" ", "").replace("-", "").replace("_", "");

        if (!sanitizedTitle.isEmpty())
        {
            fileName = QString("%1_%2_Pages_%3-%4.pdf")
                .arg(sectionNormalized)
                .arg(sanitizedTitle.replace(" ", "_"))
                .arg(range.firstPage)
                .arg(range.lastPage);
        }
        else
        {
            fileName = QString("Section_%1_Pages_%2-%3.pdf")
                .arg(sectionNormalized)
                .arg(range.firstPage)
                .arg(range.lastPage);
        }

        QString outputPath = QDir(outputDir).filePath(fileName);

        // Extract the range
        QString errorMessage;
        bool success = m_model->extractPageRange(
            range.documentIndex,
            range.firstPage,
            range.lastPage,
            outputPath,
            errorMessage);

        if (success)
        {
            successCount++;
        }
        else
        {
            failureCount++;
            errors << tr("Section %1, Range %2: %3")
                .arg(sectionResult.specSection)
                .arg(rangeIdx + 1)
                .arg(errorMessage);
        }
    }

    progress.setValue(static_cast<int>(selectedRanges.size()));

    // Show summary
    QString summary = tr("Extraction complete:\n"
                        "  Successful: %1\n"
                        "  Failed: %2")
        .arg(successCount)
        .arg(failureCount);

    if (!errors.isEmpty())
    {
        summary += tr("\n\nErrors:\n") + errors.join("\n");
        QMessageBox::warning(this, tr("Extraction Complete"), summary);
    }
    else
    {
        QMessageBox::information(this, tr("Extraction Complete"), summary);
    }
}

void BatchSearchDialog::updateButtons()
{
    // Section management buttons
    bool hasSections = ui->sectionsListWidget->count() > 0;
    bool hasSelection = !ui->sectionsListWidget->selectedItems().isEmpty();

    ui->editButton->setEnabled(hasSelection && ui->sectionsListWidget->selectedItems().count() == 1);
    ui->removeButton->setEnabled(hasSelection);
    ui->clearAllButton->setEnabled(hasSections);
    ui->searchAllButton->setEnabled(hasSections);

    // Extract button (enabled if we have results with at least one checkbox checked)
    bool hasCheckedResults = false;
    for (int i = 0; i < ui->resultsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->resultsListWidget->item(i);
        if (item->checkState() == Qt::Checked)
        {
            hasCheckedResults = true;
            break;
        }
    }
    ui->extractButton->setEnabled(hasCheckedResults);
}

void BatchSearchDialog::onSectionsContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->sectionsListWidget->itemAt(pos);
    if (!item)
    {
        return;
    }

    QMenu menu(this);
    QAction* detectAction = menu.addAction(tr("Detect Title from PDF..."));

    QAction* selected = menu.exec(ui->sectionsListWidget->mapToGlobal(pos));
    if (selected == detectAction)
    {
        onDetectTitleRequested();
    }
}

void BatchSearchDialog::onDetectTitleRequested()
{
    // Get selected item
    QListWidgetItem* item = ui->sectionsListWidget->currentItem();
    if (!item)
    {
        return;
    }

    // Get section number from UserRole
    QString section = item->data(Qt::UserRole).toString();
    if (section.isEmpty())
    {
        return;
    }

    // Normalize section for database lookup
    QString normalized = PageItemModel::normalizeSpecSection(section);

    // Check if title already exists in CSV database (not custom)
    if (SpecSectionDatabase::instance().exists(normalized) &&
        !SpecSectionDatabase::instance().hasCustomTitle(normalized))
    {
        QMessageBox::information(this, tr("Title Exists"),
            tr("This section already has a title in the Division 23 database:\n\n%1")
            .arg(SpecSectionDatabase::instance().getTitle(normalized)));
        return;
    }

    // Determine which document to search
    // For batch search dialog, we need to search all loaded documents
    int documentIndex = -1;
    const auto& documents = m_model->getDocuments();
    for (const auto& [docIdx, docItem] : documents)
    {
        // Search this document for the section
        QStringList variants = PageItemModel::generateSearchVariants(section);
        for (const QString& variant : variants)
        {
            auto results = m_model->searchText(variant, false);
            if (!results.empty())
            {
                documentIndex = docIdx;
                break;
            }
        }
        if (documentIndex != -1)
        {
            break;
        }
    }

    if (documentIndex == -1)
    {
        QMessageBox::warning(this, tr("Section Not Found"),
            tr("Could not find this section in any loaded documents."));
        return;
    }

    // Detect title
    QPair<bool, QString> result = m_model->detectSpecSectionTitle(section, documentIndex);

    if (!result.first || result.second.isEmpty())
    {
        QMessageBox::warning(this, tr("Detection Failed"),
            tr("Could not automatically detect a title for this section.\n\n"
               "You can manually enter a title by editing the section entry."));
        return;
    }

    // Show confirmation dialog
    DetectSectionTitleDialog dialog(this);
    dialog.setSection(section);
    dialog.setDetectedTitle(result.second);

    if (dialog.exec() == QDialog::Accepted)
    {
        QString title = dialog.getTitle();
        if (!title.isEmpty())
        {
            // Save to custom titles
            SpecSectionDatabase::instance().setCustomTitle(normalized, title);

            // Update display text
            QString displayText = section + " - " + title;
            item->setText(displayText);

            QMessageBox::information(this, tr("Title Saved"),
                tr("The custom title has been saved for this section."));
        }
    }
}

void BatchSearchDialog::onResultItemDoubleClicked(QListWidgetItem* item)
{
    if (!item)
    {
        return;
    }

    // Get the stored section and range indices
    QVariantMap data = item->data(Qt::UserRole).toMap();
    if (data.isEmpty())
    {
        return;  // Header item or "No results" item
    }

    int sectionIndex = data["sectionIndex"].toInt();
    int rangeIndex = data["rangeIndex"].toInt();

    // Validate indices
    if (sectionIndex < 0 || sectionIndex >= static_cast<int>(m_searchResults.size()))
    {
        return;
    }

    const auto& sectionResult = m_searchResults[sectionIndex];
    if (rangeIndex < 0 || rangeIndex >= static_cast<int>(sectionResult.ranges.size()))
    {
        return;
    }

    const PageItemModel::PageRange& range = sectionResult.ranges[rangeIndex];

    // Emit signal to navigate to the first page of this range
    navigateToPage(range.documentIndex, range.firstPage);
}

}   // namespace pdfpagemaster
