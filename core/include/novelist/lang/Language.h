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

namespace novelist {

    /**
     * A language which can be used to write a novel in
     * TODO: Validate on construction
     */
    class Language {
    public:
        /**
         * Construct language object
         * @param shortname Short name of the language, e.g. "en_GB" or "de_DE"
         */
        explicit Language(QString const& shortname) noexcept;

        /**
         * @return Language shortname
         */
        QString const& shortname() const;

        /**
         * @return Name of the language
         */
        QString name() const;

    private:
        QString m_short;
    };
}

#endif //NOVELIST_LANGUAGE_H
