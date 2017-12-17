/**********************************************************
 * @file   FindWidget.h
 * @author jan
 * @date   12/17/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_FINDWIDGET_H
#define NOVELIST_FINDWIDGET_H

#include <QtWidgets/QWidget>
#include <memory>

namespace Ui {
    class FindWidget;
}

namespace novelist {
    class FindWidget : public QWidget {
    public:
        explicit FindWidget(QWidget* parent = nullptr) noexcept;

    protected:
        void changeEvent(QEvent* event) override;

    private:
        std::unique_ptr<Ui::FindWidget> m_ui;
    };
}

#endif //NOVELIST_FINDWIDGET_H
