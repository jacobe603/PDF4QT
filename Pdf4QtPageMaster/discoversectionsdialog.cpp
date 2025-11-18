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

#include "discoversectionsdialog.h"
#include "ui_discoversectionsdialog.h"
#include "specsectiondatabase.h"
#include "detectsectiontitledialog.h"

#include <QMessageBox>
#include <QProgressDialog>
#include <QRegularExpression>
#include <QSet>
#include <QMenu>
#include <QInputDialog>

namespace pdfpagemaster
{

DiscoverSectionsDialog::DiscoverSectionsDialog(PageItemModel* model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DiscoverSectionsDialog),
    m_model(model)
{
    ui->setupUi(this);

    // Connect signals
    connect(ui->scanButton, &QPushButton::clicked, this, &DiscoverSectionsDialog::onScanClicked);
    connect(ui->addSelectedButton, &QPushButton::clicked, this, &DiscoverSectionsDialog::onAddSelectedClicked);
    connect(ui->resultsListWidget, &QListWidget::itemSelectionChanged, this, &DiscoverSectionsDialog::onResultsSelectionChanged);
    connect(ui->resultsListWidget, &QListWidget::itemDoubleClicked, this, &DiscoverSectionsDialog::onResultItemDoubleClicked);
    connect(ui->selectAllButton, &QPushButton::clicked, this, &DiscoverSectionsDialog::onSelectAllClicked);
    connect(ui->selectNoneButton, &QPushButton::clicked, this, &DiscoverSectionsDialog::onSelectNoneClicked);

    // Setup context menu for results list
    ui->resultsListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->resultsListWidget, &QWidget::customContextMenuRequested,
            this, &DiscoverSectionsDialog::onResultsContextMenu);

    updateButtons();
}

DiscoverSectionsDialog::~DiscoverSectionsDialog()
{
    delete ui;
}

void DiscoverSectionsDialog::onScanClicked()
{
    scanDocuments();
}

void DiscoverSectionsDialog::onAddSelectedClicked()
{
    m_selectedSections.clear();

    for (int i = 0; i < ui->resultsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->resultsListWidget->item(i);

        if (item->checkState() == Qt::Checked)
        {
            // Get the section number from UserRole
            QString section = item->data(Qt::UserRole).toString();
            if (!section.isEmpty())
            {
                m_selectedSections << section;
            }
        }
    }

    if (m_selectedSections.isEmpty())
    {
        QMessageBox::information(this, tr("No Selection"),
            tr("Please select at least one section to add."));
        return;
    }

    // Emit signal and close dialog
    sectionsSelected(m_selectedSections);
    accept();
}

void DiscoverSectionsDialog::onResultsSelectionChanged()
{
    updateButtons();
}

void DiscoverSectionsDialog::scanDocuments()
{
    m_discoveredSections.clear();
    ui->resultsListWidget->clear();

    if (!m_model || m_model->rowCount(QModelIndex()) == 0)
    {
        QMessageBox::warning(this, tr("No Documents"),
            tr("Please load at least one PDF document before scanning."));
        return;
    }

    // We'll search for a pattern that matches Division 23 sections
    // For simplicity, we'll search for "23" and then validate the results
    QProgressDialog progress(tr("Scanning documents..."),
                            tr("Cancel"), 0, 1, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);

    // Search for "23" to find all potential Division 23 sections
    std::vector<PageItemModel::SearchResult> searchResults = m_model->searchText("23", false);

    progress.setMaximum(searchResults.size());

    // Regex pattern for Division 23 sections: 23 xx xx or 23 xx xx.xx
    QRegularExpression sectionRegex(R"(\b23\s*\d{2}\s*\d{2}(?:\.\d{2})?\b)");

    // Track unique sections and their page occurrences
    QMap<QString, QSet<int>> sectionPages;  // normalized -> set of page indices

    for (size_t i = 0; i < searchResults.size(); ++i)
    {
        progress.setValue(static_cast<int>(i));

        if (progress.wasCanceled())
        {
            break;
        }

        const auto& result = searchResults[i];

        // Check if the context matches our pattern
        QRegularExpressionMatch match = sectionRegex.match(result.context);

        if (match.hasMatch())
        {
            QString foundText = match.captured(0);

            // Normalize the section number
            QString normalized = PageItemModel::normalizeSpecSection(foundText);

            // Track this section and page
            if (!sectionPages.contains(normalized))
            {
                sectionPages[normalized] = QSet<int>();

                // Store the first occurrence's display text and location
                DiscoveredSection section;
                section.section = normalized;
                section.displayText = foundText;
                section.pageCount = 0;
                section.firstDocumentIndex = result.documentIndex;
                section.firstPageNumber = result.pageNumber;  // Already 1-based from search results
                m_discoveredSections[normalized] = section;
            }

            // Track the page (we use a unique key combining document and page)
            int uniquePageId = result.documentIndex * 10000 + result.pageNumber;
            sectionPages[normalized].insert(uniquePageId);
        }
    }

    progress.setValue(static_cast<int>(searchResults.size()));

    // Update page counts
    for (auto it = sectionPages.begin(); it != sectionPages.end(); ++it)
    {
        QString normalized = it.key();
        int pageCount = it.value().size();

        if (m_discoveredSections.contains(normalized))
        {
            m_discoveredSections[normalized].pageCount = pageCount;
        }
    }

    // Display results
    displayResults();

    // Show summary
    if (m_discoveredSections.isEmpty())
    {
        QMessageBox::information(this, tr("Scan Complete"),
            tr("No Division 23 spec sections were found in the loaded documents."));
    }
    else
    {
        QMessageBox::information(this, tr("Scan Complete"),
            tr("Found %1 unique Division 23 spec sections.").arg(m_discoveredSections.size()));
    }
}

