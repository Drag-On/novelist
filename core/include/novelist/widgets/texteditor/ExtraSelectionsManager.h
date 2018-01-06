/**********************************************************
 * @file   ExtraSelectionsManager.h
 * @author jan
 * @date   12/16/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_EXTRASELECTIONSMANAGER_H
#define NOVELIST_EXTRASELECTIONSMANAGER_H

#include <QtWidgets/QTextEdit>
#include <gsl/gsl>

namespace novelist {
    /**
     * Type of extra selection, e.g. the current line highlight or matching brackets
     */
    enum class ExtraSelectionType {
        CurrentLine,  //!< Current line indicator
        MatchingChars,//!< Matching characters indicator
        Find,         //!< Find result indicator
    };

    /**
     * Handles extra selections in a QTextEdit and allows to tag them and remove all selections with a tag
     */
    class ExtraSelectionsManager {
    public:
        using ExtraSelection = QTextEdit::ExtraSelection;

        /**
         * Constructor
         * @param editor Editor whose extra selections should be managed
         */
        explicit ExtraSelectionsManager(gsl::not_null<QTextEdit*> editor) noexcept;

        /**
         * Inserts a new extra selection
         * @note Changes will only be reflected after calling commit(). This is for performance reasons.
         * @param extraSelect Extra selection to insert
         * @param type Type to tag the selection with
         */
        void insert(ExtraSelection extraSelect, ExtraSelectionType type) noexcept;

        /**
         * Erase all extra selections with a certain type
         * @note Changes will only be reflected after calling commit(). This is for performance reasons.
         * @param type Type to remove
         */
        void erase(ExtraSelectionType type) noexcept;

        /**
         * Erase all extra selections
         * @note Changes will only be reflected after calling commit(). This is for performance reasons.
         */
        void eraseAll() noexcept;

        /**
         * Commit all previous changes to the extra selections, such as adding or removing them. This will actually
         * change the extra selections of the underlying editor.
         */
        void commit() noexcept;

    private:
        gsl::not_null<QTextEdit*> m_editor;
        QMultiMap<ExtraSelectionType, ExtraSelection> m_extraSelections;

        void setCurrentSelections() noexcept;
    };
}

#endif //NOVELIST_EXTRASELECTIONSMANAGER_H
