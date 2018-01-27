/**********************************************************
 * @file   TextFormatManager.h
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NEW_NOVELIST_TEXTFORMATMANAGER_H
#define NEW_NOVELIST_TEXTFORMATMANAGER_H

#include <QtGui/QTextBlockFormat>
#include <novelist_core_export.h>
#include "util/Identity.h"
#include "TextFormat.h"

namespace novelist {
    namespace editor {
        class Document;
        class TextCursor;

        class NOVELIST_CORE_EXPORT TextFormatManager : public QObject {
        Q_OBJECT

        public:
            using WeakId = TextFormat::WeakId;

            /**
             * Insert a new text format into the manager. It will be added after the last element.
             * @param textFormat New text format
             * @return ID of the new format
             */
            WeakId push_back(TextFormatData textFormat) noexcept;

            /**
             * Insert a new text format at a specific position into the manager.
             * @param idx Index to add new format at
             * @param textFormat New text format
             * @return ID of the new format
             */
            WeakId insert(size_t idx, TextFormatData textFormat) noexcept;

            /**
             * Remove text format at a certain index
             * @param idx Index of the format
             * @return true in case the format was removed, false with idx is out of range
             */
            bool remove(size_t idx) noexcept;

            /**
             * Change order of formats, moving the format at \p srcIdx to \p destIdx
             * @param srcIdx Source index
             * @param destIdx Destination index
             */
            void move(size_t srcIdx, size_t destIdx) noexcept;

            /**
             * Gets the format ID of the element a certain index
             * @param idx Index
             * @return Format ID of index. Invalid if index was out of range.
             * @throw std::out_of_range if passed index is out of range
             */
            WeakId idFromIndex(size_t idx) const;

            /**
             * Gets the index of the element with a certain id
             * @param id ID
             * @return Index of element with requested id. Out of bounds if id was invalid.
             * @throw std::out_of_range if passed ID is invalid
             */
            size_t indexFromId(WeakId id) const;

            /**
             * Retrieve a pointer to the text format at a certain index
             * @param idx Index of the format
             * @return Pointer to the format if found, or nullptr if index is out of bounds
             */
            TextFormat const* getTextFormat(size_t idx) const noexcept;

            /**
             * Replaces a text format.
             * @param idx Index of the format
             * @param format New format
             */
            void setTextFormat(size_t idx, TextFormatData format) noexcept;

            /**
             * @return Amount of formats currently managed
             */
            size_t size() const noexcept;

        private:
            using IdType = IdManager<TextFormatManager>::IdType;
            struct InternalTextFormat {
                IdType m_id;
                TextFormat m_textFormat{};
                QTextBlockFormat m_blockFormat{};
                QTextCharFormat m_charFormat{};
            };

            /**
             * Retrieve a pointer to the text block format associated with a given ID
             * @param id ID of the format
             * @return Pointer to the block format if found or nullptr if ID is invalid
             */
            QTextBlockFormat const* getTextBlockFormat(WeakId id) const noexcept;

            /**
             * Retrieve a pointer to the text character format associated with a given ID
             * @param id ID of the format
             * @return Pointer to the character format if found or nullptr if ID is invalid
             */
            QTextCharFormat const* getTextCharFormat(WeakId id) const noexcept;

            /**
             * Extracts the format ID from a QTextBlockFormat
             * @param format Format
             * @return The extracted ID
             * @throw std::invalid_argument if the passed format has no format ID attached to it
             */
            WeakId getIdOfBlockFormat(QTextBlockFormat const& format) const;

            /**
             * Extracts the format ID from a QTextCharFormat
             * @param format Format
             * @return The extracted ID
             * @throw std::invalid_argument if the passed format has no format ID attached to it
             */
            WeakId getIdOfCharFormat(QTextCharFormat const& format) const;

            QTextBlockFormat extractBlockFormat(TextFormat const& format, WeakId id) const noexcept;

            QTextCharFormat extractCharFormat(TextFormat const& format, WeakId id) const noexcept;

            Qt::Alignment convertAlignment(Alignment align) const noexcept;

            IdManager<TextFormatManager, WeakId> m_idMgr;
            std::vector<InternalTextFormat> m_formats;
            static int const s_typePropertyId = QTextFormat::UserProperty + 1;

            friend Document;
            friend TextCursor;
        };
    }
}

#endif //NEW_NOVELIST_TEXTFORMATMANAGER_H
