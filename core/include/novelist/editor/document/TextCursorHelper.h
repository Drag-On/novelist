/**********************************************************
 * @file   TextCursorHelper.h
 * @author jan
 * @date   3/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTCURSORHELPER_H
#define NOVELIST_TEXTCURSORHELPER_H

#include <vector>
#include <iostream>
#include "TextFormat.h"

namespace novelist::editor {
    class Document;

    namespace helper {
        struct FragmentData {
            int m_startPos;
            int m_endPos;
            TextFormat::WeakId m_formatId;

            bool operator==(FragmentData const& other) const noexcept;
        };
        std::ostream& operator<<(std::ostream& os, FragmentData const& fragment) noexcept;

        /**
         * Compiles a list of fragment that overlap with a given character range
         * @note The first and last element in the list might not start/end exactly with the provided positions.
         * @param doc Document
         * @param pos Character position
         * @param anchor End position, might be identical to \p pos
         * @return List of fragments
         */
        std::vector<FragmentData> overlappingFragments(Document const& doc, int pos, int anchor) noexcept;

        /**
         * Sets the character format of the given fragments on a document
         * @param doc Document
         * @param fragments Fragment data to set
         * @throw Might throw if the given fragments are out of range
         */
        void setCharacterFormats(Document const& doc, std::vector<FragmentData> const& fragments);
    }
}


#endif //NOVELIST_TEXTCURSORHELPER_H
