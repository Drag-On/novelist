/**********************************************************
 * @file   ProjectPropertiesWindow.h
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_PROJECTPROPERTIESWINDOW_H
#define NOVELIST_PROJECTPROPERTIESWINDOW_H

#include <QDialog>
#include <memory>
#include "model/ProjectModel.h"
#include <novelist_core_export.h>

namespace Ui {
    class ProjectPropertiesWindow;
}

namespace novelist {
    /**
     * Modal dialog used to edit project properties
     */
    class NOVELIST_CORE_EXPORT ProjectPropertiesWindow : public QDialog {
    Q_OBJECT

    public:
        /**
         * @param parent Parent window
         * @param f Window flags
         */
        ProjectPropertiesWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags{});

        ~ProjectPropertiesWindow() noexcept override;

        /**
         * Translate UI to new language
         */
        void retranslateUi();

        /**
         * Set all fields according to some project properties
         * @param properties Properties
         */
        void setProperties(ProjectProperties const& properties);

        /**
         * @return Currently set project properties
         */
        ProjectProperties properties() const;

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::ProjectPropertiesWindow> m_ui;
    };
}

#endif //NOVELIST_PROJECTPROPERTIESWINDOW_H
