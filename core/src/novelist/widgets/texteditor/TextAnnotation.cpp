/**********************************************************
 * @file   TextAnnotation.cpp
 * @author jan
 * @date   10/9/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "widgets/texteditor/TextAnnotation.h"

namespace novelist {
    TextAnnotation::TextAnnotation(gsl::not_null<QTextDocument*> doc, int left, int right, QString msg,
            TextAnnotationType type)
            :QObject(nullptr),
             m_marker(doc, left, right, typeToFormat(type)),
             m_msg(std::move(msg))
    {
    }

    std::pair<int, int> TextAnnotation::range() const noexcept
    {
        return m_marker.range();
    }

    void TextAnnotation::setRange(int left, int right)
    {
        m_marker.setRange(left, right);
    }

    QString const& TextAnnotation::message() const noexcept
    {
        return m_msg;
    }

    void TextAnnotation::setMessage(QString msg) noexcept
    {
        m_msg = std::move(msg);
    }

    int TextAnnotation::length() const noexcept
    {
        return m_marker.length();
    }

    QTextCursor TextAnnotation::toCursor() const noexcept
    {
        return m_marker.toCursor();
    }

    QTextCharFormat const& TextAnnotation::typeToFormat(TextAnnotationType type)
    {
        switch (type) {
            case TextAnnotationType::Note:
                return s_defaultNoteFormat;
            case TextAnnotationType::Info:
                return s_defaultInfoFormat;
            case TextAnnotationType::SpellingError:
                return s_defaultSpellingFormat;
            case TextAnnotationType::GrammarError:
                return s_defaultGrammarFormat;
        }
        throw; // Should only ever happen if the enum is extended and this method isn't updated.
    }

    std::ostream& operator<<(std::ostream& s, TextAnnotation const& m) noexcept
    {
        s << m.m_marker << ": \"" << m.m_msg.toStdString() << "\"";
        return s;
    }
}