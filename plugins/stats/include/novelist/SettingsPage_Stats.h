/**********************************************************
 * @file   SettingsPage_Stats.h
 * @author jan
 * @date   11/19/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSPAGE_STATS_H
#define NOVELIST_SETTINGSPAGE_STATS_H

#include <QtCore/QEvent>
#include <QtWidgets/QWidget>
#include <QtCore/QProcess>
#include <gsl/gsl>
#include <memory>
#include <settings/SettingsPage.h>
#include <util/ConnectionWrapper.h>

namespace Ui {
    class SettingsPage_Stats;
}

namespace novelist {
    class SettingsPage_Stats_Creator;
    class ProjectStatCollector;

    class SettingsPage_Stats : public QWidget {
    Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         * @param f Window flags
         */
        explicit SettingsPage_Stats(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~SettingsPage_Stats() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsPage_Stats> m_ui;

        friend SettingsPage_Stats_Creator;
    };

    class SettingsPage_Stats_Creator : public SettingsPage {
    Q_OBJECT

    public:
        SettingsPage_Stats_Creator(gsl::not_null<ProjectStatCollector*> collector) noexcept;

        ~SettingsPage_Stats_Creator() noexcept;

        QString name() noexcept override;

        QString uid() noexcept override;

        void initialize(QWidget* widget, QSettings const& settings) noexcept override;

        void apply(QWidget const* widget, QSettings& settings) noexcept override;

        void initiateUpdate(QSettings const& settings) noexcept override;

        void restoreDefaults(QWidget const* widget) noexcept override;

        QWidget* createWidget() noexcept override;

    private:
        gsl::not_null<ProjectStatCollector*> m_collector;
    };
}

#endif //NOVELIST_SETTINGSPAGE_STATS_H
