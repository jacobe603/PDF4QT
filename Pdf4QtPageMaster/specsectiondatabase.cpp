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

#include "specsectiondatabase.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

namespace pdfpagemaster
{

SpecSectionDatabase& SpecSectionDatabase::instance()
{
    static SpecSectionDatabase instance;
    return instance;
}

bool SpecSectionDatabase::loadFromResource()
{
    // Clear existing data
    m_sections.clear();

    // Open resource file
    QFile file(":/pdfpagemaster/resources/masterformat_div23.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream in(&file);

    // Skip header line
    if (!in.atEnd())
    {
        in.readLine();
    }

    // Read data lines
    int lineNumber = 1;
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        lineNumber++;

        // Skip empty lines
        if (line.isEmpty())
        {
            continue;
        }

        // Parse CSV: "Section Number","Section Title"
        // Handle quoted fields with commas
        QStringList parts;
        QRegularExpression csvRegex("(?:^|,)(\"(?:[^\"]|\"\")*\"|[^,]*)");
        QRegularExpressionMatchIterator matches = csvRegex.globalMatch(line);

        while (matches.hasNext())
        {
            QRegularExpressionMatch match = matches.next();
            QString field = match.captured(1);

            // Remove quotes and unescape doubled quotes
            if (field.startsWith('"') && field.endsWith('"'))
            {
                field = field.mid(1, field.length() - 2);
                field.replace("\"\"", "\"");
            }

            parts << field.trimmed();
        }

        // Validate we have at least 2 fields
        if (parts.size() < 2)
        {
            continue;
        }

        QString section = parts[0];
        QString title = parts[1];

        // Normalize section number and store
        QString normalizedSection = normalize(section);
        if (!normalizedSection.isEmpty() && !title.isEmpty())
        {
            m_sections[normalizedSection] = title;
        }
    }

    file.close();
    return !m_sections.isEmpty();
}

QString SpecSectionDatabase::getTitle(const QString& section) const
{
    QString normalizedSection = normalize(section);
    return m_sections.value(normalizedSection, QString());
}

bool SpecSectionDatabase::exists(const QString& section) const
{
    QString normalizedSection = normalize(section);
    return m_sections.contains(normalizedSection);
}

QStringList SpecSectionDatabase::getAllSections() const
{
    return m_sections.keys();
}

QString SpecSectionDatabase::normalize(const QString& section)
{
    // Remove all spaces, dashes, underscores
    // "23 36 00" -> "233600"
    // "23-36-00" -> "233600"
    // "23_36_00" -> "233600"
    QString normalized = section;
    normalized.remove(' ');
    normalized.remove('-');
    normalized.remove('_');
    return normalized.trimmed();
}

}   // namespace pdfpagemaster
