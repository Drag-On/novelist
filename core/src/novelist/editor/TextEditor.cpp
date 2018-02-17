/**********************************************************
 * @file   TextEditor.cpp
 * @author jan
 * @date   2/3/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "editor/TextEditor.h"
#include "util/DelegateAction.h"
#include <QtGui/QGuiApplication>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>
#include <QClipboard>
#include <QMimeData>
#include <QPainter>
#include "editor/TextEditorParagraphNumbersSideBar.h"
#include "util/Overloaded.h"

namespace novelist::editor {
    TextEditor::TextEditor(QWidget* parent) noexcept
            :QWidget(parent)
    {
        // Signify that this widget handles composite keys (like ô composed from ^ and o)
        setAttribute(Qt::WA_InputMethodEnabled, true);
        m_vBoxLayout = new QVBoxLayout();
        m_hBoxLayout = new QHBoxLayout();
        m_vBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_vBoxLayout->setSpacing(0);
        m_hBoxLayout->setContentsMargins(0, 0, 0, 0);
        m_hBoxLayout->setSpacing(0);
        m_textEdit = new internal::TextEdit(this);
        m_textEdit->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
        m_textEdit->setAttribute(Qt::WA_InputMethodEnabled, false); // Text edit should not bypass our own handling
        m_textEdit->installEventFilter(this);
        m_textEdit->setEnabled(false);
        m_hBoxLayout->addWidget(m_textEdit);
        m_vBoxLayout->addLayout(m_hBoxLayout);
        setLayout(m_vBoxLayout);
        updateActions();

        connect(m_textEdit, &QTextEdit::cursorPositionChanged, this, &TextEditor::onCursorPositionChanged);
        connect(m_textEdit, &QTextEdit::selectionChanged, this, &TextEditor::onSelectionChanged);
        connect(m_textEdit, &QTextEdit::textChanged, this, &TextEditor::onTextChanged);
        connect(this, &TextEditor::blockCountChanged, this, &TextEditor::onBlockCountChanged);
        connect(m_textEdit->horizontalScrollBar(), &QAbstractSlider::valueChanged, this, &TextEditor::onHorizontalScroll);
        connect(m_textEdit->verticalScrollBar(), &QAbstractSlider::valueChanged, this, &TextEditor::onVerticalScroll);

        // TODO: Remove this test
        m_leftSideBars.emplace_back(new TestSideBar(this, QColor::fromRgb(255, 0, 0)));
        m_leftSideBars.emplace_back(new TextEditorParagraphNumbersSideBar(this));
        m_rightSideBars.emplace_back(new TextEditorParagraphNumbersSideBar(this));
    }

    void TextEditor::setDocument(std::unique_ptr<Document> doc) noexcept
    {
        m_doc = std::move(doc);
        if (m_doc) {
            m_textEdit->setDocument(m_doc->m_doc.get());
            m_textEdit->setEnabled(true);
        }
        else {
            m_textEdit->setDocument(nullptr);
            m_textEdit->setEnabled(false);
        }
        updateActions();
    }

    Document* TextEditor::getDocument() noexcept
    {
        return m_doc.get();
    }

    Document const* TextEditor::getDocument() const noexcept
    {
        return m_doc.get();
    }

    TextCursor TextEditor::getCursor() const
    {
        return TextCursor(m_doc.get(), m_textEdit->textCursor().position(), m_textEdit->textCursor().anchor());
    }

    void TextEditor::setCursor(TextCursor cursor)
    {
        m_textEdit->setTextCursor(cursor.m_cursor);
    }

    EditorActions const& TextEditor::editorActions() const noexcept
    {
        return m_actions;
    }

    bool TextEditor::isParagraphVisible(TextParagraph const& par) const noexcept
    {
        QTextBlock const block = par.m_block;
        QRect r1 = m_textEdit->viewport()->geometry();
        QRect r2 = m_textEdit->document()->documentLayout()->blockBoundingRect(block).translated(
                m_textEdit->viewport()->geometry().x(),
                m_textEdit->viewport()->geometry().y() - (m_textEdit->verticalScrollBar()->value())
        ).toRect();

        return r1.intersects(r2);
    }

    std::pair<int, int> TextEditor::scrollBarValues() const noexcept
    {
        return {m_textEdit->horizontalScrollBar()->value(), m_textEdit->verticalScrollBar()->value()};
    }

    QWidget* TextEditor::viewport() const noexcept
    {
        return m_textEdit->viewport();
    }

    void TextEditor::keyPressEvent(QKeyEvent* event)
    {
        QString modifier;
        QString key;
        if (event->modifiers() & Qt::ShiftModifier)
            modifier += "Shift+";
        if (event->modifiers() & Qt::ControlModifier)
            modifier += "Ctrl+";
        if (event->modifiers() & Qt::AltModifier)
            modifier += "Alt+";
        if (event->modifiers() & Qt::MetaModifier)
            modifier += "Meta+";
        key = QKeySequence(event->key()).toString();
        QKeySequence keySequence(modifier + key);

        // Ignore any key sequence that should be mapped to an action
        for (auto action : actions()) {
            if (action->shortcut() == keySequence) {
                event->ignore();
                return;
            }
        }

        if (event->key() == Qt::Key_Backspace)
            getCursor().deletePrevious();
        else if (event->matches(QKeySequence::StandardKey::DeleteEndOfLine)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::EndOfLine);
            cursor.deleteSelected();
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::DeleteEndOfWord)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::EndOfWord);
            cursor.deleteSelected();
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::DeleteStartOfWord)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::StartOfWord);
            cursor.deleteSelected();
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::DeleteCompleteLine)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::StartOfLine);
            cursor.select(TextCursor::MoveOperation::EndOfLine);
            cursor.deleteSelected();
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::InsertLineSeparator)) {
            getCursor().breakLine();
            m_textEdit->ensureCursorVisible();
        }
        else if (event->matches(QKeySequence::StandardKey::InsertParagraphSeparator)) {
            getCursor().breakParagraph();
            m_textEdit->ensureCursorVisible();
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToEndOfBlock)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::EndOfParagraph);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToEndOfDocument)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::End);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToEndOfLine)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::EndOfLine);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToNextChar)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::Right);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToNextLine)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::Down);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToNextPage)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::MoveToNextWord)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::StartOfNextWord);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToPreviousChar)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::Left);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToPreviousLine)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::Up);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToPreviousPage)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::MoveToPreviousWord)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::StartOfPreviousWord);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToStartOfBlock)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::StartOfParagraph);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToStartOfDocument)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::Start);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::MoveToStartOfLine)) {
            auto cursor = getCursor();
            cursor.move(TextCursor::MoveOperation::StartOfLine);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::Print)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::Refresh)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::Deselect)) {
            auto cursor = getCursor();
            cursor.setPosition(cursor.position());
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectEndOfBlock)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::EndOfParagraph);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectEndOfDocument)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::End);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectEndOfLine)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::EndOfLine);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectNextChar)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::Right);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectNextLine)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::Down);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectNextPage)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::SelectNextWord)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::StartOfNextWord);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectPreviousChar)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::Left);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectPreviousLine)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::Up);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectPreviousPage)) { } // TODO
        else if (event->matches(QKeySequence::StandardKey::SelectPreviousWord)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::StartOfPreviousWord);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectStartOfBlock)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::StartOfParagraph);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectStartOfDocument)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::Start);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::SelectStartOfLine)) {
            auto cursor = getCursor();
            cursor.select(TextCursor::MoveOperation::StartOfLine);
            setCursor(cursor);
        }
        else if (event->matches(QKeySequence::StandardKey::ZoomIn)) {
            QFont font = m_doc->m_formatMgr->getFont();
            if (font.pointSize() < 48) {
                font.setPointSize(font.pointSize() + 1);
                m_doc->m_formatMgr->setFont(font);
            }
        }
        else if (event->matches(QKeySequence::StandardKey::ZoomOut)) {
            QFont font = m_doc->m_formatMgr->getFont();
            if (font.pointSize() > 5) {
                font.setPointSize(font.pointSize() - 1);
                m_doc->m_formatMgr->setFont(font);
            }
        }
        else if (event->matches(QKeySequence::StandardKey::FullScreen)) { } // TODO
        else if (event->text().isEmpty())
            checkDeadKeyInput(static_cast<Qt::Key>(event->key()));
        else if (!event->text().isEmpty()) {
            getCursor().insertText(composeInputKey(event->text()));
            m_textEdit->ensureCursorVisible();
        }
        QWidget::keyPressEvent(event);
    }

    void TextEditor::keyReleaseEvent(QKeyEvent* event)
    {
        QWidget::keyReleaseEvent(event);
    }

    bool TextEditor::eventFilter(QObject* obj, QEvent* event)
    {
        if (obj == m_textEdit) {
            if (!m_doc)
                return true;

            // Override any internal shortcuts the QTextEdit might use, as we do our own keyboard handling
            if (event->type() == QEvent::ShortcutOverride) {
                auto keyEvent = static_cast<QKeyEvent*>(event);
                keyEvent->ignore();
                return true;
            }
        }
        return false;
    }

    void TextEditor::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);

        updateSideBars(ResizeUpdate{});
    }

    void TextEditor::tryMoveCursorToUndoPos() noexcept
    {
        int pos = -1;
        auto cmd = m_doc->undoStack().command(m_doc->undoStack().index());
        if (auto ptr = dynamic_cast<internal::TextInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        else if (auto ptr = dynamic_cast<internal::TextRemoveCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        else if (auto ptr = dynamic_cast<internal::BlockInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->undoPosition();
        if (pos >= 0) {
            auto cursor = getCursor();
            cursor.setPosition(pos);
            setCursor(cursor);
            m_textEdit->ensureCursorVisible();
        }
    }

    void TextEditor::tryMoveCursorToRedoPos() noexcept
    {
        int pos = -1;
        auto cmd = m_doc->undoStack().command(m_doc->undoStack().index() - 1);
        if (auto ptr = dynamic_cast<internal::TextInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        else if (auto ptr = dynamic_cast<internal::TextRemoveCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        else if (auto ptr = dynamic_cast<internal::BlockInsertCommand const*>(cmd); ptr != nullptr)
            pos = ptr->redoPosition();
        if (pos >= 0) {
            auto cursor = getCursor();
            cursor.setPosition(pos);
            setCursor(cursor);
            m_textEdit->ensureCursorVisible();
        }
    }

    bool TextEditor::checkDeadKeyInput(Qt::Key key) noexcept
    {
        // This is a pretty dirty workaround to allow input of combining unicode characters like ^. This should be
        // removed once qt properly supports it.
        //    https://bugreports.qt.io/browse/QTBUG-42181
        //    https://bugreports.qt.io/browse/QTBUG-56452
        m_inputModifier = "";
        switch (key) {
            case Qt::Key::Key_Dead_Abovedot:
                m_inputModifier = QString::fromUtf8("\u0307");
                break;
            case Qt::Key::Key_Dead_Abovering:
                m_inputModifier = QString::fromUtf8("\u030A");
                break;
            case Qt::Key::Key_Dead_Acute:
                m_inputModifier = QString::fromUtf8("\u0301");
                break;
            case Qt::Key::Key_Dead_Belowdot:
                m_inputModifier = QString::fromUtf8("\u0323");
                break;
            case Qt::Key::Key_Dead_Breve:
                m_inputModifier = QString::fromUtf8("\u0306");
                break;
            case Qt::Key::Key_Dead_Caron:
                m_inputModifier = QString::fromUtf8("\u030C");
                break;
            case Qt::Key::Key_Dead_Cedilla:
                m_inputModifier = QString::fromUtf8("\u0327");
                break;
            case Qt::Key::Key_Dead_Circumflex:
                m_inputModifier = QString::fromUtf8("\u0302");
                break;
            case Qt::Key::Key_Dead_Diaeresis:
                m_inputModifier = QString::fromUtf8("\u0308");
                break;
            case Qt::Key::Key_Dead_Doubleacute:
                m_inputModifier = QString::fromUtf8("\u030B");
                break;
            case Qt::Key::Key_Dead_Grave:
                m_inputModifier = QString::fromUtf8("\u0300");
                break;
            case Qt::Key::Key_Dead_Hook:
                m_inputModifier = QString::fromUtf8("\u0309");
                break;
            case Qt::Key::Key_Dead_Horn:
                m_inputModifier = QString::fromUtf8("\u031B");
                break;
            case Qt::Key::Key_Dead_Macron:
                m_inputModifier = QString::fromUtf8("\u0304");
                break;
            case Qt::Key::Key_Dead_Ogonek:
                m_inputModifier = QString::fromUtf8("\u0328");
                break;
            case Qt::Key::Key_Dead_Tilde:
                m_inputModifier = QString::fromUtf8("\u0303");
                break;
            case Qt::Key::Key_Dead_Iota:
            case Qt::Key::Key_Dead_Semivoiced_Sound:
            case Qt::Key::Key_Dead_Voiced_Sound:
                qDebug() << "Unhandled dead key: " << key;
                break;
            default:
                break;
        }
        return !m_inputModifier.isEmpty();
    }

    QString TextEditor::composeInputKey(QString input) noexcept
    {
        input += m_inputModifier;
        m_inputModifier = "";
        return input;
    }

    void TextEditor::updateActions() noexcept
    {
        if (m_actions.m_undoAction == nullptr) {
            m_actions.m_undoAction = new DelegateAction(QIcon::fromTheme("edit-undo"), tr("Undo"), this);
            m_actions.m_undoAction->setShortcut(QKeySequence::StandardKey::Undo);
            addAction(m_actions.m_undoAction);
            connect(m_actions.m_undoAction, &QAction::triggered, this, &TextEditor::onUndo);
        }
        if (m_actions.m_redoAction == nullptr) {
            m_actions.m_redoAction = new DelegateAction(QIcon::fromTheme("edit-redo"), tr("Redo"), this);
            m_actions.m_redoAction->setShortcut(QKeySequence::StandardKey::Redo);
            addAction(m_actions.m_redoAction);
            connect(m_actions.m_redoAction, &QAction::triggered, this, &TextEditor::onRedo);
        }
        if (m_actions.m_copyAction == nullptr) {
            m_actions.m_copyAction = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy"), this);
            m_actions.m_copyAction->setShortcut(QKeySequence::StandardKey::Copy);
            addAction(m_actions.m_copyAction);
            connect(m_actions.m_copyAction, &QAction::triggered, this, &TextEditor::onCopy);
        }
        if (m_actions.m_cutAction == nullptr) {
            m_actions.m_cutAction = new QAction(QIcon::fromTheme("edit-cut"), tr("Cut"), this);
            m_actions.m_cutAction->setShortcut(QKeySequence::StandardKey::Cut);
            addAction(m_actions.m_cutAction);
            connect(m_actions.m_cutAction, &QAction::triggered, this, &TextEditor::onCut);
        }
        if (m_actions.m_pasteAction == nullptr) {
            m_actions.m_pasteAction = new QAction(QIcon::fromTheme("edit-paste"), tr("Paste"), this);
            m_actions.m_pasteAction->setShortcut(QKeySequence::StandardKey::Paste);
            addAction(m_actions.m_pasteAction);
            connect(m_actions.m_pasteAction, &QAction::triggered, this, &TextEditor::onPaste);
        }
        if (m_actions.m_deleteAction == nullptr) {
            m_actions.m_deleteAction = new QAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this);
            m_actions.m_deleteAction->setShortcut(QKeySequence::StandardKey::Delete);
            addAction(m_actions.m_deleteAction);
            connect(m_actions.m_deleteAction, &QAction::triggered, this, &TextEditor::onDelete);
        }
        if (m_actions.m_selectAllAction == nullptr) {
            m_actions.m_selectAllAction = new QAction(QIcon::fromTheme("edit-select-all"), tr("Select all"), this);
            m_actions.m_selectAllAction->setShortcut(QKeySequence::StandardKey::SelectAll);
            addAction(m_actions.m_selectAllAction);
            connect(m_actions.m_selectAllAction, &QAction::triggered, this, &TextEditor::onSelectAll);
        }
        if (m_doc) {
            dynamic_cast<DelegateAction*>(m_actions.m_undoAction)->setDelegate(
                    m_doc->undoStack().createUndoAction(this));
            m_actions.m_undoAction->setShortcut(QKeySequence::StandardKey::Undo);
            dynamic_cast<DelegateAction*>(m_actions.m_redoAction)->setDelegate(
                    m_doc->undoStack().createRedoAction(this));
            m_actions.m_redoAction->setShortcut(QKeySequence::StandardKey::Redo);
            m_actions.m_copyAction->setEnabled(getCursor().hasSelection());
            m_actions.m_cutAction->setEnabled(getCursor().hasSelection());
            m_actions.m_pasteAction->setEnabled(QGuiApplication::clipboard()->mimeData()->hasText());
            m_actions.m_deleteAction->setEnabled(true);
            m_actions.m_selectAllAction->setEnabled(true);
        }
        else {
            dynamic_cast<DelegateAction*>(m_actions.m_undoAction)->setDelegate(nullptr);
            dynamic_cast<DelegateAction*>(m_actions.m_redoAction)->setDelegate(nullptr);
            m_actions.m_copyAction->setEnabled(false);
            m_actions.m_cutAction->setEnabled(false);
            m_actions.m_pasteAction->setEnabled(false);
            m_actions.m_deleteAction->setEnabled(false);
            m_actions.m_selectAllAction->setEnabled(false);
        }
    }

    void TextEditor::updateSideBars(TextEditor::SideBarUpdate const& update) noexcept
    {
        QRect cr = contentsRect();
        if (m_textEdit->horizontalScrollBar()->isVisible())
            cr.setHeight(cr.height() - m_textEdit->horizontalScrollBar()->height());
        if (m_textEdit->verticalScrollBar()->isVisible())
            cr.setWidth(cr.width() - m_textEdit->verticalScrollBar()->width());
        int leftMargin = 0;
        int x = cr.left();
        for (auto const& w : m_leftSideBars) {
            updateSideBarIfRequired(*w, update);
            auto const width = w->sideBarWidth();
            w->setGeometry(QRect(x, cr.top(), width, cr.height()));
            x += width;
            leftMargin += width;
        }

        int rightMargin = 0;
        x = cr.right();
        for (auto const& w : m_rightSideBars) {
            updateSideBarIfRequired(*w, update);
            auto const width = w->sideBarWidth();
            w->setGeometry(QRect(x - width, cr.top(), width, cr.height()));
            x -= width;
            rightMargin += width;
        }

        m_textEdit->setViewportMargins(leftMargin, 0, rightMargin, 0); // TODO: Other margins
    }

    void TextEditor::updateSideBarIfRequired(TextEditorSideBar& sideBar, SideBarUpdate const& update) noexcept
    {
        std::visit(Overloaded {
                [&sideBar](ResizeUpdate const& /*u*/) {
                    if (sideBar.updateTriggers().test(UpdateTrigger::Resize))
                        sideBar.update();
                },
                [&sideBar](ParagraphCountChangeUpdate const& /*u*/) {
                    if (sideBar.updateTriggers().test(UpdateTrigger::ParagraphCountChange))
                        sideBar.update();
                },
                [&sideBar](TextChangeUpdate const& /*u*/) {
                    if (sideBar.updateTriggers().test(UpdateTrigger::TextChange))
                        sideBar.update();
                },
                [&sideBar](VerticalScrollUpdate const& u) {
                    if (sideBar.updateTriggers().test(UpdateTrigger::VerticalScroll))
                        sideBar.scroll(0, u.m_delta);
                },
                [&sideBar](HorizontalScrollUpdate const& u) {
                    if (sideBar.updateTriggers().test(UpdateTrigger::HorizontalScroll))
                        sideBar.scroll(u.m_delta, 0);
                },
        }, update);
    }

    void TextEditor::onUndo() noexcept
    {
        // Note: The actual undo operation is called by the action separately as constructed from undo stack
        tryMoveCursorToUndoPos();
    }

    void TextEditor::onRedo() noexcept
    {
        // Note: The actual redo operation is called by the action separately as constructed from undo stack
        tryMoveCursorToRedoPos();
    }

    void TextEditor::onCopy() const noexcept
    {
        auto cursor = getCursor();
        if (cursor.hasSelection()) {
            auto clipboard = QGuiApplication::clipboard();
            clipboard->setText(cursor.selectedText());
            // TODO: Copy formatted text
        }
    }

    void TextEditor::onCut() noexcept
    {
        auto cursor = getCursor();
        if (cursor.hasSelection()) {
            auto clipboard = QGuiApplication::clipboard();
            clipboard->setText(cursor.selectedText());
            // TODO: Copy formatted text
            cursor.deleteSelected();
        }
    }

    void TextEditor::onPaste() noexcept
    {
        auto clipboard = QGuiApplication::clipboard();
        auto cursor = getCursor();
        cursor.insertText(clipboard->text());
        // TODO: Copy formatted text
    }

    void TextEditor::onDelete() noexcept
    {
        getCursor().deleteNext();
    }

    void TextEditor::onSelectAll() noexcept
    {
        auto cursor = getCursor();
        cursor.move(TextCursor::MoveOperation::Start);
        cursor.select(TextCursor::MoveOperation::End);
        setCursor(cursor);
    }

    void TextEditor::onCursorPositionChanged() noexcept
    {
        updateActions();
    }

    void TextEditor::onSelectionChanged() noexcept
    {
        updateActions();
    }

    void TextEditor::onTextChanged() noexcept
    {
        updateActions();

        // Emit block count changed signal if necessary
        int const blockCount = m_doc->m_doc->blockCount();
        if (blockCount != m_lastBlockCount) {
            emit blockCountChanged(blockCount);
            m_lastBlockCount = blockCount;
        }

        updateSideBars(TextChangeUpdate{});
    }

    void TextEditor::onBlockCountChanged(int /*blockCount*/) noexcept
    {
        updateSideBars(ParagraphCountChangeUpdate{});
    }

    void TextEditor::onHorizontalScroll(int value) noexcept
    {
        updateSideBars(HorizontalScrollUpdate{value - m_lastHorizontalSliderPos});
        m_lastHorizontalSliderPos = value;
    }

    void TextEditor::onVerticalScroll(int value) noexcept
    {
        updateSideBars(VerticalScrollUpdate{value - m_lastVerticalSliderPos});
        m_lastVerticalSliderPos = value;
    }

    QRect TextEditor::contentArea() const noexcept
    {
        int const xShift = m_textEdit->contentsMargins().left();
        int const yShift = m_textEdit->contentsMargins().top();
        int const widthShift = -m_textEdit->contentsMargins().right() - xShift;
        int const heightShift = -m_textEdit->contentsMargins().bottom() - yShift;

        QRect area = m_textEdit->geometry();
        area.setX(area.x() + xShift);
        area.setY(area.y() + yShift);
        area.setWidth(area.width() + widthShift);
        area.setHeight(area.height() + heightShift);

        return area;
    }

    QVariant TextEditor::inputMethodQuery(Qt::InputMethodQuery query) const
    {
        return m_textEdit->inputMethodQuery(query);
    }

    void TextEditor::inputMethodEvent(QInputMethodEvent* event)
    {
        if (!event->commitString().isEmpty()) {
            QKeyEvent keyEvent(QEvent::KeyPress, 0, Qt::NoModifier, event->commitString());
            keyPressEvent(&keyEvent);
        }
        event->accept();
    }

    namespace internal {
        void TextEdit::keyPressEvent(QKeyEvent* e)
        {
            e->ignore();
        }

        void TextEdit::keyReleaseEvent(QKeyEvent* e)
        {
            e->ignore();
        }

        bool TextEdit::canInsertFromMimeData(const QMimeData* /*source*/) const
        {
            // Copy & paste is handled within the wrapping TextEditor class
            return false;
        }

        void TextEdit::insertFromMimeData(const QMimeData* /*source*/)
        {
            // Copy & paste is handled within the wrapping TextEditor class
        }

        void TextEdit::paintEvent(QPaintEvent* e)
        {
            QTextEdit::paintEvent(e);

            // Show maximum line width if set
            if (lineWrapColumnOrWidth() > 0 && lineWrapMode() == LineWrapMode::FixedPixelWidth) {
                QPainter painter(viewport());
                painter.setPen(QColor::fromRgb(220, 220, 220));
                painter.drawLine(lineWrapColumnOrWidth(), 0, lineWrapColumnOrWidth(), viewport()->height());
            }

            // Show bounding boxes for debug purposes
            if constexpr (s_showDebugInfo) {
                for (QTextBlock block = document()->begin(); block != document()->end(); block = block.next()) {

                    if (block.isValid() && block.isVisible()) {
                        QPainter painter(viewport());
                        painter.setPen(QColor::fromRgb(255, 0, 0));
                        auto bb = document()->documentLayout()->blockBoundingRect(block);
                        bb.translate(viewport()->geometry().x() - viewportMargins().left()
                                        + block.blockFormat().leftMargin(),
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
    }
}