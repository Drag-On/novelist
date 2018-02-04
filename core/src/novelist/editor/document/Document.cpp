/**********************************************************
 * @file   Document.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QTextBlock>
#include <QScreen>
#include <QDebug>
#include <QtGui/QGuiApplication>
#include "editor/document/Document.h"
#include "editor/document/TextCursor.h"

namespace novelist::editor {
    Document::Document(gsl::not_null<TextFormatManager*> formatMgr, QString title,
            gsl::not_null<ProjectLanguage const*> lang)
            :m_properties(this, std::move(title), lang),
             m_formatMgr(formatMgr),
             m_doc(std::make_unique<QTextDocument>())
    {
        if (m_formatMgr->size() == 0)
            throw std::invalid_argument("TextFormatManager may not be empty.");

        updateIndentWidth();
        m_doc->setUndoRedoEnabled(false); // Disable internal undo, we do it ourselves.

        connect(m_doc.get(), &QTextDocument::blockCountChanged, this, &Document::onBlockCountChanged);
        connect(m_formatMgr, &TextFormatManager::formatModified, this, &Document::onFormatModified);
        connect(m_formatMgr, &TextFormatManager::formatReplaced, this, &Document::onFormatReplaced);

        // Per default, use the first format
        TextCursor cursor(this);
        cursor.setParagraphFormat(formatMgr->idFromIndex(0));
        cursor.setCharacterFormat(formatMgr->idFromIndex(0));
    }

    QUndoStack& Document::undoStack() noexcept
    {
        return m_undoStack;
    }

    Properties& Document::properties() noexcept
    {
        return m_properties;
    }

    Properties const& Document::properties() const noexcept
    {
        return m_properties;
    }

    QString Document::toRawText() const noexcept
    {
        return m_doc->toRawText();
    }

    bool Document::empty() const noexcept
    {
        return m_doc->isEmpty();
    }

    void Document::onParagraphFormatChanged(int blockIdx) noexcept
    {
        // Auto text indent logic
        if (blockIdx == 0 || blockIdx >= m_doc->blockCount())
            return;

        auto block = m_doc->findBlockByNumber(blockIdx);
        updateParagraphLayout(block);
    }

    void Document::onBlockCountChanged(int /*newBlockCount*/) noexcept
    {
        for (QTextBlock b = m_doc->firstBlock(); b.isValid(); b = b.next())
            updateParagraphLayout(b);
    }

    void Document::onFormatModified(TextFormatManager::WeakId id) noexcept
    {
        onFormatReplaced(id, id);
    }

    void Document::onFormatReplaced(TextFormatManager::WeakId id, TextFormatManager::WeakId replacementId) noexcept
    {
        updateIndentWidth();
        for (QTextBlock b = m_doc->firstBlock(); b.isValid(); b = b.next()) {
            auto formatId = m_formatMgr->getIdOfBlockFormat(b.blockFormat());

            if (formatId == id) {
                auto blockFormat = *m_formatMgr->getTextBlockFormat(replacementId);
                auto blockCharFormat = *m_formatMgr->getTextCharFormat(replacementId);
                QTextCursor cursor (m_doc.get());
                cursor.setPosition(b.position());
                cursor.setBlockFormat(blockFormat);
                cursor.setBlockCharFormat(blockCharFormat);
            }

            for (auto iter = b.begin(); iter != b.end(); ++iter) {
                auto charFormatId = m_formatMgr->getIdOfCharFormat(iter.fragment().charFormat());
                if (charFormatId == id) {
                    auto charFormat = *m_formatMgr->getTextCharFormat(replacementId);
                    QTextCursor cursor (m_doc.get());
                    cursor.setPosition(iter.fragment().position());
                    cursor.setPosition(iter.fragment().position() + iter.fragment().length(), QTextCursor::KeepAnchor);
                    cursor.setCharFormat(charFormat);
                }
            }

            updateParagraphLayout(b);
        }
    }

    void Document::updateParagraphLayout(QTextBlock block) noexcept
    {
        QTextBlock const& thisBlock = block;
        QTextBlock const& prevBlock = block.previous();

        if (!thisBlock.isValid()) {
            qWarning() << "Tried to update invalid paragraph";
            return;
        }

        auto thisFormatId = m_formatMgr->getIdOfBlockFormat(thisBlock.blockFormat());
        auto thisFormat = m_formatMgr->getTextFormat(m_formatMgr->indexFromId(thisFormatId));
        auto thisBlockFormat = *m_formatMgr->getTextBlockFormat(thisFormatId);
        auto thisBlockCharFormat = *m_formatMgr->getTextCharFormat(thisFormatId);

        if (prevBlock.isValid()) {
            auto prevFormatId = m_formatMgr->getIdOfBlockFormat(prevBlock.blockFormat());
            auto prevFormat = m_formatMgr->getTextFormat(m_formatMgr->indexFromId(prevFormatId));

            if (m_formatMgr->checkNeedAutoTextIndent(prevFormat, thisFormat))
                thisBlockFormat.setTextIndent(thisBlockFormat.indent() + m_doc->indentWidth());
        }

        QTextCursor cursor(m_doc.get());
        cursor.joinPreviousEditBlock();
        cursor.setPosition(thisBlock.position());
        cursor.setBlockFormat(thisBlockFormat);
        cursor.setBlockCharFormat(thisBlockCharFormat);
        cursor.endEditBlock();
    }

    void Document::updateIndentWidth() noexcept
    {
        // The first line indent should be between the font's point size and 4 times that value
        qreal const pointWidth = m_formatMgr->getFont().pointSize() * 1.5;
        qreal const dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
        m_doc->setIndentWidth(pointWidth / 72 * dpi);
    }

    namespace internal {
        TextInsertCommand::TextInsertCommand(Document* doc, int pos, QString added) noexcept
                :m_doc(doc),
                 m_pos(pos),
                 m_added(std::move(added))
        {
            setText(Document::tr("text insertion in %1").arg(doc->properties().title()));
        }

        void TextInsertCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos + m_added.size());
            cursor.setPosition(m_pos, QTextCursor::KeepAnchor);

