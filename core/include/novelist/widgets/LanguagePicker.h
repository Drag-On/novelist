/**********************************************************
 * @file   LanguagePicker.h
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LANGUAGEPICKER_H
#define NOVELIST_LANGUAGEPICKER_H

#include <QtWidgets/QComboBox>
#include "model/Language.h"
#include <novelist_core_export.h>

namespace novelist {
    class NOVELIST_CORE_EXPORT LanguagePicker : public QComboBox {
    Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         */
        explicit LanguagePicker(QWidget* parent);

        /**
         * @return Currently selected language
         */
        Language currentLanguage() const;

        /**
         * @param lang Language to select
         */
        void setCurrentLanguage(Language lang);

        /**
         * Retranslate strings to current application language
         */
        void retranslateUi();

    signals:
        /**
         * Called when the current language has changed
         * @param lang New language
         */
        void currentLanguageChanged(Language lang);

    protected:
        void changeEvent(QEvent* e) override;

    private:
        void setup();
    };
}

#endif //NOVELIST_LANGUAGEPICKER_H
