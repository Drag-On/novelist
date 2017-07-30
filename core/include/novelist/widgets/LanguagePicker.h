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

namespace novelist {
    class LanguagePicker : public QComboBox {
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

    signals:
        /**
         * Called when the current language has changed
         * @param lang New language
         */
        void currentLanguageChanged(Language lang);

    private:
        void setup();
    };
}

#endif //NOVELIST_LANGUAGEPICKER_H
