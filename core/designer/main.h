/**********************************************************
 * @file   main.h
 * @author jan
 * @date   7/30/17
 * ********************************************************
 * @brief
 * @details
 **********************************************************/
#ifndef NOVELIST_MAIN_H
#define NOVELIST_MAIN_H

#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>

class CoreWidgetsPlugin : public QObject, public QDesignerCustomWidgetCollectionInterface {
Q_OBJECT
    Q_PLUGIN_METADATA(IID
            "novelist.core.QDesignerCustomWidgetCollectionInterface")
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit CoreWidgetsPlugin(QObject* parent = nullptr);

    QList<QDesignerCustomWidgetInterface*> customWidgets() const override;

private:
    QList<QDesignerCustomWidgetInterface*> m_widgets;
};

#endif //NOVELIST_MAIN_H
