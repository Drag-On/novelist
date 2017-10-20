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
#include <QtWidgets/QToolTip>
#include "widgets/texteditor/TextAnnotation.h"
#include "widgets/texteditor/TextEditor.h"

namespace novelist {
    TextEditor::TextEditor(QWidget* parent)
            :QTextEdit(parent),
             m_paragraphNumberArea{std::make_unique<internal::ParagraphNumberArea>(this)}
    {
        connect(this, &TextEditor::textChanged, this, &TextEditor::onTextChanged);
        connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateParagraphNumberAreaWidth);
        connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);
        connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::onCursorPositionChanged);
        connect(this, &TextEditor::selectionChanged, this, &TextEditor::onSelectionChanged);

        setDocument(new SceneDocument{this}); // Overwrite default QTextDocument
        setMouseTracking(true);

        updateParagraphNumberAreaWidth();
        highlightCurrentLine();

        m_onBoldActionConnection = Connection([this]() {
            return connect(&m_boldAction, &QAction::toggled, this, &TextEditor::onBoldActionToggled);
        });
        m_onItalicActionConnection = Connection([this]() {
           return connect(&m_italicAction, &QAction::toggled, this, &TextEditor::onItalicActionToggled);
        });
        m_onUnderlineActionConnection = Connection([this]() {
            return connect(&m_underlineAction, &QAction::toggled, this, &TextEditor::onUnderlineActionToggled);
        });
        m_onOverlineActionConnection = Connection([this]() {
            return connect(&m_overlineAction, &QAction::toggled, this, &TextEditor::onOverlineActionToggled);
        });
        m_onStrikethroughActionConnection = Connection([this]() {
            return connect(&m_strikethroughAction, &QAction::toggled, this, &TextEditor::onStrikethroughActionToggled);
        });
        m_onSmallCapsActionConnection = Connection([this]() {
            return connect(&m_smallCapsAction, &QAction::toggled, this, &TextEditor::onSmallCapsActionToggled);
        });
        m_boldAction.setCheckable(true);
        m_italicAction.setCheckable(true);
        m_underlineAction.setCheckable(true);
        m_overlineAction.setCheckable(true);
        m_strikethroughAction.setCheckable(true);
        m_smallCapsAction.setCheckable(true);

        connect(&m_addNoteAction, &QAction::triggered, this, &TextEditor::makeSelectionIntoNote);
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
        m_insights.clear();
        QTextEdit::setDocument(document);
        setDefaultBlockFormat();
    }

    void TextEditor::setDocument(QTextDocument* document)
    {
        auto* doc = dynamic_cast<SceneDocument*>(document);
        if (doc != nullptr) {
            setDocument(doc);
            setDefaultBlockFormat();
        }
        else
            throw std::runtime_error{"Tried to set document that isn't a SceneDocument."};
    }

    bool TextEditor::canUndo() const
    {
        if (document())
            return document()->isUndoAvailable();
        return false;
    }

    bool TextEditor::canRedo() const
    {
        if (document())
            return document()->isRedoAvailable();
        return false;
    }

    QAction* TextEditor::boldAction()
    {
        return &m_boldAction;
    }

    QAction* TextEditor::italicAction()
    {
        return &m_italicAction;
    }

    QAction* TextEditor::underlineAction()
    {
        return &m_underlineAction;
    }

    QAction* TextEditor::overlineAction()
    {
        return &m_overlineAction;
    }

    QAction* TextEditor::strikethroughAction()
    {
        return &m_strikethroughAction;
    }

    QAction* TextEditor::smallCapsAction()
    {
        return &m_smallCapsAction;
    }

    QAction* TextEditor::addNoteAction()
    {
        return &m_addNoteAction;
    }

    InsightModel* TextEditor::insights()
    {
        return &m_insights;
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


        // Show bounding boxes for debug purposes
        if constexpr(show_debug_info) {
            for (QTextBlock block = document()->begin(); block != document()->end(); block = block.next()) {

                if (block.isValid() && block.isVisible()) {
                    QPainter painter(viewport());
                    painter.setPen(QColor::fromRgb(255, 0, 0));
                    auto bb = document()->documentLayout()->blockBoundingRect(block);
                    bb.translate(
                            viewport()->geometry().x() - paragraphNumberAreaWidth() + block.blockFormat().leftMargin(),
                            viewport()->geometry().y() - verticalScrollBar()->value());
                    painter.drawRect(bb);

                    QFont const& font = block.begin() != block.end() ? block.begin().fragment().charFormat().font()
                                                                     : block.charFormat().font();
                    auto baseline = bb.top() + block.layout()->lineAt(0).ascent();

                    painter.setPen(QColor::fromRgb(0, 0, 0));
                    painter.setPen(Qt::DotLine);
                    painter.drawLine(0, baseline, 100, baseline);
                }
            }
        }
    }

    void TextEditor::focusInEvent(QFocusEvent* e)
    {
        emit focusReceived(true);

        QTextEdit::focusInEvent(e);
    }

    void TextEditor::focusOutEvent(QFocusEvent* e)
    {
        emit focusReceived(false);

        QTextEdit::focusOutEvent(e);
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

    void TextEditor::onCursorPositionChanged()
    {
        auto cursor = textCursor();
        ConnectionBlocker blockBoldAction(m_onBoldActionConnection);
        ConnectionBlocker blockItalicAction(m_onItalicActionConnection);
        ConnectionBlocker blockUnderlineAction(m_onUnderlineActionConnection);
        ConnectionBlocker blockOverlineAction(m_onOverlineActionConnection);
        ConnectionBlocker blockStrikethroughAction(m_onStrikethroughActionConnection);
        ConnectionBlocker blockSmallCapsAction(m_onSmallCapsActionConnection);
        m_boldAction.setChecked(cursor.charFormat().fontWeight() > QFont::Weight::Normal);
        m_italicAction.setChecked(cursor.charFormat().fontItalic());
        m_underlineAction.setChecked(cursor.charFormat().fontUnderline());
        m_overlineAction.setChecked(cursor.charFormat().fontOverline());
        m_strikethroughAction.setChecked(cursor.charFormat().fontStrikeOut());
        m_smallCapsAction.setChecked(cursor.charFormat().fontCapitalization() != QFont::Capitalization::MixedCase);
    }

    void TextEditor::onSelectionChanged()
    {
        m_addNoteAction.setEnabled(textCursor().hasSelection());
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

        while (block.isValid() && block.isVisible()) {
            auto bb = document()->documentLayout()->blockBoundingRect(block);
            bb.translate(viewport()->geometry().x() - paragraphNumberAreaWidth() + block.blockFormat().leftMargin(),
                    viewport()->geometry().y() - verticalScrollBar()->value());

            if (bb.top() <= event->rect().bottom() && bb.bottom() >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);

                QFont const& font = block.begin() != block.end() ? block.begin().fragment().charFormat().font()
                                                                 : block.charFormat().font();
                auto baseline = bb.top() + block.layout()->lineAt(0).ascent();
                auto y = baseline - fontMetrics().ascent() - 1;

                painter.setPen(m_parNumberColor);
                painter.drawText(0, y, m_paragraphNumberArea->width() - 2, fontMetrics().height(), Qt::AlignRight,
                        number);
            }

            block = block.next();
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

    void TextEditor::setDefaultBlockFormat()
    {
        setUndoRedoEnabled(false);
        QTextBlockFormat defaultBlockFormat;
        defaultBlockFormat.setAlignment(Qt::AlignLeft);
        defaultBlockFormat.setTextIndent(10);
        QTextCursor cursor = textCursor();
        cursor.setBlockFormat(defaultBlockFormat);
        setTextCursor(cursor);
        setUndoRedoEnabled(true);
        document()->setModified(false);
    }

    void TextEditor::onBoldActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        if (checked)
            curFormat.setFontWeight(QFont::Weight::Bold);
        else
            curFormat.setFontWeight(QFont::Weight::Normal);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::onItalicActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        curFormat.setFontItalic(checked);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::onUnderlineActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        curFormat.setFontUnderline(checked);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::onOverlineActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        curFormat.setFontOverline(checked);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::onStrikethroughActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        curFormat.setFontStrikeOut(checked);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::onSmallCapsActionToggled(bool checked)
    {
        if(!document())
            return;

        auto curFormat = textCursor().charFormat();
        if(checked)
            curFormat.setFontCapitalization(QFont::Capitalization::SmallCaps);
        else
            curFormat.setFontCapitalization(QFont::Capitalization::MixedCase);
        textCursor().setCharFormat(curFormat);
        setCurrentCharFormat(curFormat);
    }

    void TextEditor::makeSelectionIntoNote()
    {
        QTextCursor cursor = textCursor();
        if(!cursor.hasSelection())
            return;

        m_insights.insert(new TextAnnotation(document(), cursor.selectionStart(), cursor.selectionEnd(), ""));
    }

    void TextEditor::mouseMoveEvent(QMouseEvent* e)
    {
        m_textAnnotationMgr.onMousePosChanged(e->pos());
        QTextEdit::mouseMoveEvent(e);
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

        TextAnnotationManager::TextAnnotationManager(gsl::not_null<TextEditor*> editor) noexcept
                :m_editor(editor)
        {
        }

        void TextAnnotationManager::onMousePosChanged(QPoint pos)
        {
            auto const& insights = m_editor->m_insights;
            for (int row = 0; row < insights.rowCount(); ++row) {
                IInsight* insight = qvariant_cast<IInsight*>(
                        insights.data(insights.index(row, 0), static_cast<int>(InsightModelRoles::DataRole)));
                auto* annotation = dynamic_cast<TextAnnotation*>(insight);
                if (!annotation)
                    continue;

                auto cursor = annotation->toCursor();
                QTextCursor leftCursor(cursor.document());
                leftCursor.setPosition(cursor.selectionStart());
                QTextCursor rightCursor(cursor.document());
                rightCursor.setPosition(cursor.selectionEnd());
                QRect cursorRect = m_editor->cursorRect(leftCursor);
                QRect otherRect = m_editor->cursorRect(rightCursor);
                QRect selectionRect = cursorRect.united(otherRect);
                if (selectionRect.height() > cursorRect.height()) {
                    selectionRect.setLeft(0);
                    selectionRect.setRight(m_editor->size().width());
                }
                if (selectionRect.contains(pos))
                    QToolTip::showText(m_editor->mapToGlobal(pos), annotation->message(), m_editor, selectionRect);
            }
        }
    }
}