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

#ifndef PDFPAGEMASTER_BATCHSEARCHDIALOG_H
#define PDFPAGEMASTER_BATCHSEARCHDIALOG_H

#include "pageitemmodel.h"

#include <QDialog>
#include <vector>

class QListWidgetItem;

namespace Ui
{
class BatchSearchDialog;
}

namespace pdfpagemaster
{

class BatchSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchSearchDialog(PageItemModel* model, QWidget* parent);
    virtual ~BatchSearchDialog() override;

private slots:
    void onAddSectionClicked();
    void onEditSectionClicked();
    void onRemoveSectionClicked();
    void onClearAllClicked();
    void onSectionListDoubleClicked(QListWidgetItem* item);
    void onSectionSelectionChanged();
    void onSearchAllClicked();
    void onExtractSelectedClicked();

private:
    struct SectionSearchResult
    {
        QString specSection;  // Original spec section entered by user
        std::vector<PageItemModel::PageRange> ranges;
    };

    void updateButtons();
    void displaySearchResults();
    void extractSeparateFiles(const std::vector<std::pair<int, int>>& selectedRanges);

    Ui::BatchSearchDialog* ui;
    PageItemModel* m_model;
    std::vector<SectionSearchResult> m_searchResults;
};

}   // namespace pdfpagemaster

#endif // PDFPAGEMASTER_BATCHSEARCHDIALOG_H
