/**********************************************************
 * @file   Properties.h
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROPERTIES_H
#define NOVELIST_PROPERTIES_H

#include <QtCore/QObject>
#include <gsl/gsl>
#include "Language.h"

namespace novelist::editor {
    class Document;

    class Properties : public QObject {
    Q_OBJECT

    public:
        QString const& title() const noexcept;

        void setTitle(QString title) noexcept;

        ProjectLanguage const& language() const noexcept;

        void setLanguage(gsl::not_null<ProjectLanguage*> lang) noexcept;

        size_t countParagraphs() const noexcept;

        size_t countLines() const noexcept;

        size_t countWords() const noexcept;

        size_t countCharacters() const noexcept;

    signals:

        void changed();

    private:
        Properties(Document* doc, QString title, gsl::not_null<ProjectLanguage*> lang);

        Document* m_doc;
        QString m_title;
        gsl::not_null<ProjectLanguage*> m_lang;

        friend Document;
    };
}

#endif //NOVELIST_PROPERTIES_H
