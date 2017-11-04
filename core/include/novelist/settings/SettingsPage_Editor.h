/**********************************************************
 * @file   SettingsPage_Editor.h
 * @author jan
 * @date   11/4/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSPAGE_EDITOR_H
#define NOVELIST_SETTINGSPAGE_EDITOR_H

#include <QtWidgets/QWidget>
#include <memory>
#include "SettingsPage.h"

namespace Ui {
    class SettingsPage_Editor;
}

namespace novelist {
    class SettingsPage_Editor_Creator;

    class SettingsPage_Editor  : public QWidget {
        Q_OBJECT

    public:
        /**
         * @param parent Parent widget
         * @param f Window flags
         */
        explicit SettingsPage_Editor(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~SettingsPage_Editor() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsPage_Editor> m_ui;

        friend SettingsPage_Editor_Creator;
    };

    class SettingsPage_Editor_Creator : public SettingsPage {
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

#endif //NOVELIST_SETTINGSPAGE_EDITOR_H
