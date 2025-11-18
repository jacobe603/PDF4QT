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
#include <QSettings>

namespace pdfpagemaster
{

// QSettings keys for custom titles
const QString SpecSectionDatabase::SETTINGS_GROUP = "PageMaster";
const QString SpecSectionDatabase::CUSTOM_TITLES_KEY = "CustomSpecTitles";

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
    loadCustomTitles();

    QString normalizedSection = normalize(section);

    // Check custom titles first (higher priority)
    if (m_customTitles.contains(normalizedSection))
    {
        return m_customTitles.value(normalizedSection);
    }

    // Fallback to CSV database
    return m_sections.value(normalizedSection, QString());
}

bool SpecSectionDatabase::exists(const QString& section) const
{
    loadCustomTitles();

    QString normalizedSection = normalize(section);

    // Check both custom titles and CSV database
    return m_customTitles.contains(normalizedSection) || m_sections.contains(normalizedSection);
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

void SpecSectionDatabase::loadCustomTitles() const
{
    if (m_customTitlesLoaded)
    {
        return;
    }

    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    int size = settings.beginReadArray(CUSTOM_TITLES_KEY);

    for (int i = 0; i < size; ++i)
    {
        settings.setArrayIndex(i);
        QString section = settings.value("section").toString();
        QString title = settings.value("title").toString();

        if (!section.isEmpty() && !title.isEmpty())
        {
            m_customTitles[normalize(section)] = title;
        }
    }

    settings.endArray();
    settings.endGroup();
    m_customTitlesLoaded = true;
}

void SpecSectionDatabase::saveCustomTitles() const
{
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    settings.beginWriteArray(CUSTOM_TITLES_KEY);

    int index = 0;
    for (auto it = m_customTitles.begin(); it != m_customTitles.end(); ++it, ++index)
    {
        settings.setArrayIndex(index);
        settings.setValue("section", it.key());
        settings.setValue("title", it.value());
    }

    settings.endArray();
    settings.endGroup();
}

void SpecSectionDatabase::setCustomTitle(const QString& section, const QString& title)
{
    loadCustomTitles();

    QString normalized = normalize(section);

    if (title.isEmpty())
    {
        // Remove custom title if empty
        m_customTitles.remove(normalized);
    }
    else
    {
        // Set or update custom title
        m_customTitles[normalized] = title;
    }

    saveCustomTitles();
}

bool SpecSectionDatabase::hasCustomTitle(const QString& section) const
{
    loadCustomTitles();

    QString normalized = normalize(section);
    return m_customTitles.contains(normalized);
}

QString SpecSectionDatabase::getCustomTitle(const QString& section) const
{
    loadCustomTitles();

    QString normalized = normalize(section);
    return m_customTitles.value(normalized, QString());
}

void SpecSectionDatabase::removeCustomTitle(const QString& section)
{
    loadCustomTitles();

    QString normalized = normalize(section);
    m_customTitles.remove(normalized);

    saveCustomTitles();
}

}   // namespace pdfpagemaster
