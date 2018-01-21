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
#include <QtWidgets/QListWidgetItem>
#include <gsl/gsl>
#include <novelist_core_export.h>
#include "document/TextFormatManager.h"

namespace Ui {
    class TextFormatDialog;
}

namespace novelist {
    class NOVELIST_CORE_EXPORT TextFormatDialog : public QDialog {
    Q_OBJECT

    public:
        /**
         * @param mgr Text format manager
         * @param parent Parent window
         * @param f Window flags
         */
        explicit TextFormatDialog(gsl::not_null<TextFormatManager*> mgr, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~TextFormatDialog() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

        void accept() override;

        void reject() override;

        void apply();

    protected:
        void changeEvent(QEvent* event) override;

    private slots:
        void onAddTextFormat();
        void onRemoveTextFormat();
        void onItemChanged(QListWidgetItem* item);
        void onItemSelectionChanged();
        void onAlignLeft();
        void onAlignRight();
        void onAlignCenter();
        void onAlignFill();
        void onLeftMarginChanged(int val);
        void onRightMarginChanged(int val);
        void onTopMarginChanged(int val);
        void onBottomMarginChanged(int val);
        void onIndentChanged(int val);
        void onTextIndentChanged(int val);
        void onAutoTextIndentClicked();
        void onBold();
        void onItalic();
        void onUnderline();
        void onOverline();
        void onStrikethrough();
        void onSmallCaps();

    private:
        enum Roles {
            TextFormatIdRole = Qt::UserRole,
            TextFormatRole,
        };

        TextFormatManager::TextFormat* getActiveFormat();

        std::unique_ptr<Ui::TextFormatDialog> m_ui;
        TextFormatManager* m_mgr;
    };
}

#endif //NOVELIST_TEXTFORMATDIALOG_H
