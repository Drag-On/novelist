/**********************************************************
 * @file   NoteEditWindow.h
 * @author jan
 * @date   10/20/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_NOTEEDITWINDOW_H
#define NOVELIST_NOTEEDITWINDOW_H

#include <memory>
#include <QtWidgets/QDialog>

namespace Ui {
    class NoteEditDialog;
}

namespace novelist {

    class NoteEditWindow : public QDialog {
        Q_OBJECT

    public:
        enum DialogCode { Rejected = QDialog::Rejected, Accepted, Removed };

        /**
         * @param allowRemove Shows a button to remove the note if flag is true
         * @param parent Parent window
         * @param f Window flags
         */
        explicit NoteEditWindow(bool allowRemove = false, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~NoteEditWindow() noexcept override;

        void setText(QString text) noexcept;

        QString text() const noexcept;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::NoteEditDialog> m_ui;

    };
}

#endif //NOVELIST_NOTEEDITWINDOW_H
