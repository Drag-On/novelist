/**********************************************************
 * @file   TranslatorWrapper.h
 * @author jan
 * @date   11/8/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TRANSLATIONMANAGER_H
#define NOVELIST_TRANSLATIONMANAGER_H

#include <QtCore/QTranslator>
#include <QtCore/QLocale>
#include <QtCore/QFile>
#include <memory>
#include <vector>
#include <QtCore/QDir>

namespace novelist {
    namespace internal {
        struct Locale : public QLocale {
            using QLocale::QLocale;
            using QLocale::operator=;

            Locale(QLocale const& locale) noexcept;

            Locale& operator=(QLocale const& locale) noexcept;

            bool operator<(Locale const& rhs) const noexcept;
        };
    }

    /**
     * Extracts a locale based on the filename of a translation file
     * @note The filename is assumed to have the following format: "<Name>_<lang>.qm", e.g. "novelist_core_de.qm"
     * @param filename Translation filename
     * @return The detected locale or the "C" locale, if the filename couldn't be parsed
     */
    QLocale extractLocaleFromTranslationFile(QString filename) noexcept;

    /**
     * Manages translations of the whole program, can be used to switch language at runtime
     */
    class TranslationManager {
    public:
        /**
         * @return Reference to the singleton instance of this class
         */
        static TranslationManager& instance() noexcept;

        /**
         * Registers a new translation
         * @param locale Locale of the translation. This must not be unique, several translations can be registered per
         *               locale.
         * @param file Filename and path of the qm file to load for this locale
         */
        void registerTranslation(QLocale const& locale, QString file) noexcept;

        /**
         * Registers all translations found in a particular directory
         * @param dir Directory
         * @param prefix Prefix of all language files, e.g. "novelist_core"
         * @param noNew Indicates whether to allow loading of locales which are not known already
         */
        void registerInDirectory(QDir const& dir, QString const& prefix, bool noNew = false) noexcept;

        /**
         * Switch language to the requested locale. This unloads the previously active translation(s) and loads every
         * translation that has been registered for the new locale.
         * @param locale Locale to switch to
         */
        void switchLanguage(QLocale const& locale) noexcept;

        /**
         * @return List of all locales for which there are translations
         */
        std::vector<QLocale> knownLocales() const noexcept;

        /**
         * Checks whether there are translations for a locale
         * @param locale Locale
         * @return True in case translations for this locale have been registered before
         */
        bool isLocaleKnown(QLocale const& locale) const noexcept;

        ~TranslationManager() = default;
        TranslationManager(TranslationManager const&) = delete;
        TranslationManager(TranslationManager&&) = delete;
        TranslationManager& operator=(TranslationManager const&) = delete;
        TranslationManager& operator=(TranslationManager&&) = delete;

    private:
        QMap<internal::Locale, std::vector<QString>> m_registered;
        std::vector<std::unique_ptr<QTranslator>> m_active;

        TranslationManager() = default;
    };
}
#endif //NOVELIST_TRANSLATIONMANAGER_H
