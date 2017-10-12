/**********************************************************
 * @file   TextMarker.cpp
 * @author jan
 * @date   10/11/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtGui/QTextBlock>
#include <QDebug>
#include "document/TextMarker.h"

namespace novelist {

    TextMarker::TextMarker(gsl::not_null<QTextDocument*> document, int position, TextMarkerAttachment attachment)
            :m_doc(document),
             m_pos(position),
             m_attachment(attachment)
    {
        int const lastBlockPos = m_doc->lastBlock().position();
        int const lastBlockLength = m_doc->lastBlock().length();
        int const lastPos = lastBlockPos + lastBlockLength;
        if (m_pos < 0 || m_pos > lastPos)
            throw std::out_of_range(
                    "The position " + std::to_string(m_pos) + " is not in the allowed range of the document [0, "
                            + std::to_string(lastPos) + "].");

        connect(m_doc, &QTextDocument::contentsChange, this, &TextMarker::onContentsChange, Qt::ConnectionType::DirectConnection);
    }

    int TextMarker::position() const noexcept
    {
        return m_pos;
    }

    std::pair<int, int> TextMarker::blockNoAndPos() const noexcept
    {
        auto block = m_doc->findBlock(m_pos);
        return std::make_pair<int, int>(block.blockNumber(), m_pos - block.position());
    }

    TextMarkerAttachment TextMarker::attachment() const noexcept
    {
        return m_attachment;
    }

    void TextMarker::setAttachment(TextMarkerAttachment attachment) noexcept
    {
        m_attachment = attachment;
    }

    void TextMarker::onContentsChange(int position, int charsRemoved, int charsAdded)
    {
        if (m_pos > position) {
            if(position + charsRemoved > m_pos) {
                int oldPos = m_pos;
                m_pos -= m_pos - position;
                if(m_attachment == TextMarkerAttachment::AttachRight)
                    m_pos += charsAdded;
                emit removed(oldPos);
            }
            else {
                m_pos -= std::min(charsRemoved, m_pos - position);
                m_pos += charsAdded;
            }
        }
    }
}