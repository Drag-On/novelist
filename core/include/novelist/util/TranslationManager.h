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

namespace novelist {
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
         * Register a new translation
         * @param locale Locale of the translation. This must not be unique, several translations can be registered per
         *               locale.
         * @param file Filename and path of the qm file to load for this locale
         */
        void registerTranslation(QLocale const& locale, QString const& file) noexcept;

        /**
         * Switch language to the requested locale. This unloads the previously active translation(s) and loads every
         * translation that has been registered for the new locale.
         * @param locale Locale to switch to
         */
        void switchLanguage(QLocale const& locale) noexcept;

        ~TranslationManager() = default;
        TranslationManager(TranslationManager const&) = delete;
        TranslationManager(TranslationManager&&) = delete;
        TranslationManager& operator=(TranslationManager const&) = delete;
        TranslationManager& operator=(TranslationManager&&) = delete;

    private:
        QMap<QString, std::vector<QString>> m_registered;
        std::vector<std::unique_ptr<QTranslator>> m_active;

        TranslationManager() = default;
    };
}
#endif //NOVELIST_TRANSLATIONMANAGER_H
