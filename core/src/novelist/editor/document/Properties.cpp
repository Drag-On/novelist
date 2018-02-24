/**********************************************************
 * @file   Properties.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/Properties.h"
#include "editor/document/Document.h"

namespace novelist::editor {
    QString const& Properties::title() const noexcept
    {
        return m_title;
    }

    void Properties::setTitle(QString title) noexcept
    {
        m_title = std::move(title);
        emit changed();
    }

    ProjectLanguage const& Properties::language() const noexcept
    {
        return *m_lang;
    }

    void Properties::setLanguage(gsl::not_null<ProjectLanguage*> lang) noexcept
    {
        m_lang = lang;
        emit changed();
    }

    size_t Properties::countParagraphs() const noexcept
    {
        return m_doc->m_doc->blockCount();
    }

    size_t Properties::countLines() const noexcept
    {
        size_t count = 0;
        for (auto b = m_doc->m_doc->begin(); b != m_doc->m_doc->end(); b = b.next())
            count += b.layout()->lineCount();
        return count;
    }

    size_t Properties::countWords() const noexcept
    {
        return novelist::editor::countWords(m_doc->toRawText(), *m_lang);
    }

    size_t Properties::countCharacters() const noexcept
    {
        size_t charCount = 0;
        auto const text = m_doc->toRawText();
        for (auto const& c : text) {
            if (!c.isHighSurrogate() && !c.isNonCharacter() && c != QChar::SpecialCharacter::LineFeed)
                ++charCount;
        }
        return charCount;
    }

    Properties::Properties(Document* doc, QString title, gsl::not_null<ProjectLanguage const*> lang)
            :m_doc(doc),
             m_title(std::move(title)),
             m_lang(lang)
    {
    }
}