//            qDebug() << "Undo inserting" << cursor.selectedText() << "at" << m_pos << "(" << m_added.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            infoText.insert(m_pos + m_added.size(), "‸");
//            qDebug() << "    " << infoText;

            cursor.removeSelectedText();
        }

        void TextInsertCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);

//            qDebug() << "Redo inserting" << m_added << "at" << m_pos << "(" << m_added.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            qDebug() << "    " << infoText;

            cursor.insertText(m_added);
        }

        int TextInsertCommand::id() const
        {
            return 0;
        }

        bool TextInsertCommand::mergeWith(const QUndoCommand* other)
        {
            auto cmd = dynamic_cast<TextInsertCommand const*>(other);
            if (!cmd)
                return false;
            if (m_pos + m_added.size() == cmd->m_pos && !isCompleteSentence(m_added, &m_doc->m_properties.language())) {
                m_added += cmd->m_added;
                return true;
            }
            return false;
        }

        int TextInsertCommand::undoPosition() const noexcept
        {
            return m_pos;
        }

        int TextInsertCommand::redoPosition() const noexcept
        {
            return m_pos + m_added.size();
        }

        TextRemoveCommand::TextRemoveCommand(Document* doc, int pos, QString removed) noexcept
        : m_doc(doc),
          m_pos(pos),
          m_removed(std::move(removed))
        {
            setText(Document::tr("text removal in %1").arg(doc->properties().title()));
        }

        void TextRemoveCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);

//            qDebug() << "Undo removing" << m_removed << "at" << m_pos << "(" << m_removed.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            qDebug() << "    " << infoText;

            cursor.insertText(m_removed);
        }

        void TextRemoveCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos + m_removed.size());
            cursor.setPosition(m_pos, QTextCursor::KeepAnchor);

//            qDebug() << "Redo removing" << m_removed << "at" << m_pos << "(" << m_removed.size() << ")";
//            QString infoText = cursor.block().text();
//            infoText.insert(m_pos, "‸");
//            infoText.insert(m_pos + m_removed.size(), "‸");
//            qDebug() << "    " << infoText;

            cursor.removeSelectedText();
        }

        int TextRemoveCommand::id() const
        {
            return 1;
        }

        bool TextRemoveCommand::mergeWith(QUndoCommand const* other)
        {
            auto cmd = dynamic_cast<TextRemoveCommand const*>(other);
            if (!cmd)
                return false;
            if (cmd->m_pos == m_pos && !isCompleteSentence(m_removed, &m_doc->m_properties.language())) {
                m_removed += cmd->m_removed;
                return true;
            }
            if (cmd->m_pos + cmd->m_removed.size() == m_pos && !isCompleteSentence(cmd->m_removed, &m_doc->m_properties.language())) {
                m_pos = cmd->m_pos;
                m_removed = cmd->m_removed + m_removed;
                return true;
            }
            return false;
        }

        int TextRemoveCommand::undoPosition() const noexcept
        {
            return m_pos + m_removed.size();
        }

        int TextRemoveCommand::redoPosition() const noexcept
        {
            return m_pos;
        }

        BlockInsertCommand::BlockInsertCommand(Document* doc, int pos) noexcept
                :m_doc(doc),
                 m_pos(pos)
        {
            setText(Document::tr("new paragraph in %1").arg(doc->properties().title()));
        }

        void BlockInsertCommand::undo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            cursor.deleteChar();
        }

        void BlockInsertCommand::redo()
        {
            QTextCursor cursor(m_doc->m_doc.get());
            cursor.setPosition(m_pos);
            auto prevCharFormat = cursor.charFormat();
            cursor.insertBlock();
            cursor.setBlockCharFormat(prevCharFormat);
        }

        int BlockInsertCommand::undoPosition() const noexcept
        {
            return m_pos;
        }

        int BlockInsertCommand::redoPosition() const noexcept
        {
            return m_pos + 1;
        }
    }
}