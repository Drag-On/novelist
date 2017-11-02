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

namespace Ui {
    class SettingsDialog;
}

namespace novelist {
    class SettingsWindow : public QDialog {
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

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::SettingsDialog> m_ui;

        void onChangePage(QListWidgetItem* current, QListWidgetItem* previous);
    };
}

#endif //NOVELIST_SETTINGSWINDOW_H
