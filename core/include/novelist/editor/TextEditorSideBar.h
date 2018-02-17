/**********************************************************
 * @file   TextEditorSideBar.h
 * @author jan
 * @date   2/11/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTEDITORSIDEBAR_H
#define NOVELIST_TEXTEDITORSIDEBAR_H

#include <bitset>
#include <QtWidgets/QWidget>
#include <gsl/gsl>
#include "editor/document/TextParagraph.h"

namespace novelist::editor {
    class TextEditor;

    struct UpdateTrigger {
        enum {
            Resize = 0,
            ParagraphCountChange,
            TextChange,
            VerticalScroll,
            HorizontalScroll,
            __Count,
        };
    };

    class TextEditorSideBar : public QWidget {
        Q_OBJECT

    public:
        using UpdateTriggers = std::bitset<UpdateTrigger::__Count>;

        explicit TextEditorSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        TextEditor const* editor() const noexcept;

        virtual UpdateTriggers updateTriggers() const noexcept = 0;

    private:
        TextEditor* m_editor;
    };


    class TextEditorVerticalSideBar : public TextEditorSideBar {
        Q_OBJECT

    public:
        explicit TextEditorVerticalSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        virtual int sideBarWidth() const noexcept = 0;

        QSize sizeHint() const override;

        UpdateTriggers updateTriggers() const noexcept override;

    private:
        TextEditor const* m_editor;
    };

    class TestSideBar : public TextEditorVerticalSideBar {
    public:
        TestSideBar(gsl::not_null<TextEditor*> editor, QColor clr);

        int sideBarWidth() const noexcept override;

        UpdateTriggers updateTriggers() const noexcept override;

    protected:
        void paintEvent(QPaintEvent* event) override;

        QColor m_color;
    };
}

#endif //NOVELIST_TEXTEDITORSIDEBAR_H
