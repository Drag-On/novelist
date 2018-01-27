/**********************************************************
 * @file   TextFormat.h
 * @author jan
 * @date   1/27/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTFORMAT_H
#define NOVELIST_TEXTFORMAT_H

#include <QtCore/QString>

namespace novelist::editor {
    /**
     * Paragraph alignment
     */
    enum class Alignment {
        Left,
        Right,
        Center,
        Fill,
    };

    /**
     * Paragraph margin
     */
    struct Margin {
        uint32_t m_left = 0;
        uint32_t m_right = 0;
        uint32_t m_top = 0;
        uint32_t m_bottom = 0;
    };

    /**
     * Paragraph indentation
     */
    struct Indentation {
        uint32_t m_indent = 0;
        uint32_t m_textIndent = 0;
        bool m_autoTextIndent = false;
    };

    /**
     * Character format
     */
    struct CharacterFormat {
        bool m_bold = false;
        bool m_italic = false;
        bool m_underline = false;
        bool m_overline = false;
        bool m_strikethrough = false;
        bool m_smallCaps = false;
    };

    /**
     * Text format data
     */
    struct TextFormatData {
        QString m_name{};
        Alignment m_alignment = Alignment::Left;
        Margin m_margin{};
        Indentation m_indentation{};
        CharacterFormat m_characterFormat{};
    };

    /**
     * Text format including Id
     */
    struct TextFormat {
        using WeakId = uint32_t;

        WeakId m_id;
        TextFormatData m_data;
    };
}

#endif //NOVELIST_TEXTFORMAT_H
