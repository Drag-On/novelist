/**********************************************************
 * @file   TextFormatManager.cpp
 * @author jan
 * @date   1/19/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "document/TextFormatManager.h"
#include <QDebug>
#include "datastructures/SortedVector.h"

namespace novelist {
    TextFormatManager::FormatId
    TextFormatManager::push_back(TextFormat textFormat) noexcept
    {
        auto id = m_idMgr.generate();
        InternalTextFormat format{std::move(id)};
        format.m_textFormat = std::move(textFormat);
        format.m_blockFormat = extractBlockFormat(format.m_textFormat, format.m_id.id());
        format.m_charFormat = extractCharFormat(format.m_textFormat, format.m_id.id());
        m_formats.push_back(std::move(format));
        return FormatId{m_formats.back().m_id.id()};
    }

    TextFormatManager::FormatId TextFormatManager::insert(size_t idx, TextFormatManager::TextFormat textFormat) noexcept
    {
        if (idx > m_formats.size())
            return TextFormatManager::FormatId(std::numeric_limits<uint32_t>::max());

        auto id = m_idMgr.generate();
        InternalTextFormat format{std::move(id)};
        format.m_textFormat = std::move(textFormat);
        format.m_blockFormat = extractBlockFormat(format.m_textFormat, format.m_id.id());
        format.m_charFormat = extractCharFormat(format.m_textFormat, format.m_id.id());
        m_formats.insert(m_formats.begin() + idx, std::move(format));
        return FormatId{(m_formats.begin() + idx)->m_id.id()};
    }

    bool TextFormatManager::remove(TextFormatManager::FormatId id) noexcept
    {
        return m_formats.erase(
                std::find_if(m_formats.begin(), m_formats.end(), [&id](auto const& p) {
                    return p.m_id.id() == static_cast<uint32_t>(id);
                })) != m_formats.end();
    }

    void TextFormatManager::move(size_t srcIdx, size_t destIdx) noexcept
    {
        move_range(m_formats.begin() + srcIdx, 1, m_formats.begin() + destIdx);
    }

    TextFormatManager::FormatId TextFormatManager::idFromIndex(size_t idx) const noexcept
    {
        if (idx < m_formats.size())
            return FormatId{m_formats[idx].m_id.id()};
        qWarning() << "Converted out-of-bounds index" << idx << "to format id";
        return FormatId(std::numeric_limits<uint32_t>::max());
    }

    size_t TextFormatManager::indexFromId(TextFormatManager::FormatId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return static_cast<size_t>(std::distance(m_formats.begin(), iter));
        qWarning() << "Converted invalid format id" << static_cast<uint32_t>(id) << "to index";
        return std::numeric_limits<size_t>::max();
    }

    TextFormatManager::TextFormat const* TextFormatManager::getTextFormat(size_t idx) const noexcept
    {
        if (idx < m_formats.size())
            return &m_formats[idx].m_textFormat;
        return nullptr;
    }

    TextFormatManager::TextFormat const* TextFormatManager::getTextFormat(FormatId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return &iter->m_textFormat;
        return nullptr;
    }

    void TextFormatManager::setTextFormat(TextFormatManager::FormatId id, TextFormatManager::TextFormat format) noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end()) {
            iter->m_textFormat = std::move(format);
            iter->m_blockFormat = extractBlockFormat(iter->m_textFormat, iter->m_id.id());
            iter->m_charFormat = extractCharFormat(iter->m_textFormat, iter->m_id.id());
        }
        else
            qWarning() << "Tried to change unknown text format with id" << static_cast<uint32_t>(id);
    }

    QTextBlockFormat const* TextFormatManager::getTextBlockFormat(FormatId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return &iter->m_blockFormat;
        return nullptr;
    }

    QTextCharFormat const* TextFormatManager::getTextCharFormat(FormatId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return &iter->m_charFormat;
        return nullptr;
    }

    size_t TextFormatManager::size() const noexcept
    {
        return m_formats.size();
    }

    QTextBlockFormat TextFormatManager::extractBlockFormat(TextFormat const& format, uint32_t id) const noexcept
    {
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment(convertAlignment(format.m_alignment));
        blockFormat.setLeftMargin(format.m_margin.m_left);
        blockFormat.setRightMargin(format.m_margin.m_right);
        blockFormat.setTopMargin(format.m_margin.m_top);
        blockFormat.setBottomMargin(format.m_margin.m_bottom);
        blockFormat.setIndent(format.m_indentation.m_indent);
        blockFormat.setTextIndent(format.m_indentation.m_textIndent);
        blockFormat.setProperty(s_typePropertyId, id);
        return blockFormat;
    }

    QTextCharFormat TextFormatManager::extractCharFormat(TextFormat const& format, uint32_t id) const noexcept
    {
        QTextCharFormat charFormat;
        QFont font;
        font.setBold(format.m_characterFormat.m_bold);
        font.setItalic(format.m_characterFormat.m_italic);
        font.setUnderline(format.m_characterFormat.m_underline);
        font.setOverline(format.m_characterFormat.m_overline);
        font.setStrikeOut(format.m_characterFormat.m_strikethrough);
        font.setCapitalization(format.m_characterFormat.m_smallCaps ? QFont::SmallCaps : QFont::MixedCase);
        charFormat.setFont(font);
        charFormat.setProperty(s_typePropertyId, id);
        return charFormat;
    }

    Qt::Alignment TextFormatManager::convertAlignment(Alignment align) const noexcept
    {
        switch (align) {
            case Alignment::Left:
                return Qt::AlignLeft;
            case Alignment::Right:
                return Qt::AlignRight;
            case Alignment::Center:
                return Qt::AlignVCenter;
            case Alignment::Fill:
                return Qt::AlignJustify;
            default:
                qWarning() << "Alignment flag not handled";
        }
        return Qt::AlignLeft;
    }
}