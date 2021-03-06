/**********************************************************
 * @file   SettingsWindow.h
 * @author jan
 * @date   11/1/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SETTINGSWINDOW_H
#define NOVELIST_SETTINGSWINDOW_H

#include <memory>
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidgetItem>
#include <novelist_core_export.h>

namespace Ui {
    class SettingsDialog;
}

namespace novelist {
    class NOVELIST_CORE_EXPORT SettingsWindow : public QDialog {
    Q_OBJECT

    public:
        /**
         * @param parent Parent window
         * @param f Window flags
         */
        explicit SettingsWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~SettingsWindow() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

        void accept() override;

        void reject() override;

        void apply();

        void restoreDefaults();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsDialog> m_ui;

        void onChangePage(QListWidgetItem* current, QListWidgetItem* previous);

        void setupPages() const noexcept;
    };
}

#endif //NOVELIST_SETTINGSWINDOW_H
