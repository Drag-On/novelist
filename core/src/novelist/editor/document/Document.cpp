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
        QTextCursor cursor(m_doc.get());
        auto const id = formatMgr->idFromIndex(0);
        cursor.setBlockFormat(*formatMgr->getTextBlockFormat(id));
        cursor.setBlockCharFormat(*formatMgr->getTextCharFormat(id));
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

    ParagraphIterator Document::begin() const noexcept
    {
        return ParagraphIterator(this, 0);
    }

    ParagraphIterator Document::end() const noexcept
    {
        return ParagraphIterator(this, -1);
    }

    TextFormatManager* Document::formatManager() noexcept
    {
        return m_formatMgr;
    }

    TextFormatManager const* Document::formatManager() const noexcept
    {
        return m_formatMgr;
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
}