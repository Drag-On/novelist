/**********************************************************
 * @file   LanguagePicker.cpp
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include "widgets/LanguagePicker.h"

namespace novelist {
    LanguagePicker::LanguagePicker(QWidget* parent)
            :QComboBox(parent)
    {
        setup();
    }

    Language LanguagePicker::currentLanguage() const
    {
        return currentData().value<Language>();
    }

    void LanguagePicker::setCurrentLanguage(Language lang)
    {
        setCurrentIndex(findData(QVariant::fromValue(lang)));
    }

    void LanguagePicker::setup()
    {
        setEditable(false);
        setInsertPolicy(InsertPolicy::InsertAlphabetically);

        for (auto i = static_cast<int>(Language::First); i <= static_cast<int>(Language::Last); ++i) {
            auto l = static_cast<Language>(i);
            QIcon icon = lang::icon(l);
            QString text = lang::description(l);
            QVariant userData = QVariant::fromValue(l);
            addItem(icon, text, userData);
        }

        setCurrentIndex(0);

        connect(this, qOverload<int>(&LanguagePicker::currentIndexChanged),
                [&](int idx) { emit currentLanguageChanged(itemData(idx).value<Language>()); });
    }
}