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
#include <gsl/gsl>

namespace novelist {
    class TextEditor;

    /**
     * Manages insights of a TextEditor, such as displaying their tool tips on hover
     */
    class TextEditorInsightManager : public QObject {
    Q_OBJECT

    public:
        /**
         * Construct new manager
         * @param editor Non-owning pointer to the editor to manage. Pointer must stay valid during object lifetime.
         */
        explicit TextEditorInsightManager(gsl::not_null<TextEditor*> editor) noexcept;

    public slots:

        /**
         * Call this whenever mouse position was changed
         * @param pos New mouse position relative to editor viewport
         */
        void onMousePosChanged(QPoint pos);

    private:
        gsl::not_null<TextEditor*> m_editor;
    };
}

#endif //NOVELIST_TEXTEDITORINSIGHTMANAGER_H
