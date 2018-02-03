/**********************************************************
 * @file   Document.cpp
 * @author jan
 * @date   1/25/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtGui/QTextBlock>
#include <QDebug>
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

        m_doc->setIndentWidth(8);

        connect(m_doc.get(), &QTextDocument::undoCommandAdded, this, &Document::onUndoCommandAdded);
        connect(m_doc.get(), &QTextDocument::blockCountChanged, this, &Document::onBlockCountChanged);

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

    void Document::onUndoCommandAdded() noexcept
    {
        m_undoStack.push(new internal::UndoCommand(this));
    }

    void Document::updateParagraphLayout(QTextBlock block) noexcept
    {
        QTextBlock thisBlock = block;
        QTextBlock prevBlock = block.previous();

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

    namespace internal {
        UndoCommand::UndoCommand(Document* doc) noexcept
        : m_doc(doc)
        {
            setText(Document::tr("modification of %1").arg(doc->properties().title()));
        }

        void UndoCommand::undo()
        {
            m_doc->m_doc->undo();
        }

        void UndoCommand::redo()
        {
            m_doc->m_doc->redo();
        }
    }
}