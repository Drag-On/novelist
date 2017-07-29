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

namespace novelist {

    /**
     * Supported novel languages
     */
    enum class Language {
        de_DE,
        de_AT,
        de_CH,
        en_UK,
        en_US,
        en_AU,
    };

    /**
     * Language-related free functions
     */
    namespace lang {

        /**
         * @param lang Language ID
         * @return Identifier as string in the form aa-AA, e.g. "en-US".
         */
        QString identifier(Language lang);

        /**
         * @param lang Language ID
         * @return Country code as string, e.g. "en"
         */
        QString countryCode(Language lang);

        /**
         * @param lang Language ID
         * @return Description string, e.g. "English (United States)"
         */
        QString description(Language lang);

        /**
         * Attempts to load the system's flag icon for the language. Might return an empty icon, if not available.
         * @param lang Language ID
         * @return Icon for the language
         */
        QIcon icon(Language lang);

        /**
         * @param identifier String containing language identifier of the form aa-AA, e.g. "en-US".
         * @return The appropriate language ID
         * @throws language_error if the identifier was invalid
         */
        Language fromIdentifier(QString const& identifier);
    }

    /**
     * Exception thrown when an unknown (or unsupported) language is requested
     */
    class language_error : public std::logic_error {
    public:
        using std::logic_error::logic_error;
    };
}

#endif //NOVELIST_LANGUAGE_H
