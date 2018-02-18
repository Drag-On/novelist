/**********************************************************
 * @file   TextEditor.h
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITOR_H__
#define NOVELIST_TEXTEDITOR_H__
// TODO: Change include guards when old text editor is removed

#include <memory>
#include <variant>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTextEdit>
#include "editor/document/TextCursor.h"
#include "editor/document/Document.h"
#include "EditorActions.h"
#include "editor/sidebars/TextEditorSideBar.h"

namespace novelist::editor {
    namespace internal { class TextEdit; }

    class TextEditor : public QWidget {
    Q_OBJECT

    public:
        explicit TextEditor(QWidget* parent = nullptr) noexcept;

        void setDocument(std::unique_ptr<Document> doc) noexcept;

        Document* getDocument() noexcept;

        Document const* getDocument() const noexcept;

        TextCursor getCursor() const;

        void setCursor(TextCursor cursor);

        EditorActions const& editorActions() const noexcept;

        bool isParagraphVisible(TextParagraph const& par) const noexcept;

        std::pair<int, int> scrollBarValues() const noexcept;

        QWidget* viewport() const noexcept;

        QRect contentArea() const noexcept;

        QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

    signals:
        /**
         * Amount of blocks has changed
         * @param newBlockCount New amount of blocks
         */
        void blockCountChanged(int newBlockCount);

        /**
         * Amount of lines has changed
         * @param newLineCount New amount of lines
         */
        void lineCountChanged(int newLineCount);

    protected:
        void keyPressEvent(QKeyEvent* event) override;

        void keyReleaseEvent(QKeyEvent* event) override;

        void inputMethodEvent(QInputMethodEvent* event) override;

        bool eventFilter(QObject* obj, QEvent* event) override;

        void resizeEvent(QResizeEvent* event) override;

    private slots:
        void onUndo() noexcept;

        void onRedo() noexcept;

        void onCopy() const noexcept;

        void onCut() noexcept;

        void onPaste() noexcept;

        void onDelete() noexcept;

        void onSelectAll() noexcept;

        void onCursorPositionChanged() noexcept;

        void onSelectionChanged() noexcept;

        void onTextChanged() noexcept;

        void onBlockCountChanged(int blockCount) noexcept;

        void onLineCountChanged(int lineCount) noexcept;

        void onHorizontalScroll(int value) noexcept;

        void onVerticalScroll(int value) noexcept;

    private:
        struct ResizeUpdate {};
        struct ParagraphCountChangeUpdate {};
        struct LineCountChangeUpdate {};
        struct TextChangeUpdate {};
        struct VerticalScrollUpdate { int m_delta = 0; };
        struct HorizontalScrollUpdate { int m_delta = 0; };
        using SideBarUpdate = std::variant<ResizeUpdate, ParagraphCountChangeUpdate, LineCountChangeUpdate, TextChangeUpdate, VerticalScrollUpdate, HorizontalScrollUpdate>;

        void tryMoveCursorToUndoPos() noexcept;

        void tryMoveCursorToRedoPos() noexcept;

        bool checkDeadKeyInput(Qt::Key key) noexcept;

        QString composeInputKey(QString input) noexcept;

        void updateActions() noexcept;

        void updateSideBars(SideBarUpdate const& update) noexcept;

        void updateSideBarIfRequired(TextEditorSideBar& sideBar, SideBarUpdate const& update) noexcept;

        std::unique_ptr<Document> m_doc;
        QVBoxLayout* m_vBoxLayout;
        QHBoxLayout* m_hBoxLayout;
        internal::TextEdit* m_textEdit;
        EditorActions m_actions;
        QString m_inputModifier;
        int m_lastHorizontalSliderPos = 0;
        int m_lastVerticalSliderPos = 0;
        int m_lastBlockCount = 0;
        int m_lastLineCount = 0;
        std::vector<std::unique_ptr<TextEditorVerticalSideBar>> m_leftSideBars;
        std::vector<std::unique_ptr<TextEditorVerticalSideBar>> m_rightSideBars;
        std::vector<std::unique_ptr<TextEditorHorizontalSideBar>> m_topSideBars;
        std::vector<std::unique_ptr<TextEditorHorizontalSideBar>> m_bottomSideBars;
    };

    namespace internal {
        class TextEdit : public QTextEdit {
        Q_OBJECT

        public:
            using QTextEdit::QTextEdit;
        protected:
            void keyPressEvent(QKeyEvent* e) override;

            void keyReleaseEvent(QKeyEvent* e) override;

            bool canInsertFromMimeData(const QMimeData* /*source*/) const override;

            void insertFromMimeData(const QMimeData* /*source*/) override;

            void paintEvent(QPaintEvent* e) override;

        private:
            constexpr static bool const s_showDebugInfo = true;

            friend TextEditor;
        };
    }
}

#endif //NOVELIST_TEXTEDITOR_H
