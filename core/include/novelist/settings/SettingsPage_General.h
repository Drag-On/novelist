/**********************************************************
 * @file   SettingsPage_General.h
 * @author jan
 * @date   11/2/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSPAGE_GENERAL_H
#define NOVELIST_SETTINGSPAGE_GENERAL_H

#include <QtWidgets/QWidget>
#include "SettingsPage.h"

namespace Ui {
    class SettingsPage_General;
}

namespace novelist {
    class SettingsPage_General_Creator;

    class NOVELIST_CORE_EXPORT SettingsPage_General : public QWidget {
    Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         * @param f Window flags
         */
        explicit SettingsPage_General(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~SettingsPage_General() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsPage_General> m_ui;

        friend SettingsPage_General_Creator;
    };

    class NOVELIST_CORE_EXPORT SettingsPage_General_Creator : public SettingsPage {
    Q_OBJECT

    public:
        QString name() noexcept override;

        QString uid() noexcept override;

        void initialize(QWidget* widget, QSettings const& settings) noexcept override;

        void apply(QWidget const* widget, QSettings& settings) noexcept override;

        void initiateUpdate(QSettings const& settings) noexcept override;

        void restoreDefaults(QWidget const* widget) noexcept override;

        QWidget* createWidget() noexcept override;
    };
}

#endif //NOVELIST_SETTINGSPAGE_GENERAL_H
