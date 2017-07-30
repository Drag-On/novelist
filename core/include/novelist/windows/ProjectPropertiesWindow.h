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

namespace Ui {
    class ProjectPropertiesWindow;
}

namespace novelist {
    class ProjectPropertiesWindow : public QDialog {
    Q_OBJECT

    public:
        ProjectPropertiesWindow(QWidget* parent, Qt::WindowFlags f);

        ~ProjectPropertiesWindow() noexcept override;

        void retranslateUi();

        void setProperties(ProjectProperties const& properties);

        ProjectProperties properties() const;

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::ProjectPropertiesWindow> m_ui;
    };
}

#endif //NOVELIST_PROJECTPROPERTIESWINDOW_H
