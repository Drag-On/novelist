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

    signals:

        /**
         * Amount of blocks has changed
         * @param newBlockCount New amount of blocks
         */
        void blockCountChanged(int newBlockCount);

    protected:
        void resizeEvent(QResizeEvent* e) override;

        void paintEvent(QPaintEvent* e) override;

        /**
         * @return The first block that is currently visible on the screen
         */
        QTextBlock firstVisibleBlock() const;

    private slots:

        void onTextChanged();

        void highlightCurrentLine();

    private:
        void paintParagraphNumberArea(QPaintEvent* event);

        void updateParagraphNumberAreaWidth();

        void updateParagraphNumberArea(QRect const& rect, int dy);

        std::unique_ptr<internal::ParagraphNumberArea> m_paragraphNumberArea;
        int m_lastVerticalSliderPos = 0;
        int m_lastBlockCount = 0;
        QColor const m_parNumberAreaColor = QColor(250, 250, 250);
        QColor const m_parNumberColor = QColor(130, 130, 130);
        QColor const m_curLineColor = QColor(255, 248, 217);

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
