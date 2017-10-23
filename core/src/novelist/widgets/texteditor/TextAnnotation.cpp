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
            InsightType type)
            :QObject(nullptr),
             m_marker(doc, left, right, insightTypeToFormat(type)),
             m_msg(std::move(msg)),
             m_type(type)
    {
        connect(&m_marker, &TextMarker::collapsed, [this] () { emit collapsed(this); });
    }

    std::pair<int, int> TextAnnotation::parRange() const noexcept
    {
        return m_marker.parRange();
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

    InsightType TextAnnotation::type() const noexcept
    {
        return m_type;
    }

    int TextAnnotation::length() const noexcept
    {
        return m_marker.length();
    }

    QTextCursor TextAnnotation::toCursor() const noexcept
    {
        return m_marker.toCursor();
    }

    std::ostream& operator<<(std::ostream& s, TextAnnotation const& m) noexcept
    {
        s << m.m_marker << ": \"" << m.m_msg.toStdString() << "\"";
        return s;
    }
}