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

    /**
     * Signals that should trigger a side bar update
     */
    struct UpdateTrigger {
        enum {
            Resize = 0, //!< Window resize
            ParagraphCountChange, //!< Amount of paragraphs changes
            LineCountChange, //!< Amount of lines changes
            TextChange, //!< Text content changes
            VerticalScroll, //!< Window was scrolled vertically
            HorizontalScroll, //!< Window was scrolled horizontally
            __Count, //!< Special value that counts above options
        };
    };

    /**
     * Virtual base class for side bars. Side bars can be added to the left, right, top or bottom of a TextEditor widget.
     */
    class TextEditorSideBar : public QWidget {
        Q_OBJECT

    public:
        using UpdateTriggers = std::bitset<UpdateTrigger::__Count>;

        /**
         * Constructor
         * @param editor Editor to add to
         */
        explicit TextEditorSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        /**
         * @return Owning editor
         */
        TextEditor const* editor() const noexcept;

        /**
         * @return Signals that should trigger an update of this side bar
         */
        virtual UpdateTriggers updateTriggers() const noexcept = 0;

    private:
        TextEditor* m_editor;
    };

    /**
     * Virtual base class for vertical side bars, either on the left or right of the editor
     */
    class TextEditorVerticalSideBar : public TextEditorSideBar {
        Q_OBJECT

    public:
        /**
         * Constructor
         * @param editor Owning editor
         */
        explicit TextEditorVerticalSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        /**
         * @return The width of this side bar
         */
        virtual int sideBarWidth() const noexcept = 0;

        /**
         * @return Size hint
         */
        QSize sizeHint() const override;

        /**
         * @return Signals that should trigger an update of this side bar
         */
        UpdateTriggers updateTriggers() const noexcept override;

    private:
        TextEditor const* m_editor;
    };

    /**
     * Virtual base class for horizontal side bars, either at the top or bottom of the editor
     */
    class TextEditorHorizontalSideBar : public TextEditorSideBar {
    Q_OBJECT

    public:
        /**
         * Construcotr
         * @param editor Owning editor
         */
        explicit TextEditorHorizontalSideBar(gsl::not_null<TextEditor*> editor) noexcept;

        /**
         * @return The height of this side bar
         */
        virtual int sideBarHeight() const noexcept = 0;

        /**
         * @return Size hint
         */
        QSize sizeHint() const override;

        /**
         * @return Signals that should trigger an update of this side bar
         */
        UpdateTriggers updateTriggers() const noexcept override;

    private:
        TextEditor const* m_editor;
    };
}

#endif //NOVELIST_TEXTEDITORSIDEBAR_H
