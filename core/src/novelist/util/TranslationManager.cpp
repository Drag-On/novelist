/**********************************************************
 * @file   TranslatorWrapper.cpp
 * @author jan
 * @date   11/8/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QCoreApplication>
#include "util/TranslationManager.h"

namespace novelist {
    TranslationManager& TranslationManager::instance() noexcept
    {
        static TranslationManager instance;
        return instance;
    }

    void TranslationManager::registerTranslation(QLocale const& locale, QString const& file) noexcept
    {
        m_registered[locale.bcp47Name()].push_back(file);
    }

    void TranslationManager::switchLanguage(QLocale const& locale) noexcept
    {
        for (auto& t : m_active)
            qApp->removeTranslator(t.get());
        m_active.clear();
        for (auto& f : m_registered[locale.bcp47Name()]) {
            auto t = std::make_unique<QTranslator>();
            if (t->load(f)) {
                m_active.push_back(std::move(t));
                qApp->installTranslator(m_active.back().get());
            }
        }
    }
}