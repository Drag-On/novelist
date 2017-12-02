/**********************************************************
 * @file   Language.h
 * @author jan
 * @date   17.06.17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_LANGUAGE_H
#define NOVELIST_LANGUAGE_H

#include <QtCore/QString>
#include <QtGui/QIcon>
#include <stdexcept>
#include <novelist_core_export.h>

namespace novelist {

    /**
     * Supported novel languages
     */
    enum class Language {
        de_DE = 0,
        de_AT,
        de_CH,
        en_UK,
        en_US,
        en_AU,
        First = de_DE,
        Last = en_AU,
    };

    /**
     * Language-related free functions
     */
    namespace lang {

        /**
         * @param lang Language ID
         * @return Identifier as string in the form aa-AA, e.g. "en-US".
         */
        NOVELIST_CORE_EXPORT QString identifier(Language lang);

        /**
         * @param lang Language ID
         * @return Country code as string, e.g. "US"
         */
        NOVELIST_CORE_EXPORT QString countryCode(Language lang);

        /**
         * @param lang Language ID
         * @return Language code as string, e.g. "en"
         */
        NOVELIST_CORE_EXPORT QString languageCode(Language lang);

        /**
         * @param lang Language ID
         * @return Description string, e.g. "English (United States)"
         */
        NOVELIST_CORE_EXPORT QString description(Language lang);

        /**
         * Attempts to load the system's flag icon for the language. Might return an empty icon, if not available.
         * @param lang Language ID
         * @return Icon for the language
         */
        NOVELIST_CORE_EXPORT QIcon icon(Language lang);

        /**
         * @param identifier String containing language identifier of the form aa-AA, e.g. "en-US".
         * @return The appropriate language ID
         * @throws language_error if the identifier was invalid
         */
        NOVELIST_CORE_EXPORT Language fromIdentifier(QString const& identifier);
    }

    /**
     * Exception thrown when an unknown (or unsupported) language is requested
     */
    class NOVELIST_CORE_EXPORT language_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };
}

Q_DECLARE_METATYPE(novelist::Language)

#endif //NOVELIST_LANGUAGE_H
