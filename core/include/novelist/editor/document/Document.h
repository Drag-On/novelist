/**********************************************************
 * @file   Document.h
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_DOCUMENT_H
#define NOVELIST_DOCUMENT_H

#include <QtCore/QObject>
#include <QtGui/QTextDocument>
#include <QtGui/QTextBlock>
#include <QtWidgets/QUndoStack>
#include <gsl/gsl>
#include <novelist_core_export.h>
#include "TextFormatManager.h"
#include "TextParagraph.h"
#include "Properties.h"

namespace novelist::editor {
    class TextCursor;
    class TextEditor;

    namespace internal {
        class TextInsertCommand;
        class TextRemoveCommand;
        class BlockInsertCommand;
        class ParagraphFormatChangeCommand;
        class CharacterFormatChangeCommand;
    }

    namespace helper {
        struct FragmentData;
        std::vector<FragmentData> overlappingFragments(Document const& doc, int pos, int anchor) noexcept;
        void setCharacterFormats(Document const& doc, std::vector<FragmentData> const& fragments);
    }

    /**
     * Text document which can be modified through TextCursor objects
     */
    class NOVELIST_CORE_EXPORT Document : public QObject {
    Q_OBJECT

    public:
        /**
         * Constructor
         * @param formatMgr Format manager object
         * @param title Document title
         * @param lang Document language
         * @throw std::invalid_argument if the passed format manager is empty
         */
        explicit Document(gsl::not_null<TextFormatManager*> formatMgr, QString title,
                gsl::not_null<ProjectLanguage const*> lang);

        /**
         * @return Undo stack of this document
         */
        QUndoStack& undoStack() noexcept;

        /**
         * @return Document properties
         */
        Properties& properties() noexcept;

        /**
         * @return Document properties
         */
        Properties const& properties() const noexcept;

        /**
         * @return Raw, unformatted text
         */
        QString toRawText() const noexcept;

        /**
         * @return true if document is empty, otherwise false
         */
        bool empty() const noexcept;

        /**
         * @return Constant iterator to the first paragraph
         */
        ParagraphIterator begin() const noexcept;

        /**
         * @return Constant iterator past the last paragraph
         */
        ParagraphIterator end() const noexcept;

        /**
         * @return The used format manager
         */
        TextFormatManager* formatManager() noexcept;

        /**
         * @return The used format manager
         */
        TextFormatManager const* formatManager() const noexcept;

    private:
        void onParagraphFormatChanged(int blockIdx) noexcept;

        void onBlockCountChanged(int newBlockCount) noexcept;

        void onFormatModified(TextFormatManager::WeakId id) noexcept;

        void onFormatReplaced(TextFormatManager::WeakId id, TextFormatManager::WeakId replacementId) noexcept;

        void updateParagraphLayout(QTextBlock block) noexcept;

        void updateIndentWidth() noexcept;

        Properties m_properties;
        gsl::not_null<TextFormatManager*> m_formatMgr;
        std::unique_ptr<QTextDocument> m_doc;
        QUndoStack m_undoStack;

        friend Properties;
        friend TextCursor;
        friend TextEditor;
        friend TextParagraph;
        friend TextFragment;
        friend ParagraphIterator;
        friend internal::TextInsertCommand;
        friend internal::TextRemoveCommand;
        friend internal::BlockInsertCommand;
        friend internal::ParagraphFormatChangeCommand;
        friend internal::CharacterFormatChangeCommand;
        friend std::vector<helper::FragmentData> helper::overlappingFragments(Document const& doc, int pos, int anchor) noexcept;
        friend void helper::setCharacterFormats(Document const& doc, std::vector<helper::FragmentData> const& fragments);
    };
}

#endif //NOVELIST_DOCUMENT_H
