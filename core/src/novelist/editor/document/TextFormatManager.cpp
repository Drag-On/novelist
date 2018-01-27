/**********************************************************
 * @file   TextFormatManager.cpp
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QDebug>
#include "datastructures/SortedVector.h"
#include "editor/document/TextFormatManager.h"

namespace novelist::editor {
    TextFormatManager::WeakId TextFormatManager::push_back(TextFormatData textFormat) noexcept
    {
        auto id = m_idMgr.generate();
        InternalTextFormat format{std::move(id)};
        auto weakId = format.m_id.id();
        format.m_textFormat.m_id = weakId;
        format.m_textFormat.m_data = std::move(textFormat);
        format.m_blockFormat = extractBlockFormat(format.m_textFormat, format.m_id.id());
        format.m_charFormat = extractCharFormat(format.m_textFormat, format.m_id.id());
        m_formats.push_back(std::move(format));
        return weakId;
    }

    TextFormatManager::WeakId TextFormatManager::insert(size_t idx, TextFormatData textFormat) noexcept
    {
        if (idx > m_formats.size())
            return std::numeric_limits<WeakId>::max();

        auto id = m_idMgr.generate();
        InternalTextFormat format{std::move(id)};
        auto weakId = format.m_id.id();
        format.m_textFormat.m_id = weakId;
        format.m_textFormat.m_data = std::move(textFormat);
        format.m_blockFormat = extractBlockFormat(format.m_textFormat, format.m_id.id());
        format.m_charFormat = extractCharFormat(format.m_textFormat, format.m_id.id());
        m_formats.insert(m_formats.begin() + idx, std::move(format));
        return weakId;
    }

    bool TextFormatManager::remove(size_t idx) noexcept
    {
        if (m_formats.size() < idx)
        {
            m_formats.erase(m_formats.begin() + idx);
            return true;
        }
        else
            return false;
    }

    void TextFormatManager::move(size_t srcIdx, size_t destIdx) noexcept
    {
        move_range(m_formats.begin() + srcIdx, 1, m_formats.begin() + destIdx);
    }

    TextFormatManager::WeakId TextFormatManager::idFromIndex(size_t idx) const
    {
        if (idx < m_formats.size())
            return m_formats[idx].m_id.id();
        qWarning() << "Converted out-of-bounds index" << idx << "to format id";
        throw std::out_of_range("Index " + std::to_string(idx) + " is out of range.");
    }

    size_t TextFormatManager::indexFromId(TextFormatManager::WeakId id) const
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == id;
                });
        if (iter != m_formats.end())
            return static_cast<size_t>(std::distance(m_formats.begin(), iter));
        qWarning() << "Converted invalid format id" << static_cast<uint32_t>(id) << "to index";
        throw std::out_of_range("ID " + std::to_string(id) + " is out of range.");
    }

    TextFormat const* TextFormatManager::getTextFormat(size_t idx) const noexcept
    {
        if (idx < m_formats.size())
            return &m_formats[idx].m_textFormat;
        return nullptr;
    }

    void TextFormatManager::setTextFormat(size_t idx, TextFormatData format) noexcept
    {
        if (m_formats.size() > idx) {
            auto& foundFormat = m_formats[idx];
            foundFormat.m_textFormat.m_data = std::move(format);
            foundFormat.m_blockFormat = extractBlockFormat(foundFormat.m_textFormat, foundFormat.m_id.id());
            foundFormat.m_charFormat = extractCharFormat(foundFormat.m_textFormat, foundFormat.m_id.id());
        }
        else
            qWarning() << "Tried to change out-of-bounds text format with idx" << idx;
    }

    size_t TextFormatManager::size() const noexcept
    {
        return m_formats.size();
    }

    QTextBlockFormat const* TextFormatManager::getTextBlockFormat(TextFormatManager::WeakId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return &iter->m_blockFormat;
        return nullptr;
    }

    QTextCharFormat const* TextFormatManager::getTextCharFormat(TextFormatManager::WeakId id) const noexcept
    {
        auto iter = std::find_if(m_formats.begin(), m_formats.end(),
                [&id](auto const& f) {
                    return f.m_id.id() == static_cast<uint32_t>(id);
                });
        if (iter != m_formats.end())
            return &iter->m_charFormat;
        return nullptr;
    }

    TextFormatManager::WeakId TextFormatManager::getIdOfBlockFormat(QTextBlockFormat const& format) const
    {
        auto idVariant = format.property(s_typePropertyId);
        if (idVariant.isValid())
            return qvariant_cast<WeakId>(idVariant);

        qWarning() << "The supplied block format doesn't have a format ID attached to it.";
        throw std::invalid_argument("No ID attached to block format");
    }

    TextFormatManager::WeakId TextFormatManager::getIdOfCharFormat(QTextCharFormat const& format) const
    {
        auto idVariant = format.property(s_typePropertyId);
        if (idVariant.isValid())
            return qvariant_cast<WeakId>(idVariant);

        qWarning() << "The supplied character format doesn't have a format ID attached to it.";
        throw std::invalid_argument("No ID attached to character format");
    }

    QTextBlockFormat TextFormatManager::extractBlockFormat(TextFormat const& format, WeakId id) const noexcept
    {
        QTextBlockFormat blockFormat;
        blockFormat.setAlignment(convertAlignment(format.m_data.m_alignment));
        blockFormat.setLeftMargin(format.m_data.m_margin.m_left);
        blockFormat.setRightMargin(format.m_data.m_margin.m_right);
        blockFormat.setTopMargin(format.m_data.m_margin.m_top);
        blockFormat.setBottomMargin(format.m_data.m_margin.m_bottom);
        blockFormat.setIndent(format.m_data.m_indentation.m_indent);
        blockFormat.setTextIndent(format.m_data.m_indentation.m_textIndent);
        blockFormat.setProperty(s_typePropertyId, id);
        return blockFormat;
    }

    QTextCharFormat TextFormatManager::extractCharFormat(TextFormat const& format, WeakId id) const noexcept
    {
        QTextCharFormat charFormat;
        QFont font;
        font.setBold(format.m_data.m_characterFormat.m_bold);
        font.setItalic(format.m_data.m_characterFormat.m_italic);
        font.setUnderline(format.m_data.m_characterFormat.m_underline);
        font.setOverline(format.m_data.m_characterFormat.m_overline);
        font.setStrikeOut(format.m_data.m_characterFormat.m_strikethrough);
        font.setCapitalization(format.m_data.m_characterFormat.m_smallCaps ? QFont::SmallCaps : QFont::MixedCase);
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