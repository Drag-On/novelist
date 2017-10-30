/**********************************************************
 * @file   Language.cpp
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/

#include <QtCore/QCoreApplication>
#include "model/Language.h"

namespace novelist::lang {

    QString identifier(Language lang)
    {
        switch (lang) {
            case Language::de_DE:
                return "de-DE";
            case Language::de_AT:
                return "de-AT";
            case Language::de_CH:
                return "de-CH";
            case Language::en_UK:
                return "en-UK";
            case Language::en_US:
                return "en-US";
            case Language::en_AU:
                return "en-AU";
        }
        throw language_error{"No identifier for this language known. Probably forgot to update switch statement."};
    }

    QString countryCode(Language lang)
    {
        switch (lang) {
            case Language::de_DE:
                return "DE";
            case Language::de_AT:
                return "AT";
            case Language::de_CH:
                return "CH";
            case Language::en_UK:
                return "UK";
            case Language::en_US:
                return "US";
            case Language::en_AU:
                return "AU";
        }
        throw language_error{"No country code for this language known. Probably forgot to update switch statement."};
    }

    QString languageCode(Language lang)
    {
        switch (lang) {
            case Language::de_DE:
            case Language::de_AT:
            case Language::de_CH:
                return "de";
            case Language::en_UK:
            case Language::en_US:
            case Language::en_AU:
                return "en";
        }
        throw language_error{"No language code for this language known. Probably forgot to update switch statement."};
    }

    QString description(Language lang)
    {
        switch (lang) {
            case Language::de_DE:
                return QCoreApplication::translate("Language", "German (Germany)");
            case Language::de_AT:
                return QCoreApplication::translate("Language", "German (Austria)");
            case Language::de_CH:
                return QCoreApplication::translate("Language", "German (Switzerland)");
            case Language::en_UK:
                return QCoreApplication::translate("Language", "English (United Kingdom)");
            case Language::en_US:
                return QCoreApplication::translate("Language", "English (United States)");
            case Language::en_AU:
                return QCoreApplication::translate("Language", "English (Australia)");
        }
        throw language_error{"No description for this language known. Probably forgot to update switch statement."};
    }

    QIcon icon(Language lang)
    {
        QIcon icon;
        QString themeName = "flag-" + countryCode(lang).toLower();
        if (QIcon::hasThemeIcon(themeName))
            icon = QIcon::fromTheme(themeName);
        else
            icon.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        return icon;
    }

    Language fromIdentifier(QString const& identifier)
    {
        if (identifier == "de-DE")
            return Language::de_DE;
        if (identifier == "de-AT")
            return Language::de_AT;
        if (identifier == "de-CH")
            return Language::de_CH;
        if (identifier == "en-UK")
            return Language::en_UK;
        if (identifier == "en-US")
            return Language::en_US;
        if (identifier == "en-AU")
            return Language::en_AU;

        throw language_error{"Invalid identifier."};
    }
}