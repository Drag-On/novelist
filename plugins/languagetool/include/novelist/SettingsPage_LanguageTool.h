/**********************************************************
 * @file   SettingsPage_LanguageTool.h
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSPAGE_LANGUAGETOOL_H
#define NOVELIST_SETTINGSPAGE_LANGUAGETOOL_H

#include <QtCore/QEvent>
#include <QtWidgets/QWidget>
#include <QtCore/QProcess>
#include <memory>
#include <settings/SettingsPage.h>
#include <util/ConnectionWrapper.h>

namespace Ui {
    class SettingsPage_LanguageTool;
}

namespace novelist {
    class SettingsPage_LanguageTool_Creator;

    class SettingsPage_LanguageTool : public QWidget {
    Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         * @param f Window flags
         */
        explicit SettingsPage_LanguageTool(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~SettingsPage_LanguageTool() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsPage_LanguageTool> m_ui;

        friend SettingsPage_LanguageTool_Creator;
    };

    class SettingsPage_LanguageTool_Creator : public SettingsPage {
    Q_OBJECT

    public:
        ~SettingsPage_LanguageTool_Creator() noexcept;

        QString name() noexcept override;

        QString uid() noexcept override;

        void initialize(QWidget* widget, QSettings const& settings) noexcept override;

        void apply(QWidget const* widget, QSettings& settings) noexcept override;

        void initiateUpdate(QSettings const& settings) noexcept override;

        void restoreDefaults(QWidget const* widget) noexcept override;

        QWidget* createWidget() noexcept override;

    private:
        QProcess m_serverProcess;
    };
}

#endif //NOVELIST_SETTINGSPAGE_LANGUAGETOOL_H
