/**********************************************************
 * @file   TextEditor.h
 * @author jan
 * @date   8/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITOR_H
#define NOVELIST_TEXTEDITOR_H

#include <QtWidgets/QTextEdit>
#include <memory>
#include <QtWidgets/QAction>
#include "util/Connection.h"
#include "util/ConnectionWrapper.h"
#include "datastructures/SceneDocument.h"

namespace novelist {
    namespace internal {
        class ParagraphNumberArea;
    }

    /**
     * Custom rich text editor with paragraph numbering
     */
    class TextEditor : public QTextEdit {
    Q_OBJECT

    public:
        explicit TextEditor(QWidget* parent = nullptr);

        ~TextEditor() noexcept override;

        /**
         * @return Width of the paragraph number area
         */
        int paragraphNumberAreaWidth() const;

        /**
         * @return Underlying document
         */
        SceneDocument* document() const;

        /**
         * @param document New document
         */
        void setDocument(SceneDocument* document);

        /**
         * @param document New document
         * @throws std::runtime_error if the passed document isn't a SceneDocument
         */
        void setDocument(QTextDocument* document);

        /**
         * @return true if there are undoable changes to the document, false otherwise
         */
        bool canUndo() const;

        /**
         * @return true if there are redoable changes to the document, false otherwise
         */
        bool canRedo() const;

        /**
         * Provides an action that can be used to make the currently selected text bold. It will also change checked
         * state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* boldAction();

        /**
         * Provides an action that can be used to make the currently selected text italic. It will also change checked
         * state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* italicAction();

        /**
         * Provides an action that can be used to make the currently selected text underlined. It will also change
         * checked state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* underlineAction();

        /**
         * Provides an action that can be used to make the currently selected text overlined. It will also change
         * checked state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* overlineAction();

        /**
         * Provides an action that can be used to make the currently selected text strikethrough. It will also change
         * checked state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* strikethroughAction();

        /**
         * Provides an action that can be used to make the currently selected text small caps. It will also change
         * checked state depending on the currently selected text.
         * @return A non-owning pointer to the action
         */
        QAction* smallCapsAction();

    signals:

        /**
         * Amount of blocks has changed
         * @param newBlockCount New amount of blocks
         */
        void blockCountChanged(int newBlockCount);

        /**
         * Fires when the widget received focus or lost it
         * @param focused True when focus was gained, otherwise false
         */
        void focusReceived(bool focused);

    protected:
        void resizeEvent(QResizeEvent* e) override;

        void paintEvent(QPaintEvent* e) override;

        void focusInEvent(QFocusEvent* e) override;

        void focusOutEvent(QFocusEvent* e) override;

        /**
         * @return The first block that is currently visible on the screen
         */
        QTextBlock firstVisibleBlock() const;

    private slots:

        void onTextChanged();

        void onCursorPositionChanged();

        void highlightCurrentLine();

    private:
        void paintParagraphNumberArea(QPaintEvent* event);

        void updateParagraphNumberAreaWidth();

        void updateParagraphNumberArea(QRect const& rect, int dy);

        void setDefaultBlockFormat();

        void onBoldActionToggled(bool checked);

        void onItalicActionToggled(bool checked);

        void onUnderlineActionToggled(bool checked);

        void onOverlineActionToggled(bool checked);

        void onStrikethroughActionToggled(bool checked);

        void onSmallCapsActionToggled(bool checked);

        std::unique_ptr<internal::ParagraphNumberArea> m_paragraphNumberArea;
        int m_lastVerticalSliderPos = 0;
        int m_lastBlockCount = 0;
        Connection m_onBoldActionConnection;
        Connection m_onItalicActionConnection;
        Connection m_onUnderlineActionConnection;
        Connection m_onOverlineActionConnection;
        Connection m_onStrikethroughActionConnection;
        Connection m_onSmallCapsActionConnection;
        QAction m_boldAction{tr("Bold")};
        QAction m_italicAction{tr("Italic")};
        QAction m_underlineAction{tr("Underline")};
        QAction m_overlineAction{tr("Overline")};
        QAction m_strikethroughAction{tr("Strikethrough")};
        QAction m_smallCapsAction{tr("Small Caps")};
        QColor const m_parNumberAreaColor = QColor(250, 250, 250);
        QColor const m_parNumberColor = QColor(130, 130, 130);
        QColor const m_curLineColor = QColor(255, 248, 217);

        constexpr static bool show_debug_info = false;

        friend class internal::ParagraphNumberArea;
    };

    namespace internal {
        /**
         * Paragraph numbering area inside a TextEditor
         */
        class ParagraphNumberArea : public QWidget {
        public:
            explicit ParagraphNumberArea(TextEditor* editor);

            QSize sizeHint() const override;

        protected:
            void paintEvent(QPaintEvent* event) override;

        private:
            TextEditor* m_textEditor;
        };
    }
}

#endif //NOVELIST_TEXTEDITOR_H
