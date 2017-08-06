/**********************************************************
 * @file   SceneTabWidget.h
 * @author jan
 * @date   8/6/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_SCENETABWIDGET_H
#define NOVELIST_SCENETABWIDGET_H

#include <QtWidgets/QTabWidget>
#include <QtCore/QFile>

namespace novelist {
    class SceneTabWidget : public QTabWidget {
    Q_OBJECT

    public:
        using QTabWidget::QTabWidget;
        
    };
}

#endif //NOVELIST_SCENETABWIDGET_H
