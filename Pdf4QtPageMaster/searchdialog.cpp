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

#include "searchdialog.h"
#include "ui_searchdialog.h"
#include "pageitemmodel.h"

#include <QFileInfo>

namespace pdfpagemaster
{

SearchDialog::SearchDialog(PageItemModel* model, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SearchDialog),
    m_model(model)
{
    ui->setupUi(this);

    // Connect search button to search slot
    connect(ui->searchButton, &QPushButton::clicked, this, &SearchDialog::onSearchClicked);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::onSearchClicked()
{
    // Get search parameters
    QString searchText = ui->searchLineEdit->text();
    bool caseSensitive = ui->caseSensitiveCheckBox->isChecked();

    // Clear previous results
    ui->resultsListWidget->clear();

    if (searchText.isEmpty())
    {
        return;
    }

    // Perform the search
    std::vector<PageItemModel::SearchResult> results = m_model->searchText(searchText, caseSensitive);

    // Display results
    if (results.empty())
    {
        ui->resultsListWidget->addItem(tr("No results found for: %1").arg(searchText));
    }
    else
    {
        // Detect page ranges from search results
        std::vector<PageItemModel::PageRange> ranges = m_model->detectPageRanges(results);

        // Check if there are any multi-page ranges
        bool hasMultiPageRanges = false;
        for (const auto& range : ranges)
        {
            if (range.pageCount() > 1)
            {
                hasMultiPageRanges = true;
                break;
            }
        }

        // Filter ranges: if multi-page ranges exist, only show those
        std::vector<PageItemModel::PageRange> displayRanges;
        for (const auto& range : ranges)
        {
            if (!hasMultiPageRanges || range.pageCount() > 1)
            {
                displayRanges.push_back(range);
            }
        }

        // Display summary
        ui->resultsListWidget->addItem(tr("Found %1 result(s) in %2 page range(s):")
            .arg(results.size())
            .arg(displayRanges.size()));

        // Display detected page ranges
        if (!displayRanges.empty())
        {
            ui->resultsListWidget->addItem("");  // Blank line
            ui->resultsListWidget->addItem(tr("📄 Page Ranges:"));

            for (size_t i = 0; i < displayRanges.size(); ++i)
            {
                const auto& range = displayRanges[i];
                QString rangeText;

                if (range.firstPage == range.lastPage)
                {
                    // Single page
                    rangeText = tr("  Range %1: Page %2 (%3) - 1 page")
                        .arg(i + 1)
                        .arg(range.firstPage)
                        .arg(QFileInfo(range.documentName).fileName());
                }
                else
                {
                    // Multiple consecutive pages
                    rangeText = tr("  Range %1: Pages %2-%3 (%4) - %5 pages")
                        .arg(i + 1)
                        .arg(range.firstPage)
                        .arg(range.lastPage)
                        .arg(QFileInfo(range.documentName).fileName())
                        .arg(range.pageCount());
                }

                ui->resultsListWidget->addItem(rangeText);
            }
        }

        // Display individual search results
        ui->resultsListWidget->addItem("");  // Blank line
        ui->resultsListWidget->addItem(tr("🔍 Individual Results:"));

        for (const auto& result : results)
        {
            // Format: "Document - Page X: matched text (context)"
            QString itemText = QString("  %1 - Page %2: %3")
                .arg(QFileInfo(result.documentName).fileName())
                .arg(result.pageNumber)
                .arg(result.matched);

            if (!result.context.isEmpty())
            {
                itemText += QString(" (%1)").arg(result.context);
            }

            ui->resultsListWidget->addItem(itemText);
        }
    }
}

}   // namespace pdfpagemaster
