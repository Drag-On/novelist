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
#include <QtWidgets/QUndoStack>
#include <gsl/gsl>
#include <novelist_core_export.h>
#include "TextFormatManager.h"
#include "Properties.h"

namespace novelist::editor {
    class TextCursor;

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
        explicit Document(gsl::not_null<TextFormatManager*> formatMgr, QString title, gsl::not_null<ProjectLanguage const*> lang);

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

    private:
        void onParagraphFormatChanged(int blockIdx) noexcept;

        bool needAutoTextIndent(TextFormat::WeakId thisParFormat, TextFormat::WeakId prevParFormat) const noexcept;

        Properties m_properties;
        gsl::not_null<TextFormatManager*> m_formatMgr;
        std::unique_ptr<QTextDocument> m_doc;
        QUndoStack m_undoStack;

        friend TextCursor;
    };
}

#endif //NOVELIST_DOCUMENT_H
