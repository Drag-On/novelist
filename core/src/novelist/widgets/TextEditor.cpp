/**********************************************************
 * @file   TextEditor.cpp
 * @author jan
 * @date   8/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QScrollBar>
#include <QPainter>
#include <QPaintEvent>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include "widgets/TextEditor.h"

namespace novelist {
    TextEditor::TextEditor(QWidget* parent)
            :QTextEdit(parent),
             m_paragraphNumberArea{std::make_unique<internal::ParagraphNumberArea>(this)}
    {
        connect(this, &TextEditor::textChanged, this, &TextEditor::onTextChanged);
        connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateParagraphNumberAreaWidth);
        connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

        setDocument(new SceneDocument{this}); // Overwrite default QTextDocument

        updateParagraphNumberAreaWidth();
        highlightCurrentLine();
    }

    TextEditor::~TextEditor() = default;

    int TextEditor::paragraphNumberAreaWidth() const
    {
        int digits = 1;
        int max = qMax(1, document()->blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }

        int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits + 10;

        return space;
    }

    SceneDocument* TextEditor::document() const
    {
        return dynamic_cast<SceneDocument*>(QTextEdit::document());
    }

    void TextEditor::setDocument(SceneDocument* document)
    {
        QTextEdit::setDocument(document);
    }

    void TextEditor::setDocument(QTextDocument* document)
    {
        auto* doc = dynamic_cast<SceneDocument*>(document);
        if (doc != nullptr)
            setDocument(doc);
        else
            throw std::runtime_error{"Tried to set document that isn't a SceneDocument."};
    }

    void TextEditor::resizeEvent(QResizeEvent* e)
    {
        QTextEdit::resizeEvent(e);

        QRect cr = contentsRect();
        m_paragraphNumberArea->setGeometry(QRect(cr.left(), cr.top(), paragraphNumberAreaWidth(), cr.height()));
    }

    void TextEditor::paintEvent(QPaintEvent* e)
    {
        QTextEdit::paintEvent(e);

        updateParagraphNumberArea(e->rect(), verticalScrollBar()->value() - m_lastVerticalSliderPos);
        m_lastVerticalSliderPos = verticalScrollBar()->value();
    }

    QTextBlock TextEditor::firstVisibleBlock() const
    {
        QTextCursor curs{document()};
        curs.movePosition(QTextCursor::Start);
        for (int i = 0; i < document()->blockCount(); ++i) {
            QTextBlock block = curs.block();

            QRect r1 = viewport()->geometry();
            QRect r2 = document()->documentLayout()->blockBoundingRect(block).translated(viewport()->geometry().x(),
                    viewport()->geometry().y() - (verticalScrollBar()->value())).toRect();

            if (r1.intersects(r2))
                return block;

            curs.movePosition(QTextCursor::NextBlock);
        }

        return QTextBlock{};
    }

    void TextEditor::onTextChanged()
    {
        // Emit block count changed signal if necessary
        if (document()->blockCount() != m_lastBlockCount) {
            emit blockCountChanged(document()->blockCount());
            m_lastBlockCount = document()->blockCount();
        }
    }

    void TextEditor::highlightCurrentLine()
    {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection{};

            selection.format.setBackground(m_curLineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }

    void TextEditor::paintParagraphNumberArea(QPaintEvent* event)
    {
        QPainter painter(m_paragraphNumberArea.get());
        painter.fillRect(event->rect(), m_parNumberAreaColor);
        QTextBlock block = firstVisibleBlock();
        int blockNumber = block.blockNumber();

        // Compute pixel offset from the widget top to the top of the block
        auto top = static_cast<int>(viewport()->geometry().top() - document()->documentMargin() + 3);
        top += static_cast<int>(document()->documentLayout()->blockBoundingRect(block).top()
                - verticalScrollBar()->value());

        // Compute pixel offset from the widget top to the bottom of the block
        int bottom = top + static_cast<int>(document()->documentLayout()->blockBoundingRect(block).height());

        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);
                painter.setPen(m_parNumberColor);
                painter.drawText(0, top, m_paragraphNumberArea->width() - 2, fontMetrics().height(), Qt::AlignRight,
                        number);
            }

            block = block.next();
            top = bottom;
            bottom = top + static_cast<int>(document()->documentLayout()->blockBoundingRect(block).height());
            ++blockNumber;
        }
    }

    void TextEditor::updateParagraphNumberAreaWidth()
    {
        setViewportMargins(paragraphNumberAreaWidth(), 0, 0, 0);
    }

    void TextEditor::updateParagraphNumberArea(QRect const& rect, int dy)
    {
        if (dy != 0)
            m_paragraphNumberArea->scroll(0, dy);
        else
            m_paragraphNumberArea->update(0, rect.y(), m_paragraphNumberArea->width(), rect.height());

        if (rect.contains(viewport()->rect()))
            updateParagraphNumberAreaWidth();
    }

    namespace internal {
        ParagraphNumberArea::ParagraphNumberArea(TextEditor* editor)
                :QWidget(editor),
                 m_textEditor{editor}
        {

        }

        QSize ParagraphNumberArea::sizeHint() const
        {
            return QSize(m_textEditor->paragraphNumberAreaWidth(), 0);
        }

        void ParagraphNumberArea::paintEvent(QPaintEvent* event)
        {
            QWidget::paintEvent(event);

            m_textEditor->paintParagraphNumberArea(event);
        }
    }
}