void DiscoverSectionsDialog::displayResults()
{
    ui->resultsListWidget->clear();

    if (m_discoveredSections.isEmpty())
    {
        return;
    }

    // Sort sections by normalized number
    QStringList sortedSections = m_discoveredSections.keys();
    sortedSections.sort();

    for (const QString& normalized : sortedSections)
    {
        const DiscoveredSection& section = m_discoveredSections[normalized];

        // Lookup title from database
        QString title = SpecSectionDatabase::instance().getTitle(normalized);

        // Format display text: "23 36 00 - Air Terminal Units (5 pages)"
        QString displayText;
        if (!title.isEmpty())
        {
            displayText = tr("%1 - %2 (%3 pages)")
                .arg(section.displayText)
                .arg(title)
                .arg(section.pageCount);
        }
        else
        {
            displayText = tr("%1 (%2 pages)")
                .arg(section.displayText)
                .arg(section.pageCount);
        }

        QListWidgetItem* item = new QListWidgetItem(displayText);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);  // Default: all checked
        item->setData(Qt::UserRole, section.displayText);  // Store original section format

        ui->resultsListWidget->addItem(item);
    }

    updateButtons();
}

void DiscoverSectionsDialog::onSelectAllClicked()
{
    for (int i = 0; i < ui->resultsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->resultsListWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
}

void DiscoverSectionsDialog::onSelectNoneClicked()
{
    for (int i = 0; i < ui->resultsListWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->resultsListWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
}

void DiscoverSectionsDialog::updateButtons()
{
    bool hasResults = ui->resultsListWidget->count() > 0;
    ui->addSelectedButton->setEnabled(hasResults);
    ui->selectAllButton->setEnabled(hasResults);
    ui->selectNoneButton->setEnabled(hasResults);
}

void DiscoverSectionsDialog::onResultsContextMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->resultsListWidget->itemAt(pos);
    if (!item)
    {
        return;
    }

    QMenu menu(this);
    QAction* detectAction = menu.addAction(tr("Detect Title from PDF..."));
    QAction* editAction = menu.addAction(tr("Edit Title Manually..."));

    QAction* selected = menu.exec(ui->resultsListWidget->mapToGlobal(pos));
    if (selected == detectAction)
    {
        onDetectTitleRequested();
    }
    else if (selected == editAction)
    {
        onEditTitleManually();
    }
}

void DiscoverSectionsDialog::onDetectTitleRequested()
{
    // Get selected item
    QListWidgetItem* item = ui->resultsListWidget->currentItem();
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

    // Check if title already exists in database
    if (SpecSectionDatabase::instance().exists(section) &&
        !SpecSectionDatabase::instance().hasCustomTitle(section))
    {
        QMessageBox::information(this, tr("Title Exists"),
            tr("This section already has a title in the Division 23 database:\n\n%1")
            .arg(SpecSectionDatabase::instance().getTitle(section)));
        return;
    }

    // Determine which document to search
    // For discovery dialog, we need to pick the first document that contains this section
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
               "You can manually enter a title by opening the Batch Search dialog "
               "and editing the section entry."));
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
            SpecSectionDatabase::instance().setCustomTitle(section, title);

            // Update display text
            QString displayText = section + " - " + title;
            item->setText(displayText);

            QMessageBox::information(this, tr("Title Saved"),
                tr("The custom title has been saved for this section."));
        }
    }
}

void DiscoverSectionsDialog::onEditTitleManually()
{
    // Get selected item
    QListWidgetItem* item = ui->resultsListWidget->currentItem();
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

    // Get current title if it exists
    QString currentTitle = SpecSectionDatabase::instance().getTitle(section);

    // Show input dialog for manual entry
    bool ok;
    QString newTitle = QInputDialog::getText(this, tr("Edit Section Title"),
        tr("Enter title for section %1:").arg(section),
        QLineEdit::Normal,
        currentTitle,
        &ok);

    if (ok && !newTitle.isEmpty())
    {
        // Save custom title
        SpecSectionDatabase::instance().setCustomTitle(section, newTitle);

        // Find the discovered section to get page count
        QString normalized = PageItemModel::normalizeSpecSection(section);
        if (m_discoveredSections.contains(normalized))
        {
            const DiscoveredSection& discoveredSection = m_discoveredSections[normalized];

            // Update display text with page count
            QString displayText = tr("%1 - %2 (%3 pages)")
                .arg(section)
                .arg(newTitle)
                .arg(discoveredSection.pageCount);

            item->setText(displayText);
        }
        else
        {
            // Fallback without page count
            QString displayText = section + " - " + newTitle;
            item->setText(displayText);
        }

        QMessageBox::information(this, tr("Title Saved"),
            tr("The custom title has been saved for this section."));
    }
}

void DiscoverSectionsDialog::onResultItemDoubleClicked(QListWidgetItem* item)
{
    if (!item)
    {
        return;
    }

    // Get the section from the item's user data
    QString section = item->data(Qt::UserRole).toString();
    if (section.isEmpty())
    {
        return;
    }

    // Find the discovered section
    auto it = m_discoveredSections.find(section);
    if (it == m_discoveredSections.end() || it.value().firstDocumentIndex < 0)
    {
        return;  // Not found or no location data
    }

    // Emit signal to navigate to the first occurrence
    navigateToPage(it.value().firstDocumentIndex, it.value().firstPageNumber);
}

}   // namespace pdfpagemaster
