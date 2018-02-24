/**********************************************************
 * @file   TextEditorParagraphReadabilitySideBar.cpp
 * @author jan
 * @date   2/23/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <algorithm>
#include "editor/sidebars/TextEditorParagraphReadabilitySideBar.h"
#include "editor/TextEditor.h"

namespace novelist::editor {
    TextEditorSideBar::UpdateTriggers TextEditorParagraphReadabilitySideBar::updateTriggers() const noexcept
    {
        return {UpdateTrigger::VerticalScroll, UpdateTrigger::Resize, UpdateTrigger::TextChange};
    }

    QColor TextEditorParagraphReadabilitySideBar::colorFor(TextParagraph const& par) const noexcept
    {
        auto const& lang = editor()->getDocument()->properties().language();
        float readabilityIndex = 0;
        auto const str = par.text();
        auto const wordCount = countWords(str, lang);
        auto const sentenceCount = countSentences(str, lang);
        auto const syllableCount = countSyllables(str, lang);
        auto const asl = static_cast<float>(wordCount) / sentenceCount;
        auto const asw = static_cast<float>(syllableCount) / wordCount;
        switch (lang.language()) {
            case Language::English: {
                // Flesch-Reading-Ease
                // x = 206.835 - (1.015 * ASL) - (84.6 * ASW)
                readabilityIndex = std::clamp(206.835f - (1.015f * asl) - (84.6f * asw), 0.f, 100.f);
                break;
            }
            case Language::German: {
                // Flesch-Reading-Ease for German
                // x = 180 - ASL - (58.5 * ASW)
                readabilityIndex = std::clamp(180.f - asl - (58.5f * asw), 0.f, 100.f);
                break;
            }
        }
        float const fade = readabilityIndex / 100;

        if (fade < 0.5)
            return QColor::fromRgbF(1.f, 2.f * fade, 0.f);
        else
            return QColor::fromRgbF(1.f - ((fade - 0.5f) * 2.f), 1.f, 0.f);
    }
}