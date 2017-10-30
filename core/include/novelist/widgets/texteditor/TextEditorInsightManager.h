/**********************************************************
 * @file   InsightManager.h
 * @author jan
 * @date   10/23/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORINSIGHTMANAGER_H
#define NOVELIST_TEXTEDITORINSIGHTMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtCore/QFuture>
#include <gsl/gsl>
#include "document/InsightFactory.h"
#include "util/ConnectionWrapper.h"

namespace novelist {
    class TextEditor;

    /**
     * A single result as returned from an auto inspection tool
     */
    struct InsightResult {
        std::shared_ptr<InsightFactory> m_factory; //!< Factory object to create the actual insight
        int m_left; //!< Left start of the insight, relative to the block
        int m_right; //!< Right end of the insight, relative to the block
    };

    /**
     * All insight results from a single block
     */
    using InsightBlockResult = std::vector<InsightResult>;

    /**
     * Insight results for all requested blocks
     */
    using AutoInsightResults = std::vector<InsightBlockResult>;

    /**
     * Manages insights of a TextEditor, such as displaying their tool tips on hover and running auto-inspections
     */
    class TextEditorInsightManager : public QObject {
    Q_OBJECT

    public:
        /**
         * Construct new manager
         * @param editor Non-owning pointer to the editor to manage. Pointer must stay valid during object lifetime.
         */
        explicit TextEditorInsightManager(gsl::not_null<TextEditor*> editor) noexcept;

        /**
         * Runs inspection on the whole underlying document
         */
        void reinspect() noexcept;

    public slots:

        /**
         * Call this whenever mouse position was changed
         * @param pos New mouse position relative to editor viewport
         */
        void onMousePosChanged(QPoint pos);

        /**
         * Call this whenever the editor's document changed
         */
        void onDocumentChanged();

    private:
        gsl::not_null<TextEditor*> m_editor;
        ConnectionWrapper m_contentsChangeConnection;
        QFuture<AutoInsightResults> m_updateResults;
        std::vector<QTextBlock> m_needUpdateBlocks;
        std::vector<QTextBlock> m_updatingBlocks;
        QTimer m_updateTimer;

        void startAutoInsightRefresh();
        void finishAutoInsightRefresh();
        static AutoInsightResults runAutoInsightRefresh(std::vector<QString> blocks);

    private slots:

        void onContentsChange(int pos, int removed, int added);

        void onUpdate();
    };
}

#endif //NOVELIST_TEXTEDITORINSIGHTMANAGER_H