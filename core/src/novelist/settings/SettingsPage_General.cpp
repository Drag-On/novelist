/**********************************************************
 * @file   SettingsPage_General.cpp
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include <QDebug>
#include "util/TranslationManager.h"
#include "settings/SettingsPage_General.h"
#include "ui_SettingsPage_General.h"

namespace novelist {
    SettingsPage_General::SettingsPage_General(QWidget* parent, Qt::WindowFlags f)
            :QWidget(parent, f),
             m_ui{std::make_unique<Ui::SettingsPage_General>()}
    {
        m_ui->setupUi(this);
    }

    SettingsPage_General::~SettingsPage_General() = default;

    void SettingsPage_General::retranslateUi()
    {
        m_ui->retranslateUi(this);
        for (int i = 0; i < m_ui->langComboBox->count(); ++i) {
            QLocale locale = m_ui->langComboBox->itemData(i).toLocale();
            m_ui->langComboBox->setItemText(i, QLocale(locale).nativeLanguageName());
        }
    }

    void SettingsPage_General::changeEvent(QEvent* event)
    {
        QWidget::changeEvent(event);
        switch (event->type()) {
            case QEvent::LanguageChange:
                retranslateUi();
                break;
            default:
                break;
        }
    }

    QString SettingsPage_General_Creator::name() noexcept
    {
        return QObject::tr("General", "SettingsPage_General");
    }

    QString SettingsPage_General_Creator::uid() noexcept
    {
        return "general_ui";
    }

    void SettingsPage_General_Creator::initialize(QWidget* widget, QSettings const& settings) noexcept
    {
        restoreDefaults(widget);

        auto* page = dynamic_cast<SettingsPage_General*>(widget);

        if (settings.contains("font")) {
            QFont font;
            font.fromString(settings.value("font").toString());
            page->m_ui->fontComboBox->setCurrentFont(font);
            page->m_ui->fontSizeSpinBox->setValue(font.pointSize());
        }

        if (settings.contains("lang")) {
            QLocale locale(settings.value("lang").toString());
            page->m_ui->langComboBox->setCurrentText(QLocale(locale).nativeLanguageName());
        }
    }

    void SettingsPage_General_Creator::apply(QWidget const* widget, QSettings& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_General const*>(widget);

        QFont font = page->m_ui->fontComboBox->currentFont();
        font.setPointSize(page->m_ui->fontSizeSpinBox->value());
        settings.setValue("font", font.toString());

        QLocale locale = page->m_ui->langComboBox->currentData(Qt::UserRole).toLocale();
        settings.setValue("lang", locale.bcp47Name());
    }

    void SettingsPage_General_Creator::initiateUpdate(QSettings const& settings) noexcept
    {
        if (settings.contains("font")) {
            QFont font;
            font.fromString(settings.value("font").toString());
            qApp->setFont(font);
        }

        if (settings.contains("lang")) {
            QLocale locale(settings.value("lang").toString());
            if (TranslationManager::instance().isLocaleKnown(locale))
                TranslationManager::instance().switchLanguage(locale);
        }
    }

    void SettingsPage_General_Creator::restoreDefaults(QWidget const* widget) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_General const*>(widget);

        QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::SystemFont::GeneralFont);
        page->m_ui->fontComboBox->setCurrentFont(defaultFont);
        page->m_ui->fontSizeSpinBox->setValue(defaultFont.pointSize());

        auto knownLocales = TranslationManager::instance().knownLocales();
        if (!knownLocales.empty()) {
            page->m_ui->langComboBox->clear();
            QLocale defaultLocale;
            int defaultIdx = -1;
            int curIdx = 0;
            for (auto& locale : knownLocales) {
                if (locale.language() == defaultLocale.language())
                    defaultIdx = curIdx;
                page->m_ui->langComboBox->addItem(QLocale(locale).nativeLanguageName(), locale);
                ++curIdx;
            }
            if (defaultIdx >= 0)
                page->m_ui->langComboBox->setCurrentIndex(defaultIdx);
            else
                page->m_ui->langComboBox->setCurrentIndex(0);
        }
    }

    QWidget* SettingsPage_General_Creator::createWidget() noexcept
    {
        return new SettingsPage_General;
    }
}
