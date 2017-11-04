/**********************************************************
 * @file   SettingsPage_General.cpp
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
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
        auto* page = dynamic_cast<SettingsPage_General*>(widget);

        QFont font;
        if (settings.contains("font"))
            font.fromString(settings.value("font").toString());
        else
            font = page->font();
        page->m_ui->fontComboBox->setCurrentFont(font);
        page->m_ui->fontSizeSpinBox->setValue(font.pointSize());
        page->m_ui->langComboBox->setCurrentIndex(0); // TODO: Fix this
    }

    void SettingsPage_General_Creator::apply(QWidget const* widget, QSettings& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_General const*>(widget);

        QFont font = page->m_ui->fontComboBox->currentFont();
        font.setPointSize(page->m_ui->fontSizeSpinBox->value());
        settings.setValue("font", font.toString());
        settings.setValue("lang", "english"); // TODO: fix this
    }

    void SettingsPage_General_Creator::initiateUpdate(QSettings const& settings) noexcept
    {
        QFont font;
        if (settings.contains("font"))
            font.fromString(settings.value("font").toString());
        qApp->setFont(font);
    }

    void SettingsPage_General_Creator::restoreDefaults(QWidget const* widget) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_General const*>(widget);

        QFont defaultFont = QFontDatabase::systemFont(QFontDatabase::SystemFont::GeneralFont);
        page->m_ui->fontComboBox->setCurrentFont(defaultFont);
        page->m_ui->fontSizeSpinBox->setValue(defaultFont.pointSize());
        page->m_ui->langComboBox->setCurrentIndex(0); // TODO: Fix this
    }

    QWidget* SettingsPage_General_Creator::createWidget() noexcept
    {
        return new SettingsPage_General;
    }
}
