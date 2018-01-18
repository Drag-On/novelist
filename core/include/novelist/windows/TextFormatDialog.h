/**********************************************************
 * @file   TextFormatDialog.h
 * @author jan
 * @date   1/18/18
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_TEXTFORMATDIALOG_H
#define NOVELIST_TEXTFORMATDIALOG_H

#include <memory>
#include <QtWidgets/QDialog>
#include <novelist_core_export.h>

namespace Ui {
    class TextFormatDialog;
}

namespace novelist {
    class NOVELIST_CORE_EXPORT TextFormatDialog : public QDialog {
    Q_OBJECT

    public:
        /**
         * @param parent Parent window
         * @param f Window flags
         */
        explicit TextFormatDialog(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~TextFormatDialog() noexcept override;

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
        std::unique_ptr<Ui::TextFormatDialog> m_ui;
    };
}

#endif //NOVELIST_TEXTFORMATDIALOG_H
