/**********************************************************
 * @file   MainWindow.h
 * @author jan
 * @date   8/12/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MAINWINDOW_H
#define NOVELIST_MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <memory>

namespace Ui {
    class MainWindow;
}

namespace novelist
{
    class MainWindow : public QMainWindow {
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

        ~MainWindow() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::MainWindow> m_ui;
    };
}

#endif //NOVELIST_MAINWINDOW_H
