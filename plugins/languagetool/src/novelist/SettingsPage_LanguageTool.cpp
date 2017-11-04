/**********************************************************
 * @file   SettingsPage_LanguageTool.cpp
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtWidgets/QMainWindow>
#include "SettingsPage_LanguageTool.h"
#include "ui_SettingsPage_LanguageTool.h"

namespace novelist {
    SettingsPage_LanguageTool::SettingsPage_LanguageTool(QWidget* parent, Qt::WindowFlags f)
            :QWidget(parent, f),
             m_ui{std::make_unique<Ui::SettingsPage_LanguageTool>()}
    {
        m_ui->setupUi(this);
    }

    SettingsPage_LanguageTool::~SettingsPage_LanguageTool() = default;

    void SettingsPage_LanguageTool::retranslateUi()
    {
        m_ui->retranslateUi(this);
    }

    void SettingsPage_LanguageTool::changeEvent(QEvent* event)
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

    SettingsPage_LanguageTool_Creator::~SettingsPage_LanguageTool_Creator()
    {
        m_serverProcess.close();
    }

    QString SettingsPage_LanguageTool_Creator::name() noexcept
    {
        return tr("LanguageTool");
    }

    QString SettingsPage_LanguageTool_Creator::uid() noexcept
    {
        return "languagetool";
    }

    void SettingsPage_LanguageTool_Creator::initialize(QWidget* widget, QSettings const& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_LanguageTool*>(widget);

        page->m_ui->lineEditServerUrl->setText(settings.value("url", "http://localhost:8081").toString());
        page->m_ui->lineEditIgnoreRules->setText(settings.value("ignore_rules", "").toString());
        page->m_ui->groupBoxAutoStart->setChecked(settings.value("autostart", true).toBool());
        page->m_ui->filePicker->setSelectedFile(settings.value("path", "").toString());
        page->m_ui->lineEditJava->setText(settings.value("java", "java").toString());
        page->m_ui->lineEditServerArguments->setText(
                settings.value("arguments", "org.languagetool.server.HTTPServer --port 8081").toString());
    }

    void SettingsPage_LanguageTool_Creator::apply(QWidget const* widget, QSettings& settings) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_LanguageTool const*>(widget);

        settings.setValue("url", page->m_ui->lineEditServerUrl->text());
        settings.setValue("ignore_rules", page->m_ui->lineEditIgnoreRules->text());
        settings.setValue("autostart", page->m_ui->groupBoxAutoStart->isChecked());
        settings.setValue("path", page->m_ui->filePicker->selectedFile());
        settings.setValue("java", page->m_ui->lineEditJava->text());
        settings.setValue("arguments", page->m_ui->lineEditServerArguments->text());
    }

    void SettingsPage_LanguageTool_Creator::initiateUpdate(QSettings const& settings) noexcept
    {
        if (settings.value("autostart").toBool() && m_serverProcess.state() == QProcess::ProcessState::NotRunning) {
            // Start server
            QString prog = settings.value("java").toString();
            QStringList arguments;
            arguments.push_back("-cp");
            arguments.push_back(settings.value("path").toString());
            arguments << settings.value("arguments").toString().split(QRegExp("\\s+"), QString::SkipEmptyParts);

            m_serverProcess.start(prog, arguments);
            QTimer::singleShot(500, [this] {
                if (m_serverProcess.state() != QProcess::ProcessState::Running) {
                    m_serverProcess.close();
                    QMessageBox msgBox;
                    msgBox.setWindowTitle(tr("Warning"));
                    msgBox.setText(tr("The LanguageTool server could not be started."));
                    msgBox.setInformativeText(tr("Please check whether the settings are correct."));
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.exec();
                }
            });
        }
    }

    void SettingsPage_LanguageTool_Creator::restoreDefaults(QWidget const* widget) noexcept
    {
        auto* page = dynamic_cast<SettingsPage_LanguageTool const*>(widget);

        page->m_ui->lineEditServerUrl->setText("http://localhost:8081");
        page->m_ui->lineEditIgnoreRules->setText("");
        page->m_ui->groupBoxAutoStart->setChecked(true);
        page->m_ui->filePicker->setSelectedFile("");
        page->m_ui->lineEditJava->setText("java");
        page->m_ui->lineEditServerArguments->setText("org.languagetool.server.HTTPServer --port 8081");
    }

    QWidget* SettingsPage_LanguageTool_Creator::createWidget() noexcept
    {
        return new SettingsPage_LanguageTool;
    }
}