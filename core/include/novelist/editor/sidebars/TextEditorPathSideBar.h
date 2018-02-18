/**********************************************************
 * @file   TextEditorPathSideBar.h
 * @author jan
 * @date   2/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORPATHSIDEBAR_H
#define NOVELIST_TEXTEDITORPATHSIDEBAR_H

#include "TextEditorSideBar.h"

namespace novelist::editor {
    class TextEditorPathSideBar : public TextEditorHorizontalSideBar {
    Q_OBJECT

    public:
        TextEditorPathSideBar(gsl::not_null<TextEditor*> editor, QStringList path) noexcept;

        int sideBarHeight() const noexcept override;

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QColor const m_areaColor = QColor(250, 250, 250);
        QColor const m_fontColor = QColor(80, 80, 80);
        QColor const m_separatorColor = QColor(150, 150, 150);
        QFont const m_font;
        QString const m_separator = "  〉";
        QStringList const m_path;
    };
}

#endif //NOVELIST_TEXTEDITORPATHSIDEBAR_H
