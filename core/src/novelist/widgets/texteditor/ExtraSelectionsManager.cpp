/**********************************************************
 * @file   ExtraSelectionsManager.cpp
 * @author jan
 * @date   12/16/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include "widgets/texteditor/ExtraSelectionsManager.h"

namespace novelist {
    ExtraSelectionsManager::ExtraSelectionsManager(gsl::not_null<QTextEdit*> editor) noexcept
            :m_editor(editor)
    {
    }

    void ExtraSelectionsManager::insert(ExtraSelection extraSelect, ExtraSelectionType type) noexcept
    {
        m_extraSelections.insert(type, extraSelect);
    }

    void ExtraSelectionsManager::erase(ExtraSelectionType type) noexcept
    {
        m_extraSelections.remove(type);
    }

    void ExtraSelectionsManager::eraseAll() noexcept
    {
        m_extraSelections.clear();
    }

    void ExtraSelectionsManager::commit() noexcept
    {
        setCurrentSelections();
    }

    void ExtraSelectionsManager::setCurrentSelections() noexcept
    {
        QList<ExtraSelection> extraSelections;
        for (auto const& s : m_extraSelections)
            extraSelections.append(s);
        m_editor->setExtraSelections(extraSelections);
    }
}