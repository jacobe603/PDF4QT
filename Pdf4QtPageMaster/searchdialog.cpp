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

    // TODO: Implement actual search functionality in Phase 1, Task 1.4
    // For now, just add a placeholder message
    ui->resultsListWidget->addItem(tr("Search functionality will be implemented in Task 1.4"));
    ui->resultsListWidget->addItem(tr("Searching for: %1 (Case sensitive: %2)")
                                    .arg(searchText)
                                    .arg(caseSensitive ? tr("Yes") : tr("No")));
}

}   // namespace pdfpagemaster
