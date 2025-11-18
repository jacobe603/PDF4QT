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

#ifndef PDFPAGEMASTER_SPECSECTIONDATABASE_H
#define PDFPAGEMASTER_SPECSECTIONDATABASE_H

#include <QString>
#include <QMap>

namespace pdfpagemaster
{

/// Database of MasterFormat Division 23 (HVAC) spec sections
/// Provides spec section number to title lookup
class SpecSectionDatabase
{
public:
    /// Get singleton instance
    static SpecSectionDatabase& instance();

    /// Load database from embedded resource
    /// @return true if loaded successfully
    bool loadFromResource();

    /// Get title for a spec section
    /// @param section Spec section number (e.g., "23 36 00", "233600", "23-36-00")
    /// @return Title if found, empty string otherwise
    QString getTitle(const QString& section) const;

    /// Check if spec section exists in database
    /// @param section Spec section number (any format)
    /// @return true if section exists
    bool exists(const QString& section) const;

    /// Get all spec section numbers (normalized format)
    /// @return List of all section numbers
    QStringList getAllSections() const;

    /// Get count of loaded spec sections
    /// @return Number of sections in database
    int count() const { return m_sections.size(); }

    /// Check if database is loaded
    /// @return true if database has been loaded
    bool isLoaded() const { return !m_sections.isEmpty(); }

private:
    SpecSectionDatabase() = default;
    ~SpecSectionDatabase() = default;

    // Prevent copying
    SpecSectionDatabase(const SpecSectionDatabase&) = delete;
    SpecSectionDatabase& operator=(const SpecSectionDatabase&) = delete;

    /// Normalize spec section number for lookup
    /// Removes spaces, dashes, underscores: "23 36 00" -> "233600"
    static QString normalize(const QString& section);

    QMap<QString, QString> m_sections;  // normalized section -> title
};

}   // namespace pdfpagemaster

#endif // PDFPAGEMASTER_SPECSECTIONDATABASE_H
