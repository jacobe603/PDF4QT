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

#ifndef PDFPAGEMASTER_DISCOVERSECTIONSDIALOG_H
#define PDFPAGEMASTER_DISCOVERSECTIONSDIALOG_H

#include "pageitemmodel.h"

#include <QDialog>
#include <QMap>

class QListWidgetItem;

namespace Ui
{
class DiscoverSectionsDialog;
}

namespace pdfpagemaster
{

class DiscoverSectionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DiscoverSectionsDialog(PageItemModel* model, QWidget* parent);
    virtual ~DiscoverSectionsDialog() override;

    // Get the list of sections that were selected for adding to batch search
    QStringList getSelectedSections() const { return m_selectedSections; }

signals:
    void sectionsSelected(const QStringList& sections);
    void navigateToPage(int documentIndex, pdf::PDFInteger pageNumber);

private slots:
    void onScanClicked();
    void onAddSelectedClicked();
    void onResultsSelectionChanged();
    void onSelectAllClicked();
    void onSelectNoneClicked();
    void onResultsContextMenu(const QPoint& pos);
    void onDetectTitleRequested();
    void onEditTitleManually();
    void onResultItemDoubleClicked(QListWidgetItem* item);

private:
    struct DiscoveredSection
    {
        QString section;          // Normalized section number
        QString displayText;      // Original format found in document
        int pageCount;            // Number of pages where this section appears
        int firstDocumentIndex = -1;        // Document index of first occurrence
        pdf::PDFInteger firstPageNumber = 0; // 1-based page number of first occurrence
    };

    void updateButtons();
    void scanDocuments();
    void displayResults();

    Ui::DiscoverSectionsDialog* ui;
    PageItemModel* m_model;
    QMap<QString, DiscoveredSection> m_discoveredSections;  // Key: normalized section
    QStringList m_selectedSections;
};

}   // namespace pdfpagemaster

#endif // PDFPAGEMASTER_DISCOVERSECTIONSDIALOG_H
