/**********************************************************
 * @file   TextCursorHelper.cpp
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <utility>
#include "editor/document/TextCursorHelper.h"
#include "editor/document/Document.h"

namespace novelist::editor::helper {
    bool FragmentData::operator==(FragmentData const& other) const noexcept
    {
        return m_startPos == other.m_startPos
                && m_endPos == other.m_endPos
                && m_formatId == other.m_formatId;
    }

    std::ostream& operator<<(std::ostream& os, FragmentData const& fragment) noexcept
    {
        os << "{ start: " << fragment.m_startPos << ", end: " << fragment.m_endPos << ", format: " << fragment.m_formatId << " }";
        return os;
    }

    std::vector<FragmentData> overlappingFragments(Document const& doc, int pos, int anchor) noexcept
    {
        std::vector<FragmentData> fragments;
        if (pos > anchor)
            std::swap(pos, anchor);

        for (auto block = doc.m_doc->findBlock(pos);
             block.isValid()
                     && block.position() + block.length() >= pos
                     && block.position() < anchor;
             block = block.next()) {

            for (auto iter = block.begin();
                 !iter.atEnd();
                 ++iter) {
                auto fragment = iter.fragment();
                if (fragment.isValid()
                        && fragment.position() + fragment.length() > pos
                        && fragment.position() < anchor) {
                    auto formatId = doc.formatManager()->getIdOfCharFormat(fragment.charFormat());
                    if (!fragments.empty() && fragments.back().m_formatId == formatId)
                        fragments.back().m_endPos = fragment.position() + fragment.length();
                    else
                        fragments.push_back(FragmentData{fragment.position(),
                                                         fragment.position() + fragment.length(),
                                                         formatId});
                }
            }
        }
        return fragments;
    }

    void setCharacterFormats(Document const& doc, std::vector<FragmentData> const& fragments)
    {
        for (auto const& f : fragments) {
            QTextCursor cursor(doc.m_doc.get());
            cursor.setPosition(f.m_endPos);
            cursor.setPosition(f.m_startPos, QTextCursor::KeepAnchor);
            cursor.setCharFormat(*doc.formatManager()->getTextCharFormat(f.m_formatId));
        }
    }
}