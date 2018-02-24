/**********************************************************
 * @file   TextEditorToolBar.h
 * @author jan
 * @date   2/24/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORTOOLBAR_H
#define NOVELIST_TEXTEDITORTOOLBAR_H

#include <QtWidgets/QToolBar>
#include <QtWidgets/QComboBox>
#include <gsl/gsl>
#include "TextEditor.h"
#include "util/Connection.h"

namespace novelist::editor {
    class TextEditorToolBar : public QToolBar {
    Q_OBJECT

    public:
        explicit TextEditorToolBar(gsl::not_null<QMainWindow*> parent) noexcept;

        void setEditor(TextEditor* editor) noexcept;

        TextEditor* editor() const noexcept;

    private slots:

        void onDocumentChanged(Document* document);

        void setParagraphFormat(int index);

        void setCharacterFormat(int index);

    private:
        void enableFormats(bool enabled) noexcept;

        void updateWidgets() noexcept;

        TextEditor* m_editor = nullptr;
        TextFormatManager* m_mgr = nullptr;
        QComboBox* m_parFormatComboBox;
        QComboBox* m_charFormatComboBox;
        Connection m_documentChangedConnection;
        Connection m_cursorPositionChangedConnection;
        Connection m_currentParagraphIndexChangedConnection;
        Connection m_currentCharacterIndexChangedConnection;
    };
}

#endif //NOVELIST_TEXTEDITORTOOLBAR_H
