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
#include <QtWidgets/QAction>
#include <QtCore/QTimer>
#include <QSyntaxHighlighter>
#include <QtCore/QReadWriteLock>
#include <memory>
#include "InsightModel.h"
#include "util/Connection.h"
#include "util/ConnectionWrapper.h"
#include "document/SceneDocument.h"
#include "TextEditorInsightManager.h"
#include "Inspector.h"
#include "CharacterReplacementRule.h"
#include <novelist_core_export.h>

namespace novelist {
    class TextEditor;

    namespace internal {
        /**
         * Paragraph numbering area inside a TextEditor
         */
        class ParagraphNumberArea : public QWidget {
        Q_OBJECT

        public:
            explicit ParagraphNumberArea(TextEditor* editor);

            QSize sizeHint() const override;

        protected:
            void paintEvent(QPaintEvent* event) override;

        private:
            TextEditor* m_textEditor;
        };
    }

    /**
     * Custom rich text editor with paragraph numbering
     */
    class NOVELIST_CORE_EXPORT TextEditor : public QTextEdit {
    Q_OBJECT

    public:
        explicit TextEditor(Language lang, QWidget* parent = nullptr);

        ~TextEditor() noexcept override;

        /**
         * @return Width of the paragraph number area
         */
        int paragraphNumberAreaWidth() const;

        /**
         * @param show True to show paragraph numbers, false to hide
         */
        void setShowParagraphNumberArea(bool show) noexcept;

        /**
         * @return True if paragraph numbers are shown, otherwise false
         */
        bool isShowParagraphNumberArea() const noexcept;

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
         * Makes the editor use the passed inspectors
         * @param inspectors Non-owning pointer to a vector of inspectors. The pointer must stay valid during object
         *                   lifetime or until this is called with another (possibly null) pointer.
         */
        void useInspectors(std::vector<std::unique_ptr<Inspector>> const* inspectors) noexcept;

        /**
         * Makes the editor use the passed character replacement rules
         * @param rules Non-owning pointer to a vector to rules. The pointer must stay valid during object lifetime or
         *              until this is valled with another (possibly null) pointer.
         */
        void useCharReplacement(std::vector<CharacterReplacementRule> const* rules) noexcept;

        /**
         * @return Undo action
         */
        QAction* undoAction();

        /**
         * @return Redo action
         */
        QAction* redoAction();

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

        /**
         * Provides an action that can be used to make the currently selected text into a note.
         * @return A non-owning pointer to the action
         */
        QAction* addNoteAction();

        /**
         * @return Pointer to the model containing all current insights
         */
        InsightModel* insights();

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

        void contextMenuEvent(QContextMenuEvent* e) override;

        void keyPressEvent(QKeyEvent* e) override;

        /**
         * @return The first block that is currently visible on the screen
         */
        QTextBlock firstVisibleBlock() const;

        void mouseMoveEvent(QMouseEvent* e) override;

        void insertFromMimeData(const QMimeData* source) override;

    private slots:

        void onTextChanged();

        void onCursorPositionChanged();

        void onSelectionChanged();

        void highlightCurrentLine();

        void highlightMatchingChars();

        void onBoldActionToggled(bool checked);

        void onItalicActionToggled(bool checked);

        void onUnderlineActionToggled(bool checked);

        void onOverlineActionToggled(bool checked);

        void onStrikethroughActionToggled(bool checked);

        void onSmallCapsActionToggled(bool checked);

        void makeSelectionIntoNote();

    private:
        void paintParagraphNumberArea(QPaintEvent* event);

        void updateParagraphNumberAreaWidth();

        void updateParagraphNumberArea(QRect const& rect, int dy);

        void setDefaultBlockFormat();

        std::pair<int, int> lookForMatchingChar(std::pair<QChar, QChar> const& matchingChars, int pos,
                int maxSearchLength = 1000);

        bool m_showParagraphNumberArea = true;
        std::unique_ptr<internal::ParagraphNumberArea> m_paragraphNumberArea;
        int m_lastVerticalSliderPos = 0;
        int m_lastBlockCount = 0;
        InsightModel m_insights;
        QReadWriteLock m_inspectorsLock;
        std::vector<std::unique_ptr<Inspector>> const* m_inspectors = nullptr;
        TextEditorInsightManager m_insightMgr{this};
        std::vector<CharacterReplacementRule> const* m_charReplacementRules = nullptr;
        Connection m_onBoldActionConnection;
        Connection m_onItalicActionConnection;
        Connection m_onUnderlineActionConnection;
        Connection m_onOverlineActionConnection;
        Connection m_onStrikethroughActionConnection;
        Connection m_onSmallCapsActionConnection;
        QAction* m_undoAction;
        QAction* m_redoAction;
        QAction m_boldAction{QIcon(":/icons/format-text-bold"), tr("Bold")};
        QAction m_italicAction{QIcon(":/icons/format-text-italic"), tr("Italic")};
        QAction m_underlineAction{QIcon(":/icons/format-text-underline"), tr("Underline")};
        QAction m_overlineAction{QIcon(":/icons/format-text-overline"), tr("Overline")};
        QAction m_strikethroughAction{QIcon(":/icons/format-text-strikethrough"), tr("Strikethrough")};
        QAction m_smallCapsAction{QIcon(":/icons/format-text-smallcaps"), tr("Small Caps")};
        QAction m_addNoteAction{tr("Add Note")};
        int m_highlightingMatchingChars = 0;
        std::vector<std::pair<QChar, QChar>> const m_matchingChars = {
                {QChar{40}, QChar{41}}, // ()
                {QChar{91}, QChar{93}}, // []
                {QChar{123}, QChar{125}}, // {}
                {QChar{8220}, QChar{8221}}, // “”
                {QChar{8216}, QChar{8217}}, // ‘’
                {QChar{8222}, QChar{8220}}, // „“
                {QChar{8218}, QChar{8216}}, // ‚‘
                {QChar{187}, QChar{171}}, // »«
                {QChar{8250}, QChar{8249}}, // ›‹
        };
        QColor const m_parNumberAreaColor = QColor(250, 250, 250);
        QColor const m_parNumberColor = QColor(130, 130, 130);
        QColor const m_curLineColor = QColor(255, 248, 217);
        QColor const m_matchingCharColor = QColor(153, 204, 249);
        QColor const m_noMatchingCharColor = QColor(255, 220, 220);

        constexpr static bool show_debug_info = false;

        friend class internal::ParagraphNumberArea;
        friend class TextEditorInsightManager;
    };
}

#endif //NOVELIST_TEXTEDITOR_H
