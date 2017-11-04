/**********************************************************
 * @file   SettingsPage_Editor.cpp
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtCore/QEvent>
#include "settings/SettingsPage_Editor.h"
#include "ui_SettingsPage_Editor.h"

namespace novelist {
    SettingsPage_Editor::SettingsPage_Editor(QWidget* parent, Qt::WindowFlags f)
            :QWidget(parent, f),
             m_ui{std::make_unique<Ui::SettingsPage_Editor>()}
    {
        m_ui->setupUi(this);
    }

    SettingsPage_Editor::~SettingsPage_Editor() = default;

    void SettingsPage_Editor::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void SettingsPage_Editor::changeEvent(QEvent* event)
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

    QString SettingsPage_Editor_Creator::name() noexcept
    {
        return QObject::tr("Editor", "SettingsPage_Editor");
    }

    QString SettingsPage_Editor_Creator::uid() noexcept
    {
        return "editor";
    }

    void SettingsPage_Editor_Creator::initialize(QWidget* widget, QSettings const& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_Editor*>(widget);

        int widthLimit = settings.value("width_limit", 0).toInt();
        page->m_ui->checkBoxLimitWidth->setChecked(widthLimit > 0);
        page->m_ui->spinBoxTextWidth->setValue(widthLimit);
        page->m_ui->checkBoxShowParNo->setChecked(settings.value("show_par_no", true).toBool());
    }

    void SettingsPage_Editor_Creator::apply(QWidget const* widget, QSettings& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_Editor const*>(widget);

        int widthLimit = page->m_ui->spinBoxTextWidth->value();
        if (!page->m_ui->checkBoxLimitWidth->isChecked())
            widthLimit = 0;
        settings.setValue("width_limit", widthLimit);
        settings.setValue("show_par_no", page->m_ui->checkBoxShowParNo->isChecked());
    }

    void SettingsPage_Editor_Creator::initiateUpdate(QSettings const& /*settings*/) noexcept
    {
        emit updateInitiated();
    }

    void SettingsPage_Editor_Creator::restoreDefaults(QWidget const* widget) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_Editor const*>(widget);

        page->m_ui->checkBoxLimitWidth->setChecked(false);
        page->m_ui->spinBoxTextWidth->setValue(page->m_ui->spinBoxTextWidth->minimum());
        page->m_ui->checkBoxShowParNo->setChecked(true);
    }

    QWidget* SettingsPage_Editor_Creator::createWidget() noexcept
    {
        return new SettingsPage_Editor;
    }
}