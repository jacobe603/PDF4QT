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

#include "detectsectiontitledialog.h"
#include "ui_detectsectiontitledialog.h"

namespace pdfpagemaster
{

DetectSectionTitleDialog::DetectSectionTitleDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::DetectSectionTitleDialog)
{
    ui->setupUi(this);
}

DetectSectionTitleDialog::~DetectSectionTitleDialog()
{
    delete ui;
}

void DetectSectionTitleDialog::setSection(const QString& section)
{
    ui->sectionValueLabel->setText(section);
}

void DetectSectionTitleDialog::setDetectedTitle(const QString& title)
{
    ui->titleLineEdit->setText(title);
}

QString DetectSectionTitleDialog::getTitle() const
{
    return ui->titleLineEdit->text().trimmed();
}

void DetectSectionTitleDialog::setWarningMessage(const QString& message)
{
    if (message.isEmpty())
    {
        ui->confidenceLabel->setVisible(false);
    }
    else
    {
        ui->confidenceLabel->setText(message);
        ui->confidenceLabel->setVisible(true);
    }
}

}   // namespace pdfpagemaster
