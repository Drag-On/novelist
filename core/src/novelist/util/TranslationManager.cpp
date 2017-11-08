/**********************************************************
 * @file   TranslatorWrapper.cpp
 * @author jan
 * @date   11/8/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QCoreApplication>
#include <QDebug>
#include "util/TranslationManager.h"

namespace novelist {
    namespace internal {
        Locale::Locale(QLocale const& locale) noexcept
                :QLocale(locale)
        {
        }

        Locale& Locale::operator=(QLocale const& locale) noexcept
        {
            QLocale::operator=(locale);
            return *this;
        }

        bool Locale::operator<(Locale const& rhs) const noexcept
        {
            return bcp47Name() < rhs.bcp47Name();
        }
    }

    QLocale extractLocaleFromTranslationFile(QString filename) noexcept
    {
        filename.truncate(filename.lastIndexOf('.')); // "novelist_core.de"
        filename.remove(0, filename.lastIndexOf('_') + 1); // "de"

        return QLocale(filename);
    }

    TranslationManager& TranslationManager::instance() noexcept
    {
        static TranslationManager instance;
        return instance;
    }

    void TranslationManager::registerTranslation(QLocale const& locale, QString file) noexcept
    {
        m_registered[locale].push_back(file);
    }

    void TranslationManager::registerInDirectory(QDir const& dir, QString const& prefix) noexcept
    {
        QStringList fileNames = dir.entryList(QStringList(prefix + "_*.qm"));

        for (auto const& f : fileNames) {
            QLocale locale = extractLocaleFromTranslationFile(f);
            if (locale != QLocale("C"))
                registerTranslation(locale, dir.absoluteFilePath(f));
            else
                qInfo() << "Translation file" << f << "is invalid";
        }
    }

    void TranslationManager::switchLanguage(QLocale const& locale) noexcept
    {
        for (auto& t : m_active)
            qApp->removeTranslator(t.get());
        m_active.clear();
        for (auto f : m_registered[locale]) {
            auto t = std::make_unique<QTranslator>();
            int fileSepPos = f.lastIndexOf(QDir::separator());
            QString dir = f.left(fileSepPos);
            f.remove(0, fileSepPos + 1);
            if (t->load(f, dir)) {
                m_active.push_back(std::move(t));
                if (!qApp->installTranslator(m_active.back().get()))
                    qInfo() << "Failed to install translator for locale" << locale;
            }
            else
                qInfo() << "Translator at" << f << "could not be loaded";
        }
    }

    std::vector<QLocale> TranslationManager::knownLocales() const noexcept
    {
        std::vector<QLocale> locales;
        for (auto const& locale : m_registered.keys())
            locales.push_back(locale);
        return locales;
    }

    bool TranslationManager::isLocaleKnown(QLocale const& locale) const noexcept
    {
        for (auto const& l : m_registered.keys())
            if (l == locale)
                return true;
        return false;
    }
}