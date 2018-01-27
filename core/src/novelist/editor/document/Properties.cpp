/**********************************************************
 * @file   Properties.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/document/Properties.h"

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
        return 0; // TODO: implement
    }

    size_t Properties::countWords() const noexcept
    {
        return 0; // TODO: implement
    }

    size_t Properties::countCharacters() const noexcept
    {
        return 0; // TODO: implement
    }

    Properties::Properties(Document* doc, QString title, gsl::not_null<ProjectLanguage const*> lang)
            :m_doc(doc),
             m_title(std::move(title)),
             m_lang(lang)
    {
    }
}