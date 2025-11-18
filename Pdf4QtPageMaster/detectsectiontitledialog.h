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

#ifndef PDFPAGEMASTER_DETECTSECTIONTITLEDIALOG_H
#define PDFPAGEMASTER_DETECTSECTIONTITLEDIALOG_H

#include <QDialog>

namespace Ui {
class DetectSectionTitleDialog;
}

namespace pdfpagemaster
{

/// Dialog for confirming and editing detected spec section titles
class DetectSectionTitleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DetectSectionTitleDialog(QWidget* parent = nullptr);
    ~DetectSectionTitleDialog();

    /// Set the section number to display
    /// @param section Section number (will be shown as-is)
    void setSection(const QString& section);

    /// Set the detected title (pre-populates the edit field)
    /// @param title Detected title
    void setDetectedTitle(const QString& title);

    /// Get the title entered by the user
    /// @return Title from the line edit
    QString getTitle() const;

    /// Show a warning message (for low confidence detections)
    /// @param message Warning message to display
    void setWarningMessage(const QString& message);

private:
    Ui::DetectSectionTitleDialog* ui;
};

}   // namespace pdfpagemaster

#endif // PDFPAGEMASTER_DETECTSECTIONTITLEDIALOG_